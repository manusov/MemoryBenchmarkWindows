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

Class header for functions loader: 
supports OS DLL and ASM DLL functions dynamical import.
TODO.

*/

#pragma once
#ifndef FNCLOADER_H
#define FNCLOADER_H

#include <windows.h>
#include "Constants.h"
#include "UDM.h"
#include "AppLib.h"

class FncLoader : public UDM
{
public:
	FncLoader();
	~FncLoader();
	void writeReport();
private:
	static const LIBRARY_ENTRY libDirectory[];
	static const LPCSTR fncNamesKernel32[];
	static const LPCSTR fncNamesAdvapi32[];
	static const LPCSTR fncNamesAsm[];
};

#endif  // FNCLOADER_H
