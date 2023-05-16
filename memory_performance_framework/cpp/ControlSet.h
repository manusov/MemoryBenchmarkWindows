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

Class header for classes directory structure.
TODO.

*/

#pragma once
#ifndef CONTROLSET_H
#define CONTROLSET_H

#include "FncLoader.h"
#include "ProcessorDetector.h"
#include "TopologyDetector.h"
#include "MemoryDetector.h"
#include "PagingDetector.h"
#include "DomainsBuilder.h"
#include "ThreadsBuilder.h"

// System control set classes group, as pointers to classes.
typedef struct {
	FncLoader*         pFunctionsLoader;
	ProcessorDetector* pProcessorDetector;
	TopologyDetector*  pTopologyDetector;
	MemoryDetector*    pMemoryDetector;
	PagingDetector*    pPagingDetector;
	DomainsBuilder*    pDomainsBuilder;
	ThreadsBuilder*    pThreadsBuilder;
} SYSTEM_CONTROL_SET;

#endif  // CONTROLSET_H


