/*
	MEMORY PERFORMANCE ENGINE FRAMEWORK.
		  Threads Builder class.
*/

#include "ThreadsBuilder.h"

// Pointer to global control set of functions 
FUNCTIONS_LIST* ThreadsBuilder::f = NULL;
// System threads data
SYSTEM_THREADS_DATA ThreadsBuilder::threadsData;
// Status string
#define NS 81
char ThreadsBuilder::s[NS];
// Width and up string for threads list table
#define TABLE_WIDTH_THREADS 79
const char* ThreadsBuilder::tableUpThreads =
" ID  (values hex)\n";

// Threads control constant
#define THREAD_TIMEOUT 60000

// Class constructor, 
// allocate memory for lists, initialize pointers
ThreadsBuilder::ThreadsBuilder(FUNCTIONS_LIST* functions)
{
	// Blank status string
	snprintf(s, NS, "no data");
	// global initialization and pre-blank output
	f = functions;
	threadsData.threadCount = 0;
	threadsData.pThreadsList = NULL;
	threadsData.pSignalsList = NULL;
	// allocate memory for threads list
	int n = MAXIMUM_THREADS;
	int mT = sizeof(THREAD_CONTROL_ENTRY) * n;
	THREAD_CONTROL_ENTRY* pT = (THREAD_CONTROL_ENTRY*)malloc(mT);
	threadsData.pThreadsList = pT;   // T = Threads
	// allocate memory for sequental events handles list
	int mE = sizeof(HANDLE) * n;
	HANDLE* pE = (HANDLE*)malloc(mE);
	threadsData.pSignalsList = pE;   // E = Events
	// Blank lists
	for (int i = 0; i < n; i++)
	{
		blankThread(pT);
		*pE = NULL;
		pT++;
		pE++;
	}
}

// Class destructor, release memory, allocated for lists
ThreadsBuilder::~ThreadsBuilder()
{
	if (threadsData.pThreadsList != NULL)
	{
		free(threadsData.pThreadsList);
	}
	if (threadsData.pSignalsList != NULL)
	{
		free(threadsData.pSignalsList);
	}
}

// Return pointer to threads data
SYSTEM_THREADS_DATA* ThreadsBuilder::getThreadsList()
{
	return &threadsData;
}

