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

Class header for timers test scenario.
TODO.

*/

#pragma once
#ifndef TIMERS_H
#define TIMERS_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "ControlSet.h"
#include "AppLib.h"

class Timers
{
public:
	Timers(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp);
	~Timers();
	void execute();  // Execute timers test scenario.
private:
	static int enumOp;
	static SYSTEM_CONTROL_SET* controlSet;
	static COMMAND_LINE_PARMS* pCmd;
};

#endif  // TIMERS_H
