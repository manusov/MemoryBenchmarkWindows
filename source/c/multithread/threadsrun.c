/*
 *   Operational sequence for call benchmark patterns in the multi-thread mode.
 *   Run first (after open multithread session) benchmark operation.
 */

void threadsRun( MT_DATA* mtd, LIST_DLL_FUNCTIONS* xf, DWORD64* deltaTSC )
{
    DWORD result;
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
        handle = ePointer->threadHandle;   // Get thread handle
        result = ResumeThread( handle );   // Resume this thread
        ePointer++;
    }
    // Wait for signals from threads
    result = WaitForMultipleObjects( n, hPointer, TRUE, 60000 );
    // Get end TSC, calculate delta-TSC, return
    ( xf->DLL_ExecuteRdtsc )( &tsc2 );
    *deltaTSC = tsc2 - tsc1;
    
    // TODO: status analysing required, result variable, after BOTH API calls.
    
    return;
}

 