// Get text report
void ThreadsBuilder::getThreadsText(LPSTR& dst, size_t& max)
{
	// Created threads entries count and allocated memory base address
	int count = 0;
	int x1 = threadsData.threadCount;
	LPVOID x2 = threadsData.pThreadsList;
	count = snprintf(dst, max, "threads entries created = %d , list allocated at base = ", x1);
	dst += count;
	max -= count;
	AppLib::printPointer(dst, max, x2, TRUE);
	AppLib::printCrLf(dst, max);
	// Threads table
	THREAD_CONTROL_ENTRY* pT = threadsData.pThreadsList;
	if ((x1 > 0) && (x2 != NULL))
	{
		AppLib::printCrLf(dst, max);
		AppLib::printString(dst, max, tableUpThreads);
		AppLib::printLine(dst, max, TABLE_WIDTH_THREADS);
		// Cycle for NUMA nodes
		for (int i = 0; i < x1; i++)
		{
			AppLib::printString(dst, max, " ");
			AppLib::printCellNum(dst, max, i, 4);
			// Line 1
			AppLib::printString(dst, max, "thread=");
			AppLib::print64(dst, max, (DWORD64)(pT->threadHandle), FALSE);
			AppLib::printString(dst, max, " rxevent=");
			AppLib::print64(dst, max, (DWORD64)(pT->rxEventHandle), FALSE);
			AppLib::printString(dst, max, " txevent=");
			AppLib::print64(dst, max, (DWORD64)(pT->txEventHandle), FALSE);
			// Line 2
			AppLib::printString(dst, max, "\n     base1 =");
			AppLib::print64(dst, max, (DWORD64)(pT->base1), FALSE);
			AppLib::printString(dst, max, " base2  =");
			AppLib::print64(dst, max, (DWORD64)(pT->base2), FALSE);
			AppLib::printString(dst, max, " sizeins=");
			AppLib::print64(dst, max, (DWORD64)(pT->sizeInstructions), FALSE);
			// Line 3
			AppLib::printString(dst, max, "\n     repeat=");
			AppLib::print64(dst, max, (DWORD64)(pT->measurementRepeats), FALSE);
			AppLib::printString(dst, max, " large page mode =");
			AppLib::print16(dst, max, pT->largePagesMode, FALSE);
			AppLib::printString(dst, max, "    method id =");
			AppLib::print16(dst, max, pT->methodId, FALSE);
			// Line 4
			AppLib::printString(dst, max, "\n     optimal affinity group\\mask  = ");
			AppLib::print16(dst, max, pT->optimalGaff.Group, FALSE);
			AppLib::printString(dst, max, "\\");
			AppLib::print64(dst, max, (DWORD64)(pT->optimalGaff.Mask), FALSE);
			// Line 5
			AppLib::printString(dst, max, "\n     original affinity group\\mask = ");
			AppLib::print16(dst, max, pT->originalGaff.Group, FALSE);
			AppLib::printString(dst, max, "\\");
			AppLib::print64(dst, max, (DWORD64)(pT->originalGaff.Mask), FALSE);
			// Line 6
			AppLib::printString(dst, max, "\n     control set pointer          = ");
			AppLib::print64(dst, max, (DWORD64)(pT->pRoutines), FALSE);
			// Done thread entry
			AppLib::printCrLf(dst, max);
			AppLib::printCrLf(dst, max);
			pT++;
		}
		AppLib::printLine(dst, max, TABLE_WIDTH_THREADS);
	}
}

// Method returns status string, valid if error returned
char* ThreadsBuilder::getStatusString()
{
	return s;
}

// Thread callback handler and threads management functions

// This procedure used as callback for threads run by OS
// note this routine not declared at class header
DWORD WINAPI threadEntry(LPVOID threadControl)
{
	// Thread initialization part
	THREAD_CONTROL_ENTRY* p = (THREAD_CONTROL_ENTRY*)threadControl;
	FUNCTIONS_LIST* r = p->pRoutines;
	// This thread affinitization
	if (p->optimalGaff.Mask != 0)  // If zero mask, affinitization skipped
	{
		if (r->api_SetThreadGroupAffinity != NULL)
		{	// This branch if Processor Groups supported, 256+ logical processors
			(r->api_SetThreadGroupAffinity) (p->threadHandle, &(p->optimalGaff), &(p->originalGaff));
		}
		else if (r->api_SetThreadAffinityMask != NULL)
		{	// This branch if Processor Groups not supported, maximum 64 logical processors
			(r->api_SetThreadAffinityMask) (p->threadHandle, p->optimalGaff.Mask);
		}
	}
	// Thread execution cycle, after initialization part (already affinitized)
	while (TRUE)
	{
		// Thread parameters: handles
		HANDLE rxHandle = p->rxEventHandle;
		HANDLE txHandle = p->txEventHandle;
		// Thread parameters: performance variables
		DWORD rwMethodSelect = p->methodId;
		LPVOID bufferAlignedSrc = p->base1;
		LPVOID bufferAlignedDst = p->base2;
		SIZE_T instructionsCount = (SIZE_T)p->sizeInstructions;
		SIZE_T repeatsCount    = (SIZE_T)p->measurementRepeats;
		SIZE_T repeatsCountExt = p->measurementRepeats >> 32;
		DWORD64 deltaTSC = 0;
		DWORD result = 0;
		
		// Thread main work
		(r->dll_PerformanceGate)
			(rwMethodSelect, (BYTE*)bufferAlignedSrc, (BYTE*)bufferAlignedDst,
             instructionsCount, repeatsCount, repeatsCountExt, &deltaTSC);
		
		// Thread coordination
		SetEvent(rxHandle);
		WaitForSingleObject(txHandle, INFINITE);
		ResetEvent(txHandle);
	}
}

