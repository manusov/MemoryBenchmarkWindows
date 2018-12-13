#include "PagingOptions.h"

// Status string
#define NS 81
char PagingOptions::s[NS];

// System information structure
SYSTEM_INFO PagingOptions::sysinfo;

// Data for dynamical import, note cannot statical import optional exist function
// By MSDN, this function not supported at Windows XP
// glpm means GetLargePageMinimum() WinAPI
typedef SIZE_T ( WINAPI *LPFN_GLPM )( void );
const char* PagingOptions::glpmLibrary = "kernel32";
const char* PagingOptions::glpmFunction = "GetLargePageMinimum";

// Class constructor, blank status string
PagingOptions::PagingOptions( )
{
    snprintf( s, NS, "no data" );
}

// Class destructor, yet no operations
PagingOptions::~PagingOptions( )
{
    
}

// Paging detection root method
BOOL PagingOptions::detectPaging( PAGING_OPTIONS_DATA* xp )
{
    // Pre-blank output structure values
    xp->defaultPage = 0;
    xp->largePage = 0;
    xp->pagingRights = 0;
    // Get standard page size
    GetSystemInfo( &sysinfo );
    xp->defaultPage = sysinfo.dwPageSize;
    // Get large page size, function dynamically imported
    HMODULE hglpm = NULL;
    LPFN_GLPM glpm = NULL;
    hglpm = GetModuleHandle( glpmLibrary );
    if ( hglpm != NULL )
    {
        glpm = ( LPFN_GLPM )GetProcAddress( hglpm, glpmFunction );
        if ( glpm != NULL )
        {                                 // glpm means GetLargePageMinimum() WinAPI
            xp->largePage = ( glpm )( );  // Call dynamically imported
        }
    }
    // Prepare get large pages rights, get rights for large page use
    HANDLE hToken;
    HANDLE hProcess;
    LPVOID tryLarge;
    TOKEN_PRIVILEGES tp;
    BOOL status = FALSE;
    const char* pszPrivilege = "SeLockMemoryPrivilege";
    BOOL bEnable = TRUE;
    // open process token
    if ( ( xp->largePage != 0 ) && 
       ( OpenProcessToken( GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken ) ) )
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
                    {   // Try allocate memory with large page option
                        tryLarge = VirtualAllocEx( hProcess, NULL, xp->largePage, 
                                                   MEM_COMMIT + MEM_LARGE_PAGES, PAGE_READWRITE );
                        if ( tryLarge != NULL )
                        {   // Release memory after try allocate
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
    return TRUE;
}

// Method returns status string, valid if error returned
char* PagingOptions::getStatusString( )
{
    return s;
}


