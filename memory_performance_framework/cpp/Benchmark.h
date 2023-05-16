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

Class header for memory and cache benchmark scenario.
TODO.
	
*/

#pragma once
#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "ControlSet.h"
#include "AppLib.h"

class Benchmark
{
public:
	Benchmark(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp);
	~Benchmark();
	void execute();  // Execute memory or cache benchmark scenario.
private:
	static int enumOp;
	static SYSTEM_CONTROL_SET* controlSet;
	static COMMAND_LINE_PARMS* pCmd;
};

#endif  // BENCHMARK_H