// Initializing list of execution threads, set constants not changed per iterations
BOOL ThreadsBuilder::buildThreadsList
(INPUT_CONSTANTS* pInputC, INPUT_VARIABLES* pInputV, SYSTEM_NUMA_DATA* pDomain)
{
	// Get and verify Domains / Threads ratio
	int n = pInputC->threadCount;
	int m = pDomain->nodeCount;
	if ((n % m) != 0)
	{
		snprintf(s, NS, "count threads per domain");
		return FALSE;
	}
	int threadsPerDomain = n / m;
	// Initializing thread memory parameters
	DWORD64 memoryBase = 0;
	DWORD64 memoryPerThread = pInputC->maxSizeBytes * 2;  //  OLD:  pInputC->maxSizeBytes;
	DWORD64 memoryPerRegion = memoryPerThread / 2;        //  OLD:  memoryPerThread / 2;
	// Initializing threads build cycle
	threadsData.threadCount = n;
	THREAD_CONTROL_ENTRY* pT = threadsData.pThreadsList;  // pT = pointer to threads list
	NUMA_NODE_ENTRY* pN = pDomain->nodeList;
	HANDLE* pE = threadsData.pSignalsList;                // pE = pointer to events list, sequental for API
	// Initialize system affinity
	SYSTEM_INFO x;
	GetSystemInfo(&x);
	DWORD64 systemAffinity = x.dwActiveProcessorMask;
	// Cycle for domains
	for (int j = 0; j < m; j++)
	{
		memoryBase = (DWORD64)(pN->baseAtNode);
		// Cycle for threads
		for (int i = 0; i < n; i++)
		{
			// Assign this thread handle
			pT->threadHandle = CreateThread(NULL, 0, threadEntry, pT, CREATE_SUSPENDED, 0);
			if (pT->threadHandle == NULL)
			{
				snprintf(s, NS, "create thread");
				return FALSE;
			}
			// Assign event Handle for this thread operation complete signal
			pT->rxEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (pT->rxEventHandle == NULL)
			{
				snprintf(s, NS, "create receive complete event");
				return FALSE;
			}
			// Assign event Handle for this thread continue enable signal
			pT->txEventHandle = CreateEvent(NULL, TRUE, FALSE, NULL);
			if (pT->txEventHandle == NULL)
			{
				snprintf(s, NS, "create transmit continue event");
				return FALSE;
			}
			// Assign base address for two regions, typical source and destination
			pT->base1 = (LPVOID)(memoryBase);
			pT->base2 = (LPVOID)(memoryBase + memoryPerRegion);
			memoryBase += memoryPerThread;
			// Operation size in units = instructions
			pT->sizeInstructions = pInputV->currentSizeInstructions;
			// Number of measurement repeats
			pT->measurementRepeats = pInputV->currentMeasurementRepeats;
			// Large page mode
			pT->largePagesMode = pInputC->pagingMode;
			// Method selector by CPU instruction set
			pT->methodId = pInputV->currentMethodId;
			// Optimal affinity mask (or non-optimal if NUMA REMOTE mode)
			DWORD64 mask = pN->nodeGaff.Mask;
			if (pInputC->htMode == HT_NOT_USED)
			{
				if (mask == 0)
				{
					mask = systemAffinity;
				}
				DWORD64 mask1 = mask & 0x5555555555555555L;
				if (mask1 != 0)
				{
					mask = mask1;
				}
			}
			pT->optimalGaff.Mask = (KAFFINITY)mask;
			// continue for HT-unaware part
			pT->optimalGaff.Group = pN->nodeGaff.Group;
			pT->optimalGaff.Reserved[0] = pN->nodeGaff.Reserved[0];
			pT->optimalGaff.Reserved[1] = pN->nodeGaff.Reserved[1];
			pT->optimalGaff.Reserved[2] = pN->nodeGaff.Reserved[2];
			// Blank space for store original affinity mask in the per-thread callback routines
			blankAffinity(&pT->originalGaff);
			// Control set pointer
			pT->pRoutines = f;
			// Update handle in the secondary (sequental) list
			*pE = pT->rxEventHandle;
			// Modify pointers per thread
			pT++;
			pE++;
		}
		// Modify pointer per node
		pN++;
	}
	return TRUE;
}

