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

Class header for memory paging feature detector.
detects large pages available.
TODO.
1) Check used WinAPI functions for dynamical and statical import.
   Review this legacy code import methods.
2) Verify this class at system with large pages.

*/

#pragma once
#ifndef PAGINGDETECTOR_H
#define PAGINGDETECTOR_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "UDM.h"
#include "AppLib.h"

class PagingDetector : public UDM
{
public:
	PagingDetector(FUNCTIONS_LIST* pf);
	~PagingDetector();
	void writeReport();
	// Paging detector specific methods.
	SYSTEM_PAGING_DATA* getPagingList();    // Get pointer to detected paging options values structure.
private:
	static SYSTEM_PAGING_DATA pagingData;
};

#endif  // PAGINGDETECTOR_H
