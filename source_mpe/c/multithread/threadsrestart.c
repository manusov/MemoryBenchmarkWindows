/*
 *   Operational sequence for call benchmark patterns in the multi-thread mode.
 *   Continue. run non-first (after open multithread session) benchmark operation.
 */

void threadsRestart( MT_DATA* mtd, LIST_DLL_FUNCTIONS* xf, DWORD64* deltaTSC )
{
    BOOL status;
    DWORD64 result;
    THREAD_CONTROL_ENTRY* ePointer = mtd->threadsControl;   // Pointer to entries list, one entry per thread
    HANDLE* hPointer = mtd->signalsHandles;
    HANDLE handle;
    int n = mtd->threadsCount;     // Number of threads (threads descriptors) to build
    int i = 0;                     // Cycle counter for threads descriptors list
    DWORD64 tsc1 = 0;
    DWORD64 tsc2 = 0;
    // Get start TSC
    ( xf->DLL_ExecuteRdtsc )( &tsc1 );
    // Run threads as resume thread operation
    for( i=0; i<n; i++ )
    {
        handle = ePointer->eventHandle;   // Get thread handle
        status = ResetEvent( handle );     // Resume this thread
        ePointer++;
    }
    // Wait for signals from threads
    result = WaitForMultipleObjects( n, hPointer, TRUE, 60000 );
    // Get end TSC, calculate delta-TSC, return
    ( xf->DLL_ExecuteRdtsc )( &tsc2 );
    *deltaTSC = tsc2 - tsc1;
    
    // TODO: status analysing required, status and result variables, BOTH API calls.
    
    return;
}

 
