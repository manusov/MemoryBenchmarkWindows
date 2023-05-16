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

Class header for topology detector,
detects system topology include caches levels.
TODO.

*/

#pragma once
#ifndef TOPOLOGYDETECTOR_H
#define TOPOLOGYDETECTOR_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "UDM.h"
#include "AppLib.h"

class TopologyDetector : public UDM
{
public:
	TopologyDetector(FUNCTIONS_LIST* pf);
	~TopologyDetector();
	void writeReport();
	// Topology detector specific methods.
	SYSTEM_TOPOLOGY_DATA* getTopologyList();      // Get pointer to topology list structure.
	SYSTEM_TOPOLOGY_DATA* getTopologyListEx();    // Get pointer to extended topology list structure.
	void writeStandardTopology();                 // Print text about topology to screen and text buffer.
	void writeExtendedTopology();                 // Print text about extended topology to screen and text buffer.
	void writeCacheInfo();                        // Print text about cache to screen and text buffer.
private:
	static PSYSTEM_LOGICAL_PROCESSOR_INFORMATION    pTopologyBuffer;
	static PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pTopologyBufferEx;
	static PSYSTEM_LOGICAL_PROCESSOR_INFORMATION    pTopologyLimit;
	static PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pTopologyLimitEx;
	static DWORD topologySize;
	static DWORD topologySizeEx;
	static SYSTEM_TOPOLOGY_DATA topologyData;
	static SYSTEM_TOPOLOGY_DATA topologyDataEx;
	// Text constants.
	static const char* cacheTypes[];
	// Helpers functions.
	void blankTopologyData(SYSTEM_TOPOLOGY_DATA* p);
	void printGroups(int skipLeft, PGROUP_AFFINITY pGaff, int groupCount);
};

#endif  // TOPOLOGYDETECTOR_H

