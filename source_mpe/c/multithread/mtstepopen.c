/*
 *   Operational sequence for call benchmark patterns in the multi-thread mode.
 *   Create threads management context, open multithread session.
 */

void mtStepOpen( MT_DATA* mtd, LIST_DLL_FUNCTIONS* xf,
                 MPE_PLATFORM_INPUT* xp, MPE_INPUT_PARAMETERS_BLOCK* ipb,
                 LIST_RELEASE_RESOURCES* xr )
{
    int n = mtd->threadsCount;     // Number of threads (threads descriptors) to build
    int i = 0;                     // Cycle counter for threads descriptors list
    // Parameters for per-thread entries list build cycle
    THREAD_CONTROL_ENTRY* ePointer = mtd->threadsControl;   // Pointer to entries list, one entry per thread
    SIZE_T halfPerThread = ipb->selectBlockStart;
    if ( halfPerThread < ipb->selectBlockStop )
    {
        halfPerThread = ipb->selectBlockStop;
    }
    SIZE_T fullPerThread = halfPerThread * 2;
    LPVOID base1 = 0;
    LPVOID base2 = (LPVOID)halfPerThread;
    // Parameters for per thread events handles-only list build cycle
    HANDLE* hPointer = mtd->signalsHandles;
    
    // Cycle for build threads descriptors list
    for ( i=0; i<n; i++ )
    {
        // Build per-thread entries list
        ePointer->eventHandle = CreateEvent( NULL, TRUE, FALSE, NULL );   // Event Handle for this thread operation complete signal
        if ( ePointer->eventHandle == NULL )
        {
            helperRelease( xr );
            exitWithSystemError( "create event for threads management" );
        }
        ePointer->threadHandle = CreateThread( NULL, 0, threadEntry, ePointer, CREATE_SUSPENDED, 0 );   // This thread handle
        if ( ePointer->threadHandle == NULL )
        {
            helperRelease( xr );
            exitWithSystemError( "create thread" );
        }
        ePointer->threadAffinity = xp->platformNuma.systemAffinity;     // Thread affinity mask, NUMA yet not supported
        ePointer->DLL_PerformanceGate = xf->DLL_PerformanceGate;        // Low-level function entry point
        ePointer->methodId = ipb->selectRwMethod;                       // Read-write method ID, for assembler routine select in the library
        ePointer->base1 = base1;                                        // Thread base address 1 (source/ one destination), updated later when memory allocation
        ePointer->base2 = base2;                                        // Thread base address 2 (destination if copy), updated later when memory allocation
        ePointer->sizeBytes = 0;                                        // Block size (bytes), pre-blanked, must set later, at speed=f(size) cycle
        ePointer->sizeInstructions = 0;                                 // Block size (instructions), pre-blanked, must set later, at speed=f(size) cycle
        ePointer->largePages = 0;                                       // Large pages support yet disabled, under construction
        ePointer->measurementRepeats = 0;                               // Number of measurement repeats, pre-blanked, must set later, at calibration
        ePointer->trueAffinity = xp->platformNuma.systemAffinity;       // Thread affinity mask, NUMA yet not supported
        ePointer->trueBase = NULL;                                      // Pre-blank allocated memory block base, updated later when memory allocation
        ePointer->returnDeltaTSC = 0;    // Blank reserved field
        ePointer->returnCount = 0;       // Blank reserved field
        ePointer->returnLimit = 0;       // Blank reserved field
        base1 += fullPerThread;
        base2 += fullPerThread;
        // Build per-thread events handles-only list
        *hPointer = ePointer->eventHandle;
        // Advance pointers
        ePointer++;    // next entry
        hPointer++;    // next handle
    }
}

