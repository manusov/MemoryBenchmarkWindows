/*
 *   Operational sequence for call benchmark patterns in the multi-thread mode.
 *   Allocate memory resources for multithread context.
 */

void mtStepListAlloc( MT_DATA* mtd, MPE_INPUT_PARAMETERS_BLOCK* ipb,
                      LIST_RELEASE_RESOURCES* xr )
{
    // get number of threads
    int n = ipb->selectThreadsCount;
    // allocate memory for threads list
    mtd->threadsControl = ( THREAD_CONTROL_ENTRY* )malloc( n*sizeof(THREAD_CONTROL_ENTRY) );
    xr->threadsControl = mtd->threadsControl;
    // check memory allocation error
    if ( xr->threadsControl == NULL )
    {
        helperRelease ( xr );
        exitWithSystemError( "allocate memory for threads list" );
    }
    // allocate memory for events list
    mtd->signalsHandles = ( HANDLE* )malloc( n*sizeof(HANDLE) );
    xr->signalsHandles = mtd->signalsHandles;
    // check memory allocation error
    if ( xr->signalsHandles == NULL )
    {
        helperRelease ( xr );
        exitWithSystemError( "allocate memory for events handles" );
    }
    // store number of threads
    mtd->threadsCount = n;
}

