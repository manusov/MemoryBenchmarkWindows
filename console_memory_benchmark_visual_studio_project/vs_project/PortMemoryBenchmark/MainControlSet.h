/*
		 MEMORY PERFORMANCE ENGINE FRAMEWORK.
	Classes directory for functinality interconnect.
*/

#pragma once
#ifndef MAINCONTROLSET_H
#define MAINCONTROLSET_H

#include "FunctionsLoader.h"
#include "ProcessorDetector.h"
#include "TopologyDetector.h"
#include "MemoryDetector.h"
#include "PagingDetector.h"
#include "DomainsBuilder.h"
#include "ThreadsBuilder.h"

// System control set classes group
typedef struct {
	FunctionsLoader* pFunctionsLoader;
	ProcessorDetector* pProcessorDetector;
	TopologyDetector* pTopologyDetector;
	MemoryDetector* pMemoryDetector;
	PagingDetector* pPagingDetector;
	DomainsBuilder* pDomainsBuilder;
	ThreadsBuilder* pThreadsBuilder;
} SYSTEM_CONTROL_SET;

#endif  // MAINCONTROLSET_H

