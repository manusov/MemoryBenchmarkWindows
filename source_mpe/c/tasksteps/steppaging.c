/*
 *    Get Memory Paging info by WinAPI,
 *    include large pages support and privileges.
 */

// Template and data for dynamical import
typedef SIZE_T ( WINAPI *LPFN_GLPM )( void );
CHAR* glpmLibrary = "kernel32";
CHAR* glpmFunction = "GetLargePageMinimum";

// System information structure
SYSTEM_INFO sysinfo;

void stepPaging( MPE_PAGING_DATA* xp, LIST_RELEASE_RESOURCES* xr )
{
    // Pre-blank output
    xp->defaultPage = 0;
    xp->largePage = 0;
    xp->pagingRights = 0;
    
    // Get standard page size
    GetSystemInfo( &sysinfo );
    xp->defaultPage = sysinfo.dwPageSize;
    
    // Get large page size
    HANDLE hglpm = NULL;
    LPFN_GLPM glpm = NULL;
    hglpm = GetModuleHandle( glpmLibrary );
    if ( hglpm != NULL )
    {
        glpm = ( LPFN_GLPM ) GetProcAddress( hglpm, glpmFunction );
        if ( glpm != NULL )
        {
            xp->largePage = GetLargePageMinimum();
            
            // Get rights for large page use
            HANDLE hToken;
            HANDLE hProcess;
            LPVOID tryLarge;
            TOKEN_PRIVILEGES tp;
            BOOL status = 0;
            CHAR* pszPrivilege = "SeLockMemoryPrivilege";
            BOOL bEnable = TRUE;
            // open process token
            if ( OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) )
            {
                // get the luid
                if ( LookupPrivilegeValue( NULL, pszPrivilege, &tp.Privileges[0].Luid ) )
                {
                    // enable or disable privilege
                    tp.PrivilegeCount = 1;
                    if ( bEnable )
                    tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
                    else
                    tp.Privileges[0].Attributes = 0;
                    // enable or disable privilege
                    status = AdjustTokenPrivileges( hToken, FALSE, &tp, 0, ( PTOKEN_PRIVILEGES )NULL, 0 );
                    if ( status )
                    {
                        status = CloseHandle( hToken );
                        if ( status )
                        {
                            hProcess = GetCurrentProcess();
                            if ( hProcess != NULL )
                            {
                                tryLarge = VirtualAllocEx( hProcess, NULL, xp->largePage, 
                                                           MEM_COMMIT + MEM_LARGE_PAGES, PAGE_READWRITE );
                                if ( tryLarge != NULL )
                                {
                                    status = VirtualFreeEx( hProcess, tryLarge, 0, MEM_RELEASE );
                                    if ( status != 0 )
                                    {
                                    xp->pagingRights = 1;
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
    }
    
    // Show results
    int x1 = xp->defaultPage / 1024;
    int x2 = xp->largePage / 1024;
    CHAR* s1 = "available";
    CHAR* s2 = "no rights";
    CHAR* s = s1;
    if ( xp->pagingRights == 0 )
    {
        s = s2;
    }
    printf( "\nPaging options: default page=%dK, large page=%dK (%s)", x1, x2, s );
}


