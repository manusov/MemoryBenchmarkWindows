#include "Performer.h"

// Pointers to native DLL functions and dynamical imported WinAPI functions
SYSTEM_FUNCTIONS_LIST* Performer::pF;
NUMA_CONTROL_SET* Performer::pN;

// Status string
#define NS 81
char Performer::s[NS];

// Class constructor
Performer::Performer( SYSTEM_FUNCTIONS_LIST* pFunctions, NUMA_CONTROL_SET* nFunctions )
{
	pF = pFunctions;
	pN = nFunctions;
	snprintf( s, NS, "no data" );
}

// Class destructor
Performer::~Performer( )
{
	
}

// Helper for block size alignment
DWORD64 Performer::alignByFactor( DWORD64 value, DWORD64 factor )
{
	DWORD64 x = value % factor;
	if ( x )
	{
		x = factor - x;
	}
	else
	{
		x = 0;
	}
	value += x;
	return value;
}

/* // ********** DEBUG SUPPORT **********
// Print hex value = x to output string = s with size limit = n
int print64debug( char* s, size_t n, DWORD64 x )
{
	DWORD32 xLow = x;
	DWORD32 xHigh = ( x >> 16 ) >> 16;
	int m = snprintf( s, n, "%08X%08Xh", xHigh, xLow );
	return m;
}
// ********** DEBUG **********  */

// This procedure used as callback for threads run by OS
DWORD WINAPI threadEntry( LPVOID threadControl )
{
	THREAD_CONTROL_ENTRY* p = ( THREAD_CONTROL_ENTRY* )threadControl;

/* // ********** DEBUG, IMPORTANT POINT FOR TOPOLOGY SUPPORT DEBUG ! **********
char sd1[80], sd2[80];
DWORD64 wd1 = ( DWORD64 ) ( p->base1 );
DWORD64 wd2 = ( DWORD64 ) ( p->optimalGaff.Mask );
int wd3 = p->optimalGaff.Group;
print64debug( sd1, 80, wd1 );
print64debug( sd2, 80, wd2 );
printf("  [ THREADS DEBUG DUMP: base=%s, mask=%s, group=%d ]\n", sd1, sd2, wd3 );
// p->originalGaff.Mask = 0x1111;
// ********** DEBUG ********** */

    if ( p->optimalGaff.Mask != 0 )  // If zero mask, affinitization skipped
    {
    	if ( p->API_SetThreadGroupAffinity != NULL )
    	{	// This branch if Processor Groups supported, 256+ logical processors
			( p->API_SetThreadGroupAffinity ) ( p->threadHandle, &( p->optimalGaff ), &( p->originalGaff ) );
		}
		else if ( p->API_SetThreadAffinityMask != NULL )
		{	// This branch if Processor Groups not supported, maximum 64 logical processors
			( p->API_SetThreadAffinityMask ) ( p->threadHandle, p->optimalGaff.Mask );
		}
	}

/* // ********** DEBUG **********
char sd10[80];
DWORD64 wd10 = ( DWORD64 ) ( p->originalGaff.Mask );
print64debug( sd10, 80, wd10 );
printf("\n  [ THREADS DEBUG DUMP: updated original affinity mask=%s ]\n\n", sd10 );
// ********** DEBUG ********** */
	
	while ( TRUE )
    {
		// Thread parameters: handles
    	HANDLE rxHandle = p->rxEventHandle;
    	HANDLE txHandle = p->txEventHandle;
    	// Thread parameters: performance variables
        DWORD rwMethodSelect = p->methodId;
        LPVOID bufferAlignedSrc = p->base1;
        LPVOID bufferAlignedDst = p->base2;
        SIZE_T instructionsCount = p->sizeInstructions;
        SIZE_T repeatsCount = p->measurementRepeats;
        DWORD64 deltaTSC = 0;
        DWORD result = 0;
        // Thread main work
        ( p->DLL_PerformanceGate )
        ( rwMethodSelect ,  ( BYTE* )bufferAlignedSrc , ( BYTE* )bufferAlignedDst ,
          instructionsCount , repeatsCount , &deltaTSC );
		// Thread coordination
        SetEvent( rxHandle );
        WaitForSingleObject( txHandle, INFINITE );
        ResetEvent( txHandle );
	}
}

