/*
 *    Get SMP and Cache info by WinAPI.
 */

// SMP topology and caches list (by WinAPI) function definition
typedef BOOL ( WINAPI *LPFN_GLPI )(
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, 
    PDWORD );

CHAR* glpiLibrary = "kernel32";
CHAR* glpiFunction = "GetLogicalProcessorInformation";

void stepMpCache( MPE_CACHE_DATA* xc, LIST_RELEASE_RESOURCES* xr )
{
	// Local variables for WinAPI call and buffer fill
    HANDLE hglpi = NULL;
    LPFN_GLPI glpi = NULL;
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION bufferPtr = NULL;
    DWORD bufferSize = 0;
    // Error reporting support
    int N = 100;
    CHAR p[N+1];
    // Load WinAPI function
    hglpi = GetModuleHandle( glpiLibrary );
    if ( hglpi == NULL )
    {
        helperRelease( xr );
        snprintf( p, N, "load module=%s", glpiLibrary );
        exitWithSystemError( p );	
    }
    glpi = ( LPFN_GLPI ) GetProcAddress( hglpi, glpiFunction );
    if ( glpi == NULL )
    {
        helperRelease( xr );
        snprintf( p, N, "load function=%s", glpiFunction );
        exitWithSystemError( p );	
    }
    // Get information into buffer, note required memory allocation for buffer
    while (!done)
    {
        DWORD rc = glpi( bufferPtr, &bufferSize );
        if ( rc == FALSE ) 
        {
            if ( GetLastError() == ERROR_INSUFFICIENT_BUFFER ) 
            {
                if ( bufferPtr ) 
                    free( bufferPtr );

                bufferPtr = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc( bufferSize );

                if ( bufferPtr == NULL ) 
                {
                    helperRelease( xr );
                    snprintf( p, N, "allocate buffer for function=%s", glpiFunction );
                    exitWithSystemError( p );
                }
            } 
            else 
            {
                helperRelease( xr );
                snprintf( p, N, "call function=%s", glpiFunction );
                exitWithSystemError( p );
            }
        } 
        else
        {
            done = TRUE;
        }
    }
    // Pre-clear output data
    xc->pointL1 = 0;
    xc->pointL2 = 0;
    xc->pointL3 = 0;
    xc->hyperThreadingFlag = 0;
    // Local variables fpr analysing buffer content
    PCACHE_DESCRIPTOR Cache;
    DWORD relationship;
    int bufferCount = bufferSize / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION );
    int i;
    // Analysing buffer content, update output
    for( i=0; i<bufferCount; i++ )
    {
    relationship = bufferPtr->Relationship;
    switch ( relationship )
        {
            case RelationCache:
            Cache = &bufferPtr->Cache;
            switch ( Cache->Level )
            {
                case 1:
                    if ( ( Cache->Type == CacheUnified )||( Cache->Type == CacheData ) )
                    {
                        xc->pointL1 = Cache->Size;
                    }
                    break;
                case 2:
                    if ( ( Cache->Type == CacheUnified )||( Cache->Type == CacheData ) )
                    {
                        xc->pointL2 = Cache->Size;
                    }
                    break;
                case 3:
                    if ( ( Cache->Type == CacheUnified )||( Cache->Type == CacheData ) )
                    {
                        xc->pointL3 = Cache->Size;
                    }
                    break;
                default:
                    break;
            }
            break;
            case RelationProcessorCore:
                if ( ( bufferPtr->ProcessorCore.Flags ) != 0 )
                {
                    xc->hyperThreadingFlag = 1;
                }
                break;
            case RelationNumaNode:
                break;
            case RelationProcessorPackage:
                break;
            default:
                break;
        }
    bufferPtr++;
    }
    // Show results
    int x1 = ( xc->pointL1 ) / 1024;
    int x2 = ( xc->pointL2 ) / 1024;
    int x3 = ( xc->pointL3 ) / 1024;
    int x4 = xc->hyperThreadingFlag;
    printf( "\nMeasured cache levels: L1=%dK, L2=%dK, L3=%dK. HyperThreading=%d", x1, x2, x3, x4 );
}


