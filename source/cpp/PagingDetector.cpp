#include "PagingDetector.h"

// Pointer to global control set of functions 
FUNCTIONS_LIST* PagingDetector::f = NULL;
// Paging summary report data
SYSTEM_PAGING_DATA PagingDetector::pagingData;
// Status string
#define NS 81
char PagingDetector::s[NS];

// Class constructor, initialize pointers, get paging data
PagingDetector::PagingDetector( FUNCTIONS_LIST* functions )
{
	// Blank status string
    snprintf( s, NS, "no data" );
	// global initialization and pre-blank output
	f = functions;
	pagingData.defaultPage = 0;
	pagingData.largePage = 0;
	pagingData.pagingRights = 0;
	// Get default page size
	SYSTEM_INFO sysinfo;
	GetSystemInfo( &sysinfo );
	pagingData.defaultPage = sysinfo.dwPageSize;
	// Get large page size
	if ( f->API_GetLargePageMinimum != NULL )
	{
		pagingData.largePage = ( f->API_GetLargePageMinimum ) ( );
	}
	// Get large page privileges
	// Prepare get large pages rights, get rights for large page use
    HANDLE hToken;
    HANDLE hProcess;
    LPVOID tryLarge;
    TOKEN_PRIVILEGES tp;
    BOOL status = FALSE;
    const char* pszPrivilege = "SeLockMemoryPrivilege";
    BOOL bEnable = TRUE;
    // open process token
    if ( ( pagingData.largePage != 0 ) && 
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
                        tryLarge = VirtualAllocEx( hProcess, NULL, pagingData.largePage, 
                                                   MEM_COMMIT + MEM_LARGE_PAGES, PAGE_READWRITE );
                        if ( tryLarge != NULL )
                        {   // Release memory after try allocate
                            status = VirtualFreeEx( hProcess, tryLarge, 0, MEM_RELEASE );
                            if ( status != 0 )
                            {
                            pagingData.pagingRights = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}

// Class destructor, functionality reserved
PagingDetector::~PagingDetector( )
{

}

// Return pointer to paging data
SYSTEM_PAGING_DATA* PagingDetector::getPagingList( )
{
	return &pagingData;
}

// Get text report
void PagingDetector::getPagingText( LPSTR &dst, size_t &max )
{
	int count = 0;
	DWORDLONG x1 = pagingData.defaultPage / 1024;
	DWORDLONG x2 = pagingData.largePage / 1024;
	int x3 = pagingData.pagingRights;
	count = snprintf( dst, max, "default page = %I64d KB, large page = %I64d KB, privileges = %d\r\n", x1, x2, x3 );
	dst += count;
	max -= count;
}

// Method returns status string, valid if error returned
char* PagingDetector::getStatusString( )
{
    return s;
}

