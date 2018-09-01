/*
 *    Get NUMA topology info by WinAPI.
 */

// Get NUMA nodes count (by WinAPI) function definition
typedef BOOL ( WINAPI *LPFN_GNHN )( PULONG );

CHAR* gnhnLibrary = "kernel32";
CHAR* gnhnFunction = "GetNumaHighestNodeNumber";

void stepNuma( MPE_NUMA_DATA* xn, LIST_RELEASE_RESOURCES* xr )
{
    // Get system affinity mask
    DWORD_PTR processAffinity = 0;
    DWORD_PTR systemAffinity = 0;
    BOOL status = FALSE;
    HANDLE processHandle = GetCurrentProcess();
    if ( processHandle != NULL )
    {
        status = GetProcessAffinityMask( processHandle, &processAffinity, &systemAffinity );
        if ( status )
        {
            xn->systemAffinity = systemAffinity;
        }
        else
        {
            exitWithSystemError( "get process affinity mask" );
        }
    }
    else
    {
        exitWithSystemError( "get process handle" );
    }
    if ( xn->systemAffinity == 0 )
    {
        exitWithInternalError( "interpreting affinity mask" );
    }

    // Variable for return, default NUMA domains count = 1
    int numaDomainsCount = 1;
    // Local variables, for get platform NUMA topology by WinAPI
    HANDLE hgnhn = NULL;
    LPFN_GNHN gnhn = NULL;
    BOOL numaValid;
    ULONG lastNumaNode;
    // Get platform NUMA topology by WinAPI
    hgnhn = GetModuleHandle( gnhnLibrary );
    if ( hgnhn != NULL )
    {
        gnhn = ( LPFN_GNHN ) GetProcAddress( hgnhn, gnhnFunction );
        if ( gnhn != NULL )
        {
            numaValid = gnhn ( &lastNumaNode );
            if ( numaValid )
            {
                numaDomainsCount = lastNumaNode + 1;
            }
        }
    }
    // Update output
    xn->domainsCount = numaDomainsCount;
    xn->domainsList = NULL;  // yet reserved
    // Show results
    printf( "\nNUMA domains count=%d", xn->domainsCount );
}

