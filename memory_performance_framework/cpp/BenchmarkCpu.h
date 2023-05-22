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

Class header for CPU benchmark.
TODO.

*/

#pragma once
#ifndef BENCHMARKCPU_H
#define BENCHMARKCPU_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "ControlSet.h"
#include "AppLib.h"
#include "PerfLib.h"
#include "USM.h"

class BenchmarkCpu : public USM
{
public:
	BenchmarkCpu(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp);
	~BenchmarkCpu();
	void execute();  // Execute CPU benchmark scenario.
};

#endif  // BENCHMARKCPU_H

