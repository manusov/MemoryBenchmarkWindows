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

Class header for User help:
write user help information to console output.
TODO.

*/

#pragma once
#ifndef USERHELP_H
#define USERHELP_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "AppLib.h"
#include "USM.h"

class UserHelp : public USM
{
public:
	UserHelp(int op, const OPTION_ENTRY* oplist);  // Initialize user help scenario.
	~UserHelp();
	void execute();  // Execute user help scenario.
private:
	const OPTION_ENTRY* optionsList;
	static const char* exampleString;
};

#endif  // USERHELP_H

