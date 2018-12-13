/*
 *   Helper for release allocated resources.
 */

void helperRelease( LIST_RELEASE_RESOURCES* xr )
{
    BOOL status;
    // Release benchmark read/write memory buffer
    if ( xr->bufferTarget != NULL )
    {
        status = VirtualFree( xr->bufferTarget, 0, MEM_RELEASE );
        if ( status == 0 )
        {
            exitWithSystemError( "memory release" );
        }
    }
    // Release benchmark statistics buffer
    if ( xr->bufferStatistics != NULL )
    {
        free( xr->bufferStatistics );
    }
    // Unload native library
    if ( xr->nativeLibrary != NULL )
    {
        status = FreeLibrary( xr->nativeLibrary );
        if ( status == FALSE )
        {
            int N = 100;
            CHAR p[N+1];
            snprintf( p, N, "unload module=%s", xr->dllName );
            exitWithSystemError( p );	
        }
    }
    // Release threads list
    if ( xr->threadsControl != NULL )
    {
        free( xr->threadsControl );
    }
    // Release threads events list
    if ( xr->signalsHandles != NULL )
    {
        free( xr->signalsHandles );
    }
}

