#include "SystemMemory.h"

// Status string
#define NS 81
char SystemMemory::s[NS];

// Data for memory status store
MEMORYSTATUSEX SystemMemory::memstatus;

// Class constructor
SystemMemory::SystemMemory( )
{
	snprintf( s, NS, "no data" );
}

// Class destructor
SystemMemory::~SystemMemory( )
{

}

// Memory detection root method
DWORD SystemMemory::detectMemory( SYSTEM_MEMORY_DATA* xm )
{
    // Pre-blank output
    BOOL status = FALSE;
    xm->physicalMemory = 0;
    xm->freeMemory = 0;
    // Get system memory information
    memstatus.dwLength = sizeof( memstatus );
	status = GlobalMemoryStatusEx ( &memstatus );
	if ( !status )
	{
		DWORD error = GetLastError( );
		snprintf( s, NS, "get system memory information" );
		return error;
	}
    xm->physicalMemory = memstatus.ullTotalPhys;
    xm->freeMemory = memstatus.ullAvailPhys;
    return 0;    
}

// Method returns status string, valid if error returned
char* SystemMemory::getStatusString( )
{
    return s;
}

