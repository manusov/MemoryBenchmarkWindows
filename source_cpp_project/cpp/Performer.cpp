#include "Performer.h"

// Pointer to native DLL
SYSTEM_FUNCTIONS_LIST* Performer::pF;

// Status string
#define NS 81
char Performer::s[NS];

// Class constructor
Performer::Performer( SYSTEM_FUNCTIONS_LIST* pFunctions )
{
	pF = pFunctions;
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

// This procedure used as callback for threads run by OS
DWORD WINAPI threadEntry( LPVOID threadControl )
{
    while ( TRUE )
    {
		// DEBUG: printf( "DEBUG, P=%p\n", threadControl );
		
		// Thread parameters: handles
    	HANDLE rxHandle = ( ( THREAD_CONTROL_ENTRY* )threadControl )->rxEventHandle;
    	HANDLE txHandle = ( ( THREAD_CONTROL_ENTRY* )threadControl )->txEventHandle;
    	// Thread parameters: performance variables
        DWORD rwMethodSelect = ( ( THREAD_CONTROL_ENTRY* )threadControl )->methodId;
        LPVOID bufferAlignedSrc = ( ( THREAD_CONTROL_ENTRY* )threadControl )->base1;
        LPVOID bufferAlignedDst = ( ( THREAD_CONTROL_ENTRY* )threadControl )->base2;
        SIZE_T instructionsCount = ( ( THREAD_CONTROL_ENTRY* )threadControl )->sizeInstructions;
        SIZE_T repeatsCount = ( ( THREAD_CONTROL_ENTRY* )threadControl )->measurementRepeats;
        DWORD64 deltaTSC = 0;
        DWORD result = 0;
        
        // Thread main work
        ( ( ( THREAD_CONTROL_ENTRY* )threadControl )->DLL_PerformanceGate )
        ( rwMethodSelect ,  ( BYTE* )bufferAlignedSrc , ( BYTE* )bufferAlignedDst ,
          instructionsCount , repeatsCount , &deltaTSC );

		// Thread coordination
        SetEvent( rxHandle );
        WaitForSingleObject( txHandle, INFINITE );
        ResetEvent( txHandle );
        /*
        result = WAIT_OBJECT_0;
        while ( result == WAIT_OBJECT_0 )
        {
            result = WaitForSingleObject( handle, 0 );
        }
        */
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
	DWORD_PTR mp;
	
	DWORD allocType = MEM_COMMIT;
	size_t allocSize = ba * n;                   // ( memory per thread ) * ( number of threads )
	DWORD alignFactor = pScenario->pageSize;
	allocSize = alignByFactor( allocSize, alignFactor );  // alignment by selected page size
	
	if ( pScenario->pagingMode != 0 )
	{
		allocType |= MEM_LARGE_PAGES;
	}
	
	// Memory allocation, for non-NUMA
	mp = ( DWORD_PTR )VirtualAlloc( NULL, allocSize, allocType, PAGE_READWRITE );
	if ( mp == NULL )
	{
		error = GetLastError( );
		snprintf( s, NS, "memory allocation" );
		return error;
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
		
		pThreads->sizeInstructions = pScenario->currentSizeInstructions;
		pThreads->measurementRepeats = pScenario->measurementRepeats;
		pThreads->threadAffinity = 0;
		pThreads->trueAffinity = 0;
		pThreads->threadGroup = 0;
		pThreads->trueGroup = 0;
		pThreads->largePages = 0;
		pThreads->methodId = pScenario->methodId;
		pThreads->DLL_PerformanceGate = pF->DLL_PerformanceGate;
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

// Method returns status string, valid if error returned
char* Performer::getStatusString( )
{
    return s;
}



