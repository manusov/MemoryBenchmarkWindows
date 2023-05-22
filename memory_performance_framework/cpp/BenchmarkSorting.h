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

Class header for sorting test scenario.
TODO.

*/

#pragma once
#ifndef BENCHMARKSORTING_H
#define BENCHMARKSORTING_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "ControlSet.h"
#include "AppLib.h"
#include "PerfLib.h"
#include "USM.h"

class BenchmarkSorting : public USM
{
public:
	BenchmarkSorting(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp);
	~BenchmarkSorting();
	void execute();        // Execute sorting test scenario.
private:
	static constexpr int COUNT = 200000;
};

#endif  // BENCHMARKSORTING_H

