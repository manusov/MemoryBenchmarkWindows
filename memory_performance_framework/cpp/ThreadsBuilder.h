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

Class header for execution threads management:
enumerate threads, run and stop execution.
TODO.

*/

#pragma once
#ifndef THREADSBUILDER_H
#define THREADSBUILDER_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "UDM.h"
#include "AppLib.h"

class ThreadsBuilder : public UDM
{
public:
	ThreadsBuilder(FUNCTIONS_LIST* pf);
	~ThreadsBuilder();
	void writeReport();
	// Threads builder specific methods.
	SYSTEM_THREADS_DATA* getThreadsList();             // Get pointer to list of threads control structures.
	// Threads management functions.
	BOOL buildThreadsList(INPUT_CONSTANTS* pInputC, INPUT_VARIABLES* pInputV, SYSTEM_NUMA_DATA* pDomain);  // Build list of threads.
	BOOL updateThreadsList(INPUT_VARIABLES* pInputV);  // Update list of threads per measurement iteration.
	BOOL releaseThreadsList();                         // Terminate threads, release handles.
	BOOL runThreads(OUTPUT_VARIABLES* pOutputV);       // Initialize and run threads.
	BOOL restartThreads(OUTPUT_VARIABLES* pOutputV, BOOL waitWork = TRUE);   // Restart already initialized threads.
private:
	static SYSTEM_THREADS_DATA threadsData;
	// Helpers methods.
	void helperBlankThread(THREAD_CONTROL_ENTRY* pT);
	void helperBlankAffinity(GROUP_AFFINITY* pA);
};

#endif  // THREADSBUILDER_H


