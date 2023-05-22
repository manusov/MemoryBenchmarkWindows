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

Class header for memory and/or cache benchmark scenario.
TODO.
	
*/

#pragma once
#ifndef BENCHMARKMEMORY_H
#define BENCHMARKMEMORY_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "ControlSet.h"
#include "AppLib.h"
#include "USM.h"

class BenchmarkMemory : public USM
{
public:
	BenchmarkMemory(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp);
	~BenchmarkMemory();
	void execute();  // Execute memory or cache benchmark scenario.
};

#endif  // BENCHMARKMEMORY_H

