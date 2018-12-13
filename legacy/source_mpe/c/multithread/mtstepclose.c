/*
 *   Operational sequence for call benchmark patterns in the multi-thread mode.
 *   Destroy threads management context, close multithread session.
 */

 void mtStepClose( MT_DATA* mtd, LIST_RELEASE_RESOURCES* xr )
 {
    int n = mtd->threadsCount;     // Number of threads (threads descriptors) to build
    int i = 0;                     // Cycle counter for threads descriptors list
    THREAD_CONTROL_ENTRY* ePointer = mtd->threadsControl;   // Pointer to entries list, one entry per thread
    HANDLE handle;
    BOOL status;
    
    for ( i=0; i<n; i++ )
    {
        // Terminate thread
        handle = ePointer->threadHandle;
        if ( handle != NULL )
        {
            status = TerminateThread( handle, 0 );
            if ( status == 0 )
            {
                helperRelease( xr );
                exitWithSystemError( "terminate thread" );
            }
        }
        // Close event handle
        handle = ePointer->eventHandle;
        if ( handle != NULL )
        {
            status = CloseHandle( handle );
            if ( status == 0 )
            {
                helperRelease( xr );
                exitWithSystemError( "close handle for threads management" );
            }
        }
        // Close thread handle
        handle = ePointer->threadHandle;
        if ( handle != NULL )
        {
            status = CloseHandle( handle );
            if ( status == 0 )
            {
                helperRelease( xr );
                exitWithSystemError( "close thread handle" );
            }
        }
        ePointer++;
    }
 }
 