// Initializing list, fill given (allocated by caller) thread list memory
DWORD Performer::buildThreadsList( BENCHMARK_SCENARIO* pScenario )
{
	DWORD error;
	// Initializing variables
	int i = 0;
	int n = pScenario->nThreadsList;
	THREAD_CONTROL_ENTRY* pThreads = pScenario->pThreadsList;
	HANDLE* pSignals = pScenario->pSignalsList;
	DWORD_PTR b1 = 0;
	DWORD_PTR b2 = pScenario->maxSizeBytes;
	DWORD_PTR ba = pScenario->maxSizeBytes * 2;
	DWORD_PTR mp = NULL;
	
	DWORD allocType = MEM_COMMIT;
	size_t allocSize = ba * n;                   // ( memory per thread ) * ( number of threads )
	DWORD alignFactor = pScenario->pageSize;
	allocSize = alignByFactor( allocSize, alignFactor );  // alignment by selected page size
	
	if ( ( pScenario->numaMode != NUMA_LOCAL ) && ( pScenario->numaMode != NUMA_REMOTE ) )
	{
		// Initializing memory allocation options, this branch actual for non-NUMA only
		if ( pScenario->pagingMode != 0 )
		{
			allocType |= MEM_LARGE_PAGES;
		}
		// Memory allocation, this branch actual for non-NUMA only
		mp = ( DWORD_PTR )VirtualAlloc( NULL, allocSize, allocType, PAGE_READWRITE );
		if ( mp == NULL )
		{
			error = GetLastError( );
			snprintf( s, NS, "memory allocation" );
			return error;
		}
	}
	
	// Cycle for threads list entries
	for( i=0; i<n; i++ )
	{
		pThreads->rxEventHandle = CreateEvent( NULL, TRUE, FALSE, NULL );   // Event Handle for this thread operation complete signal
		if ( pThreads->rxEventHandle == NULL )
		{
			error = GetLastError( );
			snprintf( s, NS, "create receive complete event" );
			return error;
		}

		pThreads->txEventHandle = CreateEvent( NULL, TRUE, FALSE, NULL );   // Event Handle for this thread continue enable signal
		if ( pThreads->txEventHandle == NULL )
		{
			error = GetLastError( );
			snprintf( s, NS, "create transmit continue event" );
			return error;
		}
		
		pThreads->threadHandle = CreateThread( NULL, 0, threadEntry, pThreads, CREATE_SUSPENDED, 0 );   // This thread handle
		if ( pThreads->threadHandle == NULL )
		{
			error = GetLastError( );
			snprintf( s, NS, "create thread" );
			return error;
		}
		
		if ( ( pScenario->numaMode != NUMA_LOCAL ) && ( pScenario->numaMode != NUMA_REMOTE ) )
		{
			pThreads->base1 = ( LPVOID )( mp + b1 );
			pThreads->base2 = ( LPVOID )( mp + b2 );
			if ( i == 0 )
			{
				pThreads->trueBase = ( LPVOID )mp;
			}
			else
			{
				pThreads->trueBase = NULL;
			}
		}
		
		pThreads->sizeInstructions = pScenario->currentSizeInstructions;
		pThreads->measurementRepeats = pScenario->measurementRepeats;
		// pThreads->optimalGaff.Mask = 0;  // Affinity masks = f(HT, NUMA) initialization moved to NumaTopology.cpp
		// pThreads->optimalGaff.Group = 0;
		// pThreads->originalGaff.Mask = 0;
		// pThreads->originalGaff.Group = 0;
		pThreads->largePages = 0;
		pThreads->methodId = pScenario->methodId;
		pThreads->DLL_PerformanceGate = pF->DLL_PerformanceGate;
		//
		pThreads->API_SetThreadAffinityMask = pN->API_SetThreadAffinityMask;
		pThreads->API_SetThreadGroupAffinity = pN->API_SetThreadGroupAffinity;
		//
		b1 += ba;
		b2 += ba;
		*pSignals = pThreads->rxEventHandle;  // required separate sequental handles list
		
		pThreads++;
		pSignals++;
	}
	// Done if no errors
	return 0;	
}

