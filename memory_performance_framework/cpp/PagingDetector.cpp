/*

MEMORY PERFORMANCE ENGINE (MPE) FRAMEWORK.
-------------------------------------------
System informationand performance analysing application,
include console version of NCRB(NUMA CPUand RAM Benchmark).
See also memory benchmark projects :
https://github.com/manusov/NCRBv2
https://github.com/manusov/MemoryBenchmarkWindows
Experimental samples :
https://github.com/manusov/Prototyping
Special thanks for C++ lessons :
https://ravesli.com/uroki-cpp/

Class realization for memory paging feature detector.
detects large pages available.
TODO.
1) Check used WinAPI functions for dynamical and statical import:
   some used token functions and other.
   Review this legacy code import methods.
2) Why "LookupPrivilegeValue" dynamical import failed?
   Required "LookupPrivilegeValueA" ?
3) Verify this class at system with large pages.

*/

#include "PagingDetector.h"

// Paging summary report data
SYSTEM_PAGING_DATA PagingDetector::pagingData;

PagingDetector::PagingDetector(FUNCTIONS_LIST* pf) : UDM(pf)
{
    // Blank status string.
    snprintf(statusString, APPCONST::MAX_TEXT_STRING, "No data.");
    status = TRUE;
    // Global initialization and pre-blank output.
    pagingData.defaultPage = 0;
    pagingData.largePage = 0;
    pagingData.pagingRights = 0;
    // Get default page size
    SYSTEM_INFO sysinfo;
    GetSystemInfo(&sysinfo);
    pagingData.defaultPage = sysinfo.dwPageSize;
    // Get large page size
    if (f->api_GetLargePageMinimum)
    {
        pagingData.largePage = (f->api_GetLargePageMinimum) ();
    }
    // Get large page privileges.
    // Prepare get large pages rights, get rights for large page use.
    HANDLE hToken;
    HANDLE hProcess;
    LPVOID tryLarge;
    TOKEN_PRIVILEGES tp;
    BOOL localStatus = FALSE;
    const char* pszPrivilege = "SeLockMemoryPrivilege";
    BOOL bEnable = TRUE;
    // open process token
    if ((pagingData.largePage != 0) &&
        (OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken)))
    {
        // get the luid
        if (LookupPrivilegeValue(nullptr, pszPrivilege, &tp.Privileges[0].Luid))
        {
            // Enable or disable privilege.
            tp.PrivilegeCount = 1;
            if (bEnable)
                tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
            else
                tp.Privileges[0].Attributes = 0;
            // Enable or disable privilege.
            localStatus = AdjustTokenPrivileges(hToken, FALSE, &tp, 0, (PTOKEN_PRIVILEGES)nullptr, 0);
            if (localStatus)
            {
                localStatus = CloseHandle(hToken);
                if (localStatus)
                {
                    hProcess = GetCurrentProcess();
                    if (hProcess)
                    {   // Try allocate memory with large page option
                        tryLarge = VirtualAllocEx(hProcess, nullptr, pagingData.largePage,
                            MEM_COMMIT + MEM_LARGE_PAGES, PAGE_READWRITE);
                        if (tryLarge)
                        {   // Release memory after try allocate
                            localStatus = VirtualFreeEx(hProcess, tryLarge, 0, MEM_RELEASE);
                            if (localStatus)
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
PagingDetector::~PagingDetector()
{

}
void PagingDetector::writeReport()
{
    char msg[APPCONST::MAX_TEXT_STRING];
    int defaultPage = (int)(pagingData.defaultPage / 1024);
    int largePage = (int)(pagingData.largePage / 1024);
    int pagingRights = pagingData.pagingRights;
    snprintf(msg, APPCONST::MAX_TEXT_STRING,
        "Default page = %d KB, large page = %d KB, privileges = %d.\r\n",
        defaultPage, largePage, pagingRights);
    AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
}
// Return pointer to paging data.
SYSTEM_PAGING_DATA* PagingDetector::getPagingList()
{
    return &pagingData;
}
