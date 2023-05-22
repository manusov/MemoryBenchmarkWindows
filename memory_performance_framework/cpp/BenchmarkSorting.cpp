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

Class realization for sorting test scenario.

TODO.

Sorting bug: all measurement iterations except first works with already sorted array.
Can use measurement iterations per all 3 steps, not in the each one step,
fencing required for this method.

COMPILER_PERFORMANCE_LIST is over-engineering, 
this unification required too many unused input parameters,
nullptr for two vectors, two array pointers, selector and other.
can use direct calls?

Note about cache effects, vector runs after array. Pre-fill memory.
No, memory blocks at differrent address.

Use command line parameters for make this test flexible,
include iterations with start/stop/delta,
design progress and summary tables, see memory benchmark tables as pattern.

Use ThreadsBuilder class for make this test multithread.

Measure TSC clock and calculate results at seconds.

Add test with ( build 1 time / sort 1 time / verify 1 time ) x REPEATS times.

Use "Release" build for optimal code.
Use "__debugbreak();" for make breakpoints in the "Release" build.

*/

#include "BenchmarkSorting.h"

BenchmarkSorting::BenchmarkSorting(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp) :
	USM(op, cs, pp)
{
}
BenchmarkSorting::~BenchmarkSorting()
{
}
void BenchmarkSorting::execute()
{
	char msg[APPCONST::MAX_TEXT_STRING];
	int* p = new int[COUNT];
	std::vector<int> v;
	v.reserve(COUNT);           // This required for performance optimization only.
	DWORD64 deltaTsc = 0;
	BOOL status = FALSE;

	// Measure TSC frequency.
	FncLoader* fnld = controlSet->pFunctionsLoader;
	FUNCTIONS_LIST* fl = fnld->getFunctionsList();
	DWORD64 hz = 0;
	if ((fl) && (fl->dll_MeasureTsc))
	{
		DWORD64 dt = 0;
		if (fl->dll_MeasureTsc(&dt))
		{
			hz = dt;
		}
	}
	double f = static_cast<double>(hz);
	double t = 1000.0 / f;

	if (hz)
	{
		snprintf(msg, APPCONST::MAX_TEXT_STRING, "TSC = %.3f MHz, fixed scenario sort %d integers, single thread.\r\n\r\n", f / 1000000.0, COUNT);
		AppLib::writeColor(msg, APPCONST::GROUP_COLOR);
	}
	else
	{
		snprintf(msg, APPCONST::MAX_TEXT_STRING, "TSC measurement failed, fixed scenario sort %d integers, single thread.\r\n\r\n", COUNT);
		AppLib::writeColor(msg, APPCONST::ERROR_COLOR);
	}

	// Raw array sort test.
	
	PerfLib::rawBuild(p, COUNT, 1, deltaTsc);
	if(hz) snprintf(msg, APPCONST::MAX_TEXT_STRING, "Create random array: %.3f ms.\r\n", deltaTsc * t);
	else snprintf(msg, APPCONST::MAX_TEXT_STRING, "Create random array, delta TSC = %llu.\r\n", deltaTsc);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	
	PerfLib::rawSort(p, COUNT, 1, deltaTsc);
	if (hz) snprintf(msg, APPCONST::MAX_TEXT_STRING, "Sorting random array: %.3f ms.\r\n", deltaTsc * t);
	else snprintf(msg, APPCONST::MAX_TEXT_STRING, "Sorting random array, delta TSC = %llu.\r\n", deltaTsc);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	
	status = PerfLib::rawVerify(p, COUNT, 1, deltaTsc);
	if (status)
	{
		if(hz) snprintf(msg, APPCONST::MAX_TEXT_STRING, "Verify sorted random array PASSED: %.3f ms.\r\n\r\n", deltaTsc * t);
		else snprintf(msg, APPCONST::MAX_TEXT_STRING, "Verify sorted random array PASSED, delta TSC = %llu.\r\n\r\n", deltaTsc);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	}
	else
	{
		if (hz) snprintf(msg, APPCONST::MAX_TEXT_STRING, "Verify sorted random array FAILED: %.3f ms.\r\n\r\n", deltaTsc * t);
		else snprintf(msg, APPCONST::MAX_TEXT_STRING, "Verify sorted random array FAILED, delta TSC = %llu.\r\n\r\n", deltaTsc);
		AppLib::writeColor(msg, APPCONST::ERROR_COLOR);
	}

	// Vector sort test.

	if (PerfLib::vectorBuild(&v, COUNT, 1, deltaTsc))
	{
		if (hz) snprintf(msg, APPCONST::MAX_TEXT_STRING, "Create random vector: %.3f ms.\r\n", deltaTsc * t);
		else snprintf(msg, APPCONST::MAX_TEXT_STRING, "Create random vector, delta TSC = %llu.\r\n", deltaTsc);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		if (PerfLib::vectorSort(&v, COUNT, 1, deltaTsc))
		{
			if(hz) snprintf(msg, APPCONST::MAX_TEXT_STRING, "Sorting random vector: %.3f ms.\r\n", deltaTsc * t);
			else snprintf(msg, APPCONST::MAX_TEXT_STRING, "Sorting random vector, delta TSC = %llu.\r\n", deltaTsc);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			if (PerfLib::vectorVerify(&v, COUNT, 1, deltaTsc))
			{
				if (hz) snprintf(msg, APPCONST::MAX_TEXT_STRING, "Verify sorted random array PASSED: %.3f ms.\r\n", deltaTsc * t);
				else snprintf(msg, APPCONST::MAX_TEXT_STRING, "Verify sorted random array PASSED, delta TSC = %llu.\r\n", deltaTsc);
				AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			}
			else
			{
				if (hz) snprintf(msg, APPCONST::MAX_TEXT_STRING, "Verify sorted random array FAILED: %.3f ms.\r\n", deltaTsc * t);
				else snprintf(msg, APPCONST::MAX_TEXT_STRING, "Verify sorted random array FAILED, delta TSC = %llu.\r\n", deltaTsc);
				AppLib::writeColor(msg, APPCONST::ERROR_COLOR);
			}
		}
		else
		{
			AppLib::writeColor("Vector sort FAILED.\r\n\r\n", APPCONST::ERROR_COLOR);
		}
	}
	else
	{
		AppLib::writeColor("Vector build FAILED.\r\n\r\n", APPCONST::ERROR_COLOR);
	}
	
	delete[] p;
}
