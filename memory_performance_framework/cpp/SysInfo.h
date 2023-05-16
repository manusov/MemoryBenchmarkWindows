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

Class header for system information scenario.
TODO.

*/

#pragma once
#ifndef SYSINFO_H
#define SYSINFO_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "ControlSet.h"
#include "AppLib.h"

class SysInfo
{
public:
	SysInfo(int op, SYSTEM_CONTROL_SET* cs);
	~SysInfo();
	void execute();  // Execute system information scenario.
private:
	static int enumOp;
	static SYSTEM_CONTROL_SET* controlSet;
};

#endif  // SYSINFO_H
