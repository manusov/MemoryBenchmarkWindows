#include "MemoryDetector.h"

// Pointer to global control set of functions 
FUNCTIONS_LIST* MemoryDetector::f = NULL;
// System memory summary report data
SYSTEM_MEMORY_DATA MemoryDetector::memoryData;
// Status string
#define NS 81
char MemoryDetector::s[NS];

// Class constructor, initialize pointers, get memory data
MemoryDetector::MemoryDetector( FUNCTIONS_LIST* functions )
{
	// Blank status string
    snprintf( s, NS, "no data" );
	// global initialization and pre-blank output
	f = functions;
	memoryData.physicalMemory = 0;
	memoryData.freeMemory = 0;
	// get memory data
	if ( f->API_GlobalMemoryStatusEx != NULL )
	{
		MEMORYSTATUSEX memstatus;
		// Get system memory information
    	memstatus.dwLength = sizeof( memstatus );
		BOOL status = GlobalMemoryStatusEx ( &memstatus );
		if ( !status )
		{
			DWORD error = GetLastError( );
			snprintf( s, NS, "get system memory information" );
		}
		else
		{
    		memoryData.physicalMemory = memstatus.ullTotalPhys;
			memoryData.freeMemory = memstatus.ullAvailPhys;
		}
	}
}

// Class destructor, functionality reserved
MemoryDetector::~MemoryDetector( )
{

}

// Return pointer to memory data
SYSTEM_MEMORY_DATA* MemoryDetector::getMemoryList( )
{
	return &memoryData;
}

// Get text report
void MemoryDetector::getMemoryText( LPSTR &dst, size_t &max )
{
	int count = 0;
	
	/*
	DWORDLONG x1 = memoryData.physicalMemory / 1024;
	DWORDLONG x2 = memoryData.freeMemory / 1024;
	count = snprintf( dst, max, "installed memory = %I64d KB, free = %I64d KB\r\n", x1, x2 );
	dst += count;
	max -= count;
	*/
	
	DWORD64 x1 = memoryData.physicalMemory;
	DWORD64 x2 = memoryData.freeMemory;
	AppLib::printString( dst, max, "installed memory = " );
	AppLib::printCellMemorySize( dst, max, x1, 1 );
	AppLib::printString( dst, max, ", free = " );
	AppLib::printCellMemorySize( dst, max, x2, 1 );
	AppLib::printCrLf( dst, max );
}

// Method returns status string, valid if error returned
char* MemoryDetector::getStatusString( )
{
    return s;
}