// De-initializing list, release objects, but list allocated by caller 
DWORD Performer::releaseThreadsList( BENCHMARK_SCENARIO* pScenario )
{
	DWORD error;
	// Initializing variables
	int i = 0;
	int n = pScenario->nThreadsList;
	THREAD_CONTROL_ENTRY* p = pScenario->pThreadsList;
	HANDLE handle;
	BOOL status;

	// Cycle for threads list entries
	for( i=0; i<n; i++ )
	{
		handle = p->threadHandle;
		if ( handle != NULL )
		{
			status = TerminateThread( handle, 0 );  // Terminate thread
			if ( !status )
			{
				error = GetLastError( );
				snprintf( s, NS, "terminate thread" );
				return error;
			}
		}

		handle = p->rxEventHandle;
		if ( handle != NULL )
		{
			status = CloseHandle( handle );  // Close event handle
			if ( !status )
			{
				error = GetLastError( );
				snprintf( s, NS, "close receive complete event handle" );
				return error;
			}
		}

		handle = p->txEventHandle;
		if ( handle != NULL )
		{
			status = CloseHandle( handle );  // Close event handle
			if ( !status )
			{
				error = GetLastError( );
				snprintf( s, NS, "close transmit continue event handle" );
				return error;
			}
		}

		handle = p->threadHandle;
		if ( handle != NULL )
		{
			status = CloseHandle( handle );  // Close thread handle
			if ( !status )
			{
				error = GetLastError( );
				snprintf( s, NS, "close thread handle" );
				return error;
			}
		}

		p++;
	}
	// Done if no errors
	return 0;	
}

// First run performance pattern
DWORD Performer::threadsRun( BENCHMARK_SCENARIO* pScenario, DWORD64 &deltaTsc )
{
	DWORD error;
	// Initializing variables
	int i = 0;
	int n = pScenario->nThreadsList;
	THREAD_CONTROL_ENTRY* pThreads = pScenario->pThreadsList;
	HANDLE* pSignals = pScenario->pSignalsList;
	HANDLE handle;
	DWORD result;
    DWORD64 tsc1 = 0;
    DWORD64 tsc2 = 0;
    
    // Get start TSC
    ( pF->DLL_ExecuteRdtsc )( &tsc1 );

	// Cycle for threads list entries
	for( i=0; i<n; i++ )
	{
		handle = pThreads->threadHandle;   // Get thread handle
        result = ResumeThread( handle );   // Resume this thread, means Run for this context
        if( result == -1 )
        {
        	error = GetLastError( );
			snprintf( s, NS, "resume thread" );
        	return error;
		}
		pThreads++;
	}
	
    // Wait for signals from threads
    result = WaitForMultipleObjects( n, pSignals, TRUE, 60000 );
    if ( result == WAIT_FAILED )
    {
        error = GetLastError( );
		snprintf( s, NS, "wait failed" );
        return error;
	}
	if ( result == WAIT_TIMEOUT )
	{
		error = GetLastError( );
		snprintf( s, NS, "wait timeout" );
		return error;
	}
	
    // Get end TSC, calculate delta-TSC, update result variable
    ( pF->DLL_ExecuteRdtsc )( &tsc2 );
    deltaTsc = tsc2 - tsc1;
	
	// Done if no errors
	return 0;	
}

