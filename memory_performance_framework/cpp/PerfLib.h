/*

MEMORY PERFORMANCE ENGINE (MPE) FRAMEWORK.
-------------------------------------------
System information and performance analysing application,
include console version of NCRB (NUMA CPU and RAM Benchmark).
See also memory benchmark projects:
https://github.com/manusov/NCRBv2
https://github.com/manusov/MemoryBenchmarkWindows
Experimental samples:
https://github.com/manusov/Prototyping
Special thanks for C++ lessons:
https://ravesli.com/uroki-cpp/

Class header for performance library:
vector sorting, intrinsics and same routines set.
TODO.

*/

#pragma once
#ifndef PERFLIB_H
#define PERFLIB_H

#include <windows.h>
#include <vector>
#include <algorithm>
#include "Constants.h"

class PerfLib
{
public:
	static void rawBuild(int* p, size_t count, DWORD64 repeats, DWORD64& deltaTsc);
	static void rawSort(int* p, size_t count, DWORD64 repeats, DWORD64& deltaTsc);
	static BOOL rawVerify(int* p, size_t count, DWORD64 repeats, DWORD64& deltaTsc);
	static BOOL vectorBuild(std::vector<int>* v, size_t count, DWORD64 repeats, DWORD64& deltaTsc);
	static BOOL vectorSort(std::vector<int>* v, size_t count, DWORD64 repeats, DWORD64& deltaTsc);
	static BOOL vectorVerify(std::vector<int>* v, size_t count, DWORD64 repeats, DWORD64& deltaTsc);
private:
	static constexpr int RND_BASE  = 0;
	static constexpr int RND_DELTA = 0x73100001;
	static constexpr int RND_SUM   = 0xDE164160;
};

#endif  // PERFLIB_H
