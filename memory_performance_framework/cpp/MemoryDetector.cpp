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

Class realization for memory detector,
detects installed and available memory size.
TODO.

*/

#include "MemoryDetector.h"

// System memory summary report data.
SYSTEM_MEMORY_DATA MemoryDetector::memoryData;

MemoryDetector::MemoryDetector(FUNCTIONS_LIST* functions)
{
	// Blank status string.
	snprintf(statusString, APPCONST::MAX_TEXT_STRING, "No data.");
	// global initialization and pre-blank output
	f = functions;
	memoryData.physicalMemory = 0;
	memoryData.freeMemory = 0;
	// Get memory data.
	if (f->api_GlobalMemoryStatusEx != nullptr)
	{
		MEMORYSTATUSEX memstatus;
		// Get system memory information.
		memstatus.dwLength = sizeof(memstatus);
		status = GlobalMemoryStatusEx(&memstatus);
		if (!status)
		{
			DWORD error = GetLastError();
			snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Get system memory information failed.");
		}
		else
		{
			memoryData.physicalMemory = memstatus.ullTotalPhys;
			memoryData.freeMemory = memstatus.ullAvailPhys;
		}
	}
}
// Class destructor, functionality reserved.
MemoryDetector::~MemoryDetector()
{

}
void MemoryDetector::writeReport()
{
	char msg[APPCONST::MAX_TEXT_STRING];
	DWORD64 physicalMem = memoryData.physicalMemory;
	DWORD64 freeMem = memoryData.freeMemory;
	AppLib::writeColor("Installed memory = ", APPCONST::TABLE_COLOR);
	AppLib::storeCellMemorySize(msg, APPCONST::MAX_TEXT_STRING, physicalMem, 1);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	AppLib::writeColor(", free = ", APPCONST::TABLE_COLOR);
	AppLib::storeCellMemorySize(msg, APPCONST::MAX_TEXT_STRING, freeMem, 1);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	AppLib::writeColor(".\r\n", APPCONST::TABLE_COLOR);
}
// Return pointer to memory data.
SYSTEM_MEMORY_DATA* MemoryDetector::getMemoryList()
{
	return &memoryData;
}