// Update list of execution threads, set variables changed per iterations
BOOL ThreadsBuilder::updateThreadsList(INPUT_VARIABLES* pInputV)
{
	THREAD_CONTROL_ENTRY* p = threadsData.pThreadsList;
	int n = threadsData.threadCount;
	if ((p != NULL) && (n > 0))
	{
		for (int i = 0; i < n; i++)
		{
			p->sizeInstructions = pInputV->currentSizeInstructions;
			p->measurementRepeats = pInputV->currentMeasurementRepeats;
			p->methodId = pInputV->currentMethodId;
			p++;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// Release threads list, close handles, 
// note don't release memory because memory release in domains management routines
BOOL ThreadsBuilder::releaseThreadsList()
{
	// Initializing variables
	int n = threadsData.threadCount;
	THREAD_CONTROL_ENTRY* p = threadsData.pThreadsList;
	HANDLE handle;
	BOOL status;
	// Cycle for threads list entries
	for (int i = 0; i < n; i++)
	{
		// Terminate thread
		handle = p->threadHandle;
		if (handle != NULL)
		{
			status = TerminateThread(handle, 0);
			if (!status)
			{
				snprintf(s, NS, "terminate thread");
				return FALSE;
			}
		}
		// Close event handle
		handle = p->rxEventHandle;
		if (handle != NULL)
		{
			status = CloseHandle(handle);
			if (!status)
			{

				snprintf(s, NS, "close receive complete event handle");
				return FALSE;
			}
		}
		// Close event handle
		handle = p->txEventHandle;
		if (handle != NULL)
		{
			status = CloseHandle(handle);
			if (!status)
			{

				snprintf(s, NS, "close transmit continue event handle");
				return FALSE;
			}
		}
		// Close thread handle
		handle = p->threadHandle;
		if (handle != NULL)
		{
			status = CloseHandle(handle);
			if (!status)
			{
				snprintf(s, NS, "close thread handle");
				return FALSE;
			}
		}
		// Blank thread entry, modify pointer, select next thread entry
		blankThread(p);
		p++;
	}
	// Done if no errors
	return TRUE;
}

// Run execution threads, for first call after initialization
BOOL ThreadsBuilder::runThreads(OUTPUT_VARIABLES* pOutputV)
{
	// Initializing variables
	int n = threadsData.threadCount;
	THREAD_CONTROL_ENTRY* pThreads = threadsData.pThreadsList;
	HANDLE* pSignals = threadsData.pSignalsList;
	HANDLE handle;
	DWORD result;
	DWORD64 tsc1 = 0;
	DWORD64 tsc2 = 0;
	// Get start TSC
	(f->dll_ExecuteRdtsc)(&tsc1);
	// Cycle for threads list entries
	for (int i = 0; i < n; i++)
	{
		handle = pThreads->threadHandle;   // Get thread handle
		result = ResumeThread(handle);   // Resume this thread, means Run for this context
		if (result == -1)
		{
			snprintf(s, NS, "resume thread");
			return FALSE;
		}
		pThreads++;
	}
	// Wait for signals from threads
	result = WaitForMultipleObjects(n, pSignals, TRUE, THREAD_TIMEOUT);
	if (result == WAIT_FAILED)
	{
		snprintf(s, NS, "wait failed");
		return FALSE;
	}
	if (result == WAIT_TIMEOUT)
	{
		snprintf(s, NS, "wait timeout");
		return FALSE;
	}
	// Get end TSC, calculate delta-TSC, update result variable
	(f->dll_ExecuteRdtsc)(&tsc2);
	pOutputV->resultDeltaTsc = tsc2 - tsc1;
	// Done if no errors
	return TRUE;
}

// Restart execution threads, for second and next calls after initialization
BOOL ThreadsBuilder::restartThreads(OUTPUT_VARIABLES* pOutputV)
{
	// Initializing variables
	int n = threadsData.threadCount;
	THREAD_CONTROL_ENTRY* pThreads1 = threadsData.pThreadsList;
	THREAD_CONTROL_ENTRY* pThreads2 = pThreads1;
	HANDLE* pSignals = threadsData.pSignalsList;
	HANDLE handle;
	BOOL status;
	DWORD result;
	DWORD64 tsc1 = 0;
	DWORD64 tsc2 = 0;
	// Cycle for threads list entries
	for (int i = 0; i < n; i++)
	{
		handle = pThreads1->rxEventHandle;    // Get event handle
		status = ResetEvent(handle);        // Reset this event, enable continue thread execution
		if (!status)
		{
			snprintf(s, NS, "reset event after acknowledge");
			return FALSE;
		}
		pThreads1++;
	}
	// Get start TSC
	(f->dll_ExecuteRdtsc)(&tsc1);
	// Cycle for threads list entries
	for (int i = 0; i < n; i++)
	{
		handle = pThreads2->txEventHandle;   // Get event handle
		status = SetEvent(handle);           // Set this event, enable continue thread execution
		if (!status)
		{
			snprintf(s, NS, "set event for continue");
			return FALSE;
		}
		pThreads2++;
	}
	// Wait for new signals from threads
	result = WaitForMultipleObjects(n, pSignals, TRUE, THREAD_TIMEOUT);
	if (result == WAIT_FAILED)
	{
		snprintf(s, NS, "wait failed");
		return FALSE;
	}
	if (result == WAIT_TIMEOUT)
	{
		snprintf(s, NS, "wait timeout");
		return FALSE;
	}
	// Get end TSC, calculate delta-TSC, update result variable
	(f->dll_ExecuteRdtsc)(&tsc2);
	pOutputV->resultDeltaTsc = tsc2 - tsc1;
	// Done if no errors
	return TRUE;
}

// Helpers methods

// Blank thread control entry
void ThreadsBuilder::blankThread(THREAD_CONTROL_ENTRY* pT)
{

	pT->threadHandle = NULL;             // Thread Handle for execution thread
	pT->rxEventHandle = NULL;            // Event Handle for daughter thread operation complete signal
	pT->txEventHandle = NULL;            // Event Handle for daughter thread operation continue enable signal
	pT->base1 = NULL;                    // Source for read, destination for write and modify
	pT->base2 = NULL;                    // Destination for copy
	pT->sizeInstructions = 0;            // Block size, units = instructions, for benchmarking
	pT->measurementRepeats = 0;          // Number of measurement repeats
	pT->largePagesMode = 0;              // Bit D0=Large Pages, other bits [1-31/63] = reserved
	pT->methodId = 0;                    // Entry point to Target operation method subroutine ID
	blankAffinity(&pT->optimalGaff);   // Affinity mask and group id, OPTIMAL for this thread
	blankAffinity(&pT->originalGaff);  // Affinity mask and group id, ORIGINAL for this thread
	pT->pRoutines = 0;                   // Pointers for dynamical import and this application DLL functions
}

// Blank group affinity structure
void ThreadsBuilder::blankAffinity(GROUP_AFFINITY* pA)
{
	pA->Mask = 0;
	pA->Group = 0;
	pA->Reserved[0] = 0;  // If not blanked, API returns error
	pA->Reserved[1] = 0;
	pA->Reserved[2] = 0;
}
