/*

MEMORY PERFORMANCE ENGINE (MPE) FRAMEWORK.
-------------------------------------------
System informationand performance analysing application,
include console version of NCRB(NUMA CPUand RAM Benchmark).
See also memory benchmark projects :
https://github.com/manusov/NCRBv2
https://github.com/manusov/MemoryBenchmarkWindows
Experimental samples :
https://github.com/manusov/Prototyping
Special thanks for C++ lessons :
https://ravesli.com/uroki-cpp/

Class realization for execution threads management:
enumerate threads, run and stop execution.
TODO.

*/

#include "ThreadsBuilder.h"

DWORD WINAPI threadEntry(LPVOID threadControl);

ThreadsBuilder::ThreadsBuilder(FUNCTIONS_LIST* pf) : UDM(pf)
{
	// Blank status string.
	snprintf(statusString, APPCONST::MAX_TEXT_STRING, "No data.");
	threadsData.threadCount = 0;
	threadsData.pThreadsList = NULL;
	threadsData.pSignalsList = NULL;
	// Allocate memory for threads list.
	int n = APPCONST::MAXIMUM_THREADS;
	int mT = sizeof(THREAD_CONTROL_ENTRY) * n;
	THREAD_CONTROL_ENTRY* pT = (THREAD_CONTROL_ENTRY*)malloc(mT);
	threadsData.pThreadsList = pT;   // T = Threads
	// Allocate memory for sequental events handles list.
	int mE = sizeof(HANDLE) * n;
	HANDLE* pE = (HANDLE*)malloc(mE);
	threadsData.pSignalsList = pE;   // E = Events.
	// Blank lists.
	for (int i = 0; i < n; i++)
	{
		helperBlankThread(pT);
		pT++;
		pE++;
	}
}
ThreadsBuilder::~ThreadsBuilder()
{
	if (threadsData.pThreadsList)
	{
		free(threadsData.pThreadsList);
	}
	if (threadsData.pSignalsList)
	{
		free(threadsData.pSignalsList);
	}
}
void ThreadsBuilder::writeReport()
{
	// Created threads entries count and allocated memory base address
	char msg[APPCONST::MAX_TEXT_STRING];
	LPVOID x2 = threadsData.pThreadsList;
	snprintf(msg, APPCONST::MAX_TEXT_STRING,
		"Threads entries created = %d , list allocated at base = ",
		threadsData.threadCount);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	AppLib::storeHexPointer(msg, APPCONST::MAX_TEXT_STRING, threadsData.pThreadsList, TRUE);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	AppLib::writeColor("\r\n", APPCONST::TABLE_COLOR);
	// Threads table
	THREAD_CONTROL_ENTRY* pT = threadsData.pThreadsList;
	if ((threadsData.threadCount > 0) && (threadsData.pThreadsList))
	{
		AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
		AppLib::writeColor(
			" ID  (values hex)\r\n",
			APPCONST::TABLE_COLOR);
		AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
		// Cycle for threads.
		for (unsigned int i = 0; i < threadsData.threadCount; i++)
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, " %2d  ", i);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			// Line 1.
			AppLib::writeColor("thread=", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)(pT->threadHandle), FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor(" rxevent=", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)(pT->rxEventHandle), FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor(" txevent=", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)(pT->txEventHandle), FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			// Line 2.
			AppLib::writeColor("\r\n     base1 =", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)(pT->base1), FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor(" base2  =", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)(pT->base2), FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor(" sizeins=", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)(pT->sizeInstructions), FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			// Line 3.
			AppLib::writeColor("\r\n     repeat=", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)(pT->measurementRepeats), FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor(" large page mode =", APPCONST::VALUE_COLOR);
			AppLib::storeHex16(msg, APPCONST::MAX_TEXT_STRING, pT->largePagesMode, FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("    method id =", APPCONST::VALUE_COLOR);
			AppLib::storeHex16(msg, APPCONST::MAX_TEXT_STRING, pT->methodId, FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			// Line 4.
			AppLib::writeColor("\r\n     optimal affinity group\\mask  = ", APPCONST::VALUE_COLOR);
			AppLib::storeHex16(msg, APPCONST::MAX_TEXT_STRING, pT->optimalGaff.Group, FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("\\", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)(pT->optimalGaff.Mask), FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			// Line 5.
			AppLib::writeColor("\r\n     original affinity group\\mask = ", APPCONST::VALUE_COLOR);
			AppLib::storeHex16(msg, APPCONST::MAX_TEXT_STRING, pT->originalGaff.Group, FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("\\", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)(pT->originalGaff.Mask), FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			// Line 6.
			AppLib::writeColor("\n     control set pointer          = ", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)(pT->pRoutines), FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
			// Interval between threads.
			if (i != (threadsData.threadCount - 1))
			{
				AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
			}
			pT++;
		}
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
}
SYSTEM_THREADS_DATA* ThreadsBuilder::getThreadsList()
{
	return &threadsData;
}
// Initializing list of execution threads, set constants not changed per iterations.
BOOL ThreadsBuilder::buildThreadsList(INPUT_CONSTANTS* pInputC, INPUT_VARIABLES* pInputV, SYSTEM_NUMA_DATA* pDomain)
{
	BOOL status = TRUE;
	// Get and verify Domains / Threads ratio
	int n = pInputC->threadCount;
	int m = pDomain->nodeCount;
	if ((n % m) != 0)
	{
		snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Bad count threads per domain.");
		status = FALSE;
	}
	if (status)
	{
		int threadsPerDomain = n / m;
		// Initializing thread memory parameters
		DWORD64 memoryBase = 0;
		DWORD64 memoryPerThread = pInputC->maxSizeBytes * 2;
		DWORD64 memoryPerRegion = memoryPerThread / 2;
		// Initializing threads build cycle.
		threadsData.threadCount = n;
		THREAD_CONTROL_ENTRY* pT = threadsData.pThreadsList;  // pT = pointer to threads list
		NUMA_NODE_ENTRY* pN = pDomain->nodeList;
		HANDLE* pE = threadsData.pSignalsList;                // pE = pointer to events list, sequental for API
		// Initialize system affinity.
		SYSTEM_INFO x;
		GetSystemInfo(&x);
		DWORD64 systemAffinity = x.dwActiveProcessorMask;
		// Cycle for domains.
		for (int j = 0; j < m; j++)
		{
			memoryBase = (DWORD64)(pN->baseAtNode);
			// Cycle for threads.
			for (int i = 0; i < n; i++)
			{
				// Assign this thread handle
				pT->threadHandle = CreateThread(nullptr, 0, threadEntry, pT, CREATE_SUSPENDED, 0);
				if (pT->threadHandle == nullptr)
				{
					snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Create thread failed.");
					status = FALSE;
					break;
				}
				// Assign event Handle for this thread operation complete signal
				pT->rxEventHandle = CreateEvent(nullptr, TRUE, FALSE, nullptr);
				if (pT->rxEventHandle == nullptr)
				{
					snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Create receive complete event failed.");
					status = FALSE;
					break;
				}
				// Assign event Handle for this thread continue enable signal
				pT->txEventHandle = CreateEvent(nullptr, TRUE, FALSE, nullptr);
				if (pT->txEventHandle == nullptr)
				{
					snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Create transmit continue event failed.");
					status = FALSE;
					break;
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
				// Added for support C++ tests: vectors, intrinsics.
				pT->asmOrCpp = pInputV->asmOrCpp;
				// Flag for thread termination by return from callback routine.
				pT->terminateThread = pInputV->terminateThread;
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
				helperBlankAffinity(&pT->originalGaff);
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
	}
	return status;
}
// Update list of execution threads, set variables changed per iterations.
BOOL ThreadsBuilder::updateThreadsList(INPUT_VARIABLES* pInputV)
{
	BOOL status = FALSE;
	THREAD_CONTROL_ENTRY* p = threadsData.pThreadsList;
	int n = threadsData.threadCount;
	if (p && (n > 0))
	{
		for (int i = 0; i < n; i++)
		{
			p->sizeInstructions = pInputV->currentSizeInstructions;
			p->measurementRepeats = pInputV->currentMeasurementRepeats;
			p->methodId = pInputV->currentMethodId;
			p->asmOrCpp = pInputV->asmOrCpp;
			p->terminateThread = pInputV->terminateThread;
			p++;
		}
		status = TRUE;
	}
	return status;
}
// Release threads list, close handles, 
// note don't release memory because memory release in the domains management routines.
BOOL ThreadsBuilder::releaseThreadsList()
{
	// Initializing variables.
	int n = threadsData.threadCount;
	THREAD_CONTROL_ENTRY* p = threadsData.pThreadsList;
	HANDLE handle;
	BOOL status = TRUE;
	// Cycle for threads list entries.
	for (int i = 0; i < n; i++)
	{
		// Terminate thread.
		handle = p->threadHandle;
		if (handle)
		{
			status = ( (WaitForSingleObject(handle, APPCONST::THREAD_TIMEOUT) == WAIT_OBJECT_0) );
			if (!status)
			{
				snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Terminate thread failed.");
				status = FALSE;
				break;
			}
		}
		// Close event handle.
		handle = p->rxEventHandle;
		if (handle)
		{
			status = CloseHandle(handle);
			if (!status)
			{

				snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Close receive complete event handle failed.");
				status = FALSE;
				break;
			}
		}
		// Close event handle.
		handle = p->txEventHandle;
		if (handle)
		{
			status = CloseHandle(handle);
			if (!status)
			{

				snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Close transmit continue event handle failed.");
				status = FALSE;
				break;
			}
		}
		// Close thread handle.
		handle = p->threadHandle;
		if (handle)
		{
			status = CloseHandle(handle);
			if (!status)
			{
				snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Close thread handle failed.");
				status = FALSE;
				break;
			}
		}
		// Blank thread entry, modify pointer, select next thread entry.
		helperBlankThread(p);
		p++;
	}
	threadsData.threadCount = 0;
	return status;
}
// Run execution threads, for first call after initialization.
BOOL ThreadsBuilder::runThreads(OUTPUT_VARIABLES* pOutputV)
{
	// Initializing variables.
	BOOL status = TRUE;
	int n = threadsData.threadCount;
	THREAD_CONTROL_ENTRY* pThreads = threadsData.pThreadsList;
	HANDLE* pSignals = threadsData.pSignalsList;
	HANDLE handle;
	DWORD result;
	DWORD64 tsc1 = 0;
	DWORD64 tsc2 = 0;
	// Get start TSC.
	(f->dll_ExecuteRdtsc)(&tsc1);
	// Cycle for threads list entries.
	for (int i = 0; i < n; i++)
	{
		handle = pThreads->threadHandle;   // Get thread handle.
		result = ResumeThread(handle);   // Resume this thread, means Run for this context.
		if (result == -1)
		{
			snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Resume thread failed.");
			status = FALSE;
			break;
		}
		pThreads++;
	}
	if (status)
	{
		// Wait for signals from threads
		result = WaitForMultipleObjects(n, pSignals, TRUE, APPCONST::THREAD_TIMEOUT);
		if (result == WAIT_FAILED)
		{
			snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Wait failed.");
			status = FALSE;
		}
		if (result == WAIT_TIMEOUT)
		{
			snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Wait timeout.");
			status = FALSE;
		}
		// Get end TSC, calculate delta-TSC, update result variable.
		(f->dll_ExecuteRdtsc)(&tsc2);
		pOutputV->resultDeltaTsc = tsc2 - tsc1;
	}
	// Done if no errors.
	return status;
}
// Restart execution threads, for second and next calls after initialization.
BOOL ThreadsBuilder::restartThreads(OUTPUT_VARIABLES* pOutputV, BOOL waitWork)
{
	// Initializing variables.
	BOOL status = TRUE;
	int n = threadsData.threadCount;
	THREAD_CONTROL_ENTRY* pThreads1 = threadsData.pThreadsList;
	THREAD_CONTROL_ENTRY* pThreads2 = pThreads1;
	HANDLE* pSignals = threadsData.pSignalsList;
	HANDLE handle;
	DWORD result;
	DWORD64 tsc1 = 0;
	DWORD64 tsc2 = 0;
	// Cycle for threads list entries.
	for (int i = 0; i < n; i++)
	{
		handle = pThreads1->rxEventHandle;  // Get event handle.
		status = ResetEvent(handle);        // Reset this event, enable continue thread execution.
		if (!status)
		{
			snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Reset event after acknowledge.");
			status = FALSE;
			break;
		}
		pThreads1++;
	}
	if (status)
	{
		// Get start TSC.
		(f->dll_ExecuteRdtsc)(&tsc1);
		// Cycle for threads list entries.
		for (int i = 0; i < n; i++)
		{
			handle = pThreads2->txEventHandle;   // Get event handle.
			status = SetEvent(handle);           // Set this event, enable continue thread execution.
			if (!status)
			{
				snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Set event for continue.");
				status = FALSE;
				break;
			}
			pThreads2++;
		}
		if (status && waitWork)
		{
			// Wait for new signals from threads.
			result = WaitForMultipleObjects(n, pSignals, TRUE, APPCONST::THREAD_TIMEOUT);
			if (result == WAIT_FAILED)
			{
				snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Wait failed.");
				status = FALSE;
			}
			if (result == WAIT_TIMEOUT)
			{
				snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Wait timeout.");
				status = FALSE;
			}
		}
		// Get end TSC, calculate delta-TSC, update result variable.
		(f->dll_ExecuteRdtsc)(&tsc2);
		pOutputV->resultDeltaTsc = tsc2 - tsc1;
	}
	// Done if no errors.
	return status;
}

// Thread callback handler.
// This procedure used as callback for threads run by OS,
// note this routine is not declared at class header, it cannot direct use class members.
DWORD WINAPI threadEntry(LPVOID threadControl)
{
	// Thread initialization part
	THREAD_CONTROL_ENTRY* p = (THREAD_CONTROL_ENTRY*)threadControl;
	FUNCTIONS_LIST* r = p->pRoutines;
	// This thread affinitization
	if (p->optimalGaff.Mask != 0)  // If zero mask, affinitization skipped
	{
		if (r->api_SetThreadGroupAffinity)
		{	// This branch if Processor Groups supported, 256+ logical processors
			(r->api_SetThreadGroupAffinity) (p->threadHandle, &(p->optimalGaff), &(p->originalGaff));
		}
		else if (r->api_SetThreadAffinityMask)
		{	// This branch if Processor Groups not supported, maximum 64 logical processors
			(r->api_SetThreadAffinityMask) (p->threadHandle, p->optimalGaff.Mask);
		}
	}
	// Thread execution cycle, after initialization part (already affinitized).
	while (!p->terminateThread)
	{
		// Thread parameters: handles
		HANDLE rxHandle = p->rxEventHandle;
		HANDLE txHandle = p->txEventHandle;
		// Thread parameters: performance variables
		DWORD rwMethodSelect = p->methodId;
		LPVOID bufferAlignedSrc = p->base1;
		LPVOID bufferAlignedDst = p->base2;
		SIZE_T instructionsCount = (SIZE_T)p->sizeInstructions;
		SIZE_T repeatsCount = (SIZE_T)p->measurementRepeats;
		SIZE_T repeatsCountExt = p->measurementRepeats >> 32;
		DWORD64 deltaTSC = 0;
 
		if (!(p->asmOrCpp))
		{   // Thread main work, for patterns from FASM DLL.
			(r->dll_PerformanceGate)
				(rwMethodSelect, (BYTE*)bufferAlignedSrc, (BYTE*)bufferAlignedDst,
					instructionsCount, repeatsCount, repeatsCountExt, &deltaTSC);
		}
		else
		{   // Thread main work, for CPP patterns: vectors, intrinsics and same patterns.
			(r->selected_pattern)
				(rwMethodSelect, (BYTE*)bufferAlignedSrc, (BYTE*)bufferAlignedDst,
					instructionsCount, repeatsCount, repeatsCountExt, &deltaTSC);
		}

		// Thread coordination
		SetEvent(rxHandle);
		WaitForSingleObject(txHandle, INFINITE);
		ResetEvent(txHandle);
	}

	return 0;
}

// Helpers methods.
// Blank thread control entry.
void ThreadsBuilder::helperBlankThread(THREAD_CONTROL_ENTRY* pT)
{

	pT->threadHandle = NULL;                 // Thread Handle for execution thread.
	pT->rxEventHandle = NULL;                // Event Handle for daughter thread operation complete signal.
	pT->txEventHandle = NULL;                // Event Handle for daughter thread operation continue enable signal.
	pT->base1 = NULL;                        // Source for read, destination for write and modify.
	pT->base2 = NULL;                        // Destination for copy.
	pT->sizeInstructions = 0;                // Block size, units = instructions, for benchmarking.
	pT->measurementRepeats = 0;              // Number of measurement repeats.
	pT->largePagesMode = 0;                  // Bit D0=Large Pages, other bits [1-31/63] = reserved.
	pT->methodId = 0;                        // Entry point to Target operation method subroutine ID.
	pT->asmOrCpp = 0;                        // Added for support C++ tests: vectors, intrinsics.
	pT->terminateThread = 0;                 // Flag for thread termination by return from callback routine.
	helperBlankAffinity(&pT->optimalGaff);   // Affinity mask and group id, OPTIMAL for this thread.
	helperBlankAffinity(&pT->originalGaff);  // Affinity mask and group id, ORIGINAL for this thread.
	pT->pRoutines = 0;                       // Pointers for dynamical import and this application DLL functions.
}
// Blank group affinity structure.
void ThreadsBuilder::helperBlankAffinity(GROUP_AFFINITY* pA)
{
	pA->Mask = 0;
	pA->Group = 0;
	pA->Reserved[0] = 0;  // If not blanked, API returns error.
	pA->Reserved[1] = 0;
	pA->Reserved[2] = 0;
}

// System threads data.
SYSTEM_THREADS_DATA ThreadsBuilder::threadsData;
