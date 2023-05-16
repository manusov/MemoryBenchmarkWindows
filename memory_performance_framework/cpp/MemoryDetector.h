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

Class header for memory detector,
detects installed and available memory size.
TODO.

*/

#pragma once
#ifndef MEMORYDETECTOR_H
#define MEMORYDETECTOR_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "UDM.h"
#include "AppLib.h"

class MemoryDetector : public UDM
{
public:
	MemoryDetector(FUNCTIONS_LIST* pf);
	~MemoryDetector();
	void writeReport();
	// Memory detector specific methods.
	SYSTEM_MEMORY_DATA* getMemoryList();    // Get pointer to structure with installed and current free memory size.
private:
	static SYSTEM_MEMORY_DATA memoryData;
};

#endif  // MEMORYDETECTOR_H
