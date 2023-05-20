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

Class header for Unified Scenario Model:
parent class for benchmarks and diagnostics scenarios in this application.
TODO.

*/

#pragma once
#ifndef USM_H
#define USM_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "ControlSet.h"
#include "AppLib.h"

class USM
{
public:
	USM(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp);
	~USM();
	void execute();  // Execute selected test or information scenario.
protected:
	int enumOp;
	SYSTEM_CONTROL_SET* controlSet;
	COMMAND_LINE_PARMS* pCmd;
};

#endif  // USM_H