// Continued (non-first) run performance pattern
DWORD Performer::threadsRestart( BENCHMARK_SCENARIO* pScenario, DWORD64 &deltaTsc )
{
	DWORD error;
	// Initializing variables
	int i = 0;
	int n = pScenario->nThreadsList;
	THREAD_CONTROL_ENTRY* pThreads1 = pScenario->pThreadsList;
	THREAD_CONTROL_ENTRY* pThreads2 = pThreads1;
	HANDLE* pSignals = pScenario->pSignalsList;
	HANDLE handle;
	BOOL status;
	DWORD result;
    DWORD64 tsc1 = 0;
    DWORD64 tsc2 = 0;
    
	// Cycle for threads list entries
	for( i=0; i<n; i++ )
	{
		handle = pThreads1->rxEventHandle;    // Get event handle
        status = ResetEvent( handle );        // Reset this event, enable continue thread execution
        if( !status )
        {
        	error = GetLastError( );
			snprintf( s, NS, "reset event after acknowledge" );
        	return error;
		}
		pThreads1++;
	}

    // Get start TSC
    ( pF->DLL_ExecuteRdtsc )( &tsc1 );

	// Cycle for threads list entries
	for( i=0; i<n; i++ )
	{
		handle = pThreads2->txEventHandle;    // Get event handle
        status = SetEvent( handle );          // Set this event, enable continue thread execution
        if( !status )
        {
        	error = GetLastError( );
			snprintf( s, NS, "set event for continue" );
        	return error;
		}
		pThreads2++;
	}
	
    // Wait for new signals from threads
    result = WaitForMultipleObjects( n, pSignals, TRUE, 60000 );
    if ( result == WAIT_FAILED )
    {
        error = GetLastError( );
		snprintf( s, NS, "wait failed" );
        return error;
	}
	if ( result == WAIT_TIMEOUT )
	{
		error = GetLastError( );
		snprintf( s, NS, "wait timeout" );
		return error;
	}
	
    // Get end TSC, calculate delta-TSC, update result variable
    ( pF->DLL_ExecuteRdtsc )( &tsc2 );
    deltaTsc = tsc2 - tsc1;
	
	// Done if no errors
	return 0;	
}	

// Update threads list for block size changes at measurement cycle
void Performer::threadsUpdate( BENCHMARK_SCENARIO* pScenario, DWORD64 x )
{
	// Updates at header
	pScenario->currentSizeInstructions = x;

	// Initializing variables
	int i = 0;
	int n = pScenario->nThreadsList;
	THREAD_CONTROL_ENTRY* pThreads = pScenario->pThreadsList;
	
	// Cycle for threads entries
	for( i=0; i<n; i++ )
	{
		pThreads->sizeInstructions = pScenario->currentSizeInstructions;
		pThreads++;
	}
}

// Update threads list for number of measurement repeats changes by calibration
void Performer::repeatsUpdate( BENCHMARK_SCENARIO* pScenario, DWORD64 x )
{
	// Updates at header
	pScenario->measurementRepeats = x;

	// Initializing variables
	int i = 0;
	int n = pScenario->nThreadsList;
	THREAD_CONTROL_ENTRY* pThreads = pScenario->pThreadsList;
	
	// Cycle for threads entries
	for( i=0; i<n; i++ )
	{
		pThreads->measurementRepeats = pScenario->measurementRepeats;
		pThreads++;
	}
}

// Update threads list for r/w method-routine changes by latency measure algorithm
void Performer::routineUpdate( BENCHMARK_SCENARIO* pScenario, DWORD64 x )
{
	// Initializing variables
	int i = 0;
	int n = pScenario->nThreadsList;
	THREAD_CONTROL_ENTRY* pThreads = pScenario->pThreadsList;
	
	// Cycle for threads entries
	for( i=0; i<n; i++ )
	{
		pThreads->methodId = x;
		pThreads++;
	}
}

// Free memory, allocated for threads list
BOOL Performer::freeThreadsList( BENCHMARK_SCENARIO* pScenario )
{
	// Initializing variables
	BOOL status = TRUE;
	int i = 0;
	int n = pScenario->nThreadsList;
	THREAD_CONTROL_ENTRY* pThreads = pScenario->pThreadsList;
	
	// Cycle for threads entries
	for( i=0; i<n; i++ )
	{
		LPVOID trueBase = pThreads->trueBase;
		if ( trueBase != NULL )
		{
			BOOL status1 = VirtualFree( trueBase, 0, MEM_RELEASE );
			status = status && status1;
		}
		pThreads++;
	}
	
	return status;
}

// Method returns status string, valid if error returned
char* Performer::getStatusString( )
{
    return s;
}



