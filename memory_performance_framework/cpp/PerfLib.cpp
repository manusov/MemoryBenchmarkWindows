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

Class realization for performance library:
vector sorting, intrinsics and same routines set.
TODO.
Sorting bug: all measurement iterations except first works with already sorted array.
Can use measurement iterations per all 3 steps, not in the each one step,
fencing required for this method.
Warnings at vectorVerify() function,
unsigned int vs DWORD64, required verification for x64 and x86(ia32).
Use size_t for length, note about 4GB limit for x86(ia32).

*/

#include "PerfLib.h"

void PerfLib::rawBuild(int* p, size_t count, DWORD64 repeats, DWORD64& deltaTsc)
{
	DWORDLONG t = __rdtsc();
	for (DWORD64 i = 0; i < repeats; i++)
	{
		int* p1 = p;
		int a = RND_BASE;
		for (size_t j = 0; j < count; j++)
		{
			*(p1++) = a;
			a += RND_DELTA;
		}
	}
	deltaTsc = __rdtsc() - t;
}
void PerfLib::rawSort(int* p, size_t count, DWORD64 repeats, DWORD64& deltaTsc)
{
	// If measurement repeats > 1, bubble sorting runs for already sorted array,
	// note about this when interpreting timings results.
	DWORDLONG t = __rdtsc();
	for (DWORD64 i = 0; i < repeats; i++)
	{
		size_t c = count;
		BOOL f = TRUE;
		while (f && (c > 1))
		{
			int* p1 = p;
			f = FALSE;
			for (size_t j = 0; j < (c - 1); j++)
			{
				int a = *p1;
				int b = *(p1 + 1);
				if (a > b)
				{
					*p1 = b;
					*(p1 + 1) = a;
					f = TRUE;
				}
				p1++;
			}
			c--;
		}
	}
	deltaTsc = __rdtsc() - t;
}
BOOL PerfLib::rawVerify(int* p, size_t count, DWORD64 repeats, DWORD64& deltaTsc)
{
	DWORDLONG t = __rdtsc();
	BOOL f = TRUE;
	for (DWORD64 i = 0; i < repeats; i++)
	{
		int* p1 = p;
		int sum = 0;
		for (size_t i = 0; i < (count - 1); i++)
		{
			sum = sum + *p1;
			if ((*p1) > (*(p1 + 1))) f = FALSE;
			p1++;
		}
		sum = sum + *p1;
		if (sum != RND_SUM) f = FALSE;
	}
	deltaTsc = __rdtsc() - t;
	return f;
}
BOOL PerfLib::vectorBuild(std::vector<int>* v, size_t count, DWORD64 repeats, DWORD64& deltaTsc)
{
	DWORDLONG t = __rdtsc();
	for (DWORD64 i = 0; i < repeats; i++)
	{
		int a = RND_BASE;
		v->clear();
		for (size_t j = 0; j < count; j++)
		{
			v->push_back(a);
			a += RND_DELTA;
		}
	}
	BOOL f = ((v->size()) == count);
	deltaTsc = __rdtsc() - t;
	return f;
}
BOOL PerfLib::vectorSort(std::vector<int>* v, size_t count, DWORD64 repeats, DWORD64& deltaTsc)
{
	DWORDLONG t = __rdtsc();
	for (DWORD64 i = 0; i < repeats; i++)
	{
		sort(v->begin(), v->end());
	}
	BOOL f = ((v->size()) == count);
	deltaTsc = __rdtsc() - t;
	return f;
}
BOOL PerfLib::vectorVerify(std::vector<int>* v, size_t count, DWORD64 repeats, DWORD64& deltaTsc)
{
	DWORDLONG t = __rdtsc();
	BOOL f = TRUE;
	for (DWORD64 i = 0; i < repeats; i++)
	{
		int sum = 0;
		for (size_t j = 0; j < (count - 1); j++)
		{
			sum = sum + (*v)[j];
			if ((*v)[j] > ((*v)[j + 1])) f = FALSE;
		}
		sum = sum + (*v)[count - 1];
		if (sum != RND_SUM) f = FALSE;
	}
	if (v->size() != count) f = FALSE;
	deltaTsc = __rdtsc() - t;
	return f;
}
