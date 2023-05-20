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

Class header for Unified Driver Model:
parent class for components drivers in this application.
TODO.

*/

#pragma once
#ifndef UDM_H
#define UDM_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "AppLib.h"

class UDM
{
public:
	UDM();
	UDM(FUNCTIONS_LIST* pf);
	~UDM();
	BOOL getStatus();
	const char* const getStatusString();
	void writeReport();
	FUNCTIONS_LIST* getFunctionsList();
protected:
	BOOL status;
	FUNCTIONS_LIST* f;
	static char statusString[APPCONST::MAX_TEXT_STRING];
private:
	static const char* const msgError;
};

#endif  // UDM_H

