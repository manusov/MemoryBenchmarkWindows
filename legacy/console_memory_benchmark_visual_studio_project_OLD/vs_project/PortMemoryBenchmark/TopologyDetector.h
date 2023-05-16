/*
				 MEMORY PERFORMANCE ENGINE FRAMEWORK.
	Header for Windows SMP/Cache topology detection API interface class.
	 This method can be functional even if cache not detected by CPUID.
*/

#pragma once
#ifndef TOPOLOGYDETECTOR_H
#define TOPOLOGYDETECTOR_H

#include <windows.h>
#include <cstdio>
#include "MainDefinitions.h"
#include "AppLib.h"

class TopologyDetector
{
public:
	TopologyDetector(FUNCTIONS_LIST* functions);
	~TopologyDetector();
	SYSTEM_TOPOLOGY_DATA* getTopologyList();
	SYSTEM_TOPOLOGY_DATA* getTopologyListEx();
	void getTopologyText(LPSTR& dst, size_t& max);
	void getTopologyTextEx(LPSTR& dst, size_t& max);
	void getCacheText(LPSTR& dst, size_t& max);
	char* getStatusString();
private:
	static FUNCTIONS_LIST* f;
	static PSYSTEM_LOGICAL_PROCESSOR_INFORMATION    pTopologyBuffer;
	static PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pTopologyBufferEx;
	static PSYSTEM_LOGICAL_PROCESSOR_INFORMATION    pTopologyLimit;
	static PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pTopologyLimitEx;
	static DWORD topologySize;
	static DWORD topologySizeEx;
	static SYSTEM_TOPOLOGY_DATA topologyData;
	static SYSTEM_TOPOLOGY_DATA topologyDataEx;
	static const char* tableUpCores;
	static const char* tableUpCaches;
	static const char* tableUpDomains;
	static const char* tableUpPackages;
	static const char* tableUpCoresEx;
	static const char* tableUpCachesEx;
	static const char* tableUpDomainsEx;
	// static const char* tableUpPackagesEx;
	static const char* tableUpGroupsEx;
	static const char* cacheTypes[];
	static char s[];
	// Helpers functions
	void blankTopologyData(SYSTEM_TOPOLOGY_DATA* p);
	void printGroups(LPSTR& dst, size_t& max, int skipLeft, PGROUP_AFFINITY pGaff, int groupCount);
};

#endif  // TOPOLOGYDETECTOR_H


