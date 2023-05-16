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

Class header for NUMA domains support:
NUMA-aware memory allocation.
TODO.

*/

#pragma once
#ifndef DOMAINSBUILDER_H
#define DOMAINSBUILDER_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "UDM.h"
#include "AppLib.h"

class DomainsBuilder : public UDM
{
public:
	DomainsBuilder(FUNCTIONS_LIST* pf);
	~DomainsBuilder();
	void writeReport();
	// Domains builder specific methods.
	SYSTEM_NUMA_DATA* getNumaList();                 // Get NUMA nodes list, previously builded by this class constructor.
	SYSTEM_NUMA_DATA* getSimpleList();               // Get simple list (for platform without NUMA) previously builded by this class constructor.
	void writeReportNuma(BOOL line = false);         // Write text report about NUMA nodes.
	void writeReportSimple(BOOL line = false);       // Write text report about simple topology.
	// Memory allocation methods, NUMA-aware.
	BOOL allocateNodesList(size_t memSize, DWORD pgMode, DWORD64 pgSize, BOOL swapFlag);  // Allocate memory for NUMA domains list.
	BOOL freeNodesList();                                                                 // Release memory, allocated for NUMA domains list.
	// Memory allocation methods, NUMA-unaware, but same format for regularity
	BOOL allocateSimpleList(size_t memSize, DWORD pgMode, DWORD64 pgSize);                // Allocate memory for simple topology.
	BOOL freeSimpleList();                                                                // Release memory, allocated for simple topology.
private:
	static SYSTEM_NUMA_DATA numaData;
	static SYSTEM_NUMA_DATA simpleData;
	static NUMA_NODE_ENTRY simpleEntry;
	// Helpers methods.
	void helperBlankNode(NUMA_NODE_ENTRY* p);
	DWORD64 helperAlignByFactor(DWORD64 value, DWORD64 factor);
};

#endif  // DOMAINSBUILDER_H


