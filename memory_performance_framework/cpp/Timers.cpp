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

Class for timers test scenario.
TODO.

*/

#include "Timers.h"

union T64
{
	FILETIME fileTime;
	LARGE_INTEGER value;
	DWORD64 raw;
};

Timers::Timers(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp) :
	USM(op, cs, pp)
{
}
Timers::~Timers()
{
	// ... destructor functionality yet reserved ...
}
void Timers::execute()
{
	FncLoader* fnld = controlSet->pFunctionsLoader;
	FUNCTIONS_LIST* fl = fnld->getFunctionsList();
	if (fl)
	{
		// Variables used for all timers tests.
		char msg[APPCONST::MAX_TEXT_STRING];
		constexpr double NS_FT = 100.0;
		T64 t1;
		T64 t2;
		LARGE_INTEGER performanceFrequency;
		performanceFrequency.QuadPart = 0;
		LARGE_INTEGER tscFrequencyFt;
		tscFrequencyFt.QuadPart = 0;
		LARGE_INTEGER tscFrequencyPerf;
		tscFrequencyPerf.QuadPart = 0;

		// Get OS performance counter frequency, calculate period = 1/frequency, measure this timer resolution.
		if ((fl->api_QueryPerformanceFrequency) && (fl->api_QueryPerformanceFrequency(&t1.value)))
		{
			performanceFrequency.QuadPart = t1.value.QuadPart;
			double f = static_cast<double>(t1.value.QuadPart);
			double t = 1.0 / f;
			snprintf(msg, APPCONST::MAX_TEXT_STRING,
				"Performance frequency = %.3f MHz, period = %.1f nanoseconds.\r\n",
				f * 1E-6, t * 1E9);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			if ((fl->api_QueryPerformanceCounter) && (fl->api_QueryPerformanceCounter)(&t1.value))
			{
				DWORD64 deltaMin = ULLONG_MAX;
				DWORD64 deltaMax = 0;
				DWORD64 delta = 0;
				for (int i = 0; i < 20; i++)
				{
					do {
						fl->api_QueryPerformanceCounter(&t2.value);
						delta = t2.value.QuadPart - t1.value.QuadPart;
					} while (!delta);
					t1.value.QuadPart = t2.value.QuadPart;
					if (i)
					{
						if (delta < deltaMin) deltaMin = delta;
						if (delta > deltaMax) deltaMax = delta;
					}
				}
				double nsMin = deltaMin * t * 1E9;
				double nsMax = deltaMax * t * 1E9;
				snprintf(msg, APPCONST::MAX_TEXT_STRING,
					"Performance frequency clock resolution: min = %llu (%.1f ns), max = %llu (%.1f ns).\r\n",
					deltaMin, nsMin, deltaMax, nsMax);
				AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			}
			else
			{
				AppLib::writeColor("Query performance counter failed.", APPCONST::ERROR_COLOR);
			}
		}
		else
		{
			AppLib::writeColor("Query performance frequency failed.", APPCONST::ERROR_COLOR);
		}

		// Measure OS file time counter resolution.
		DWORD64 deltaMin = ULLONG_MAX;
		DWORD64 deltaMax = 0;
		DWORD64 delta = 0;
		GetSystemTimeAsFileTime(&t1.fileTime);
		for (int i = 0; i < 20; i++)
		{
			do {
				GetSystemTimeAsFileTime(&t2.fileTime);
				delta = t2.value.QuadPart - t1.value.QuadPart;
			} while (!delta);
			t1.value.QuadPart = t2.value.QuadPart;
			if (i)
			{
				if (delta < deltaMin) deltaMin = delta;
				if (delta > deltaMax) deltaMax = delta;
			}
		}
		double nsMin = deltaMin * NS_FT;
		double nsMax = deltaMax * NS_FT;
		snprintf(msg, APPCONST::MAX_TEXT_STRING,
			"File time clock resolution: min = %llu (%.f ns), max = %llu (%.f ns).\r\n",
			deltaMin, nsMin, deltaMax, nsMax);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);

		// Measure TSC frequency by file time, calculate period = 1/frequency.
		if (fl->dll_MeasureTsc(&t1.raw))
		{
			tscFrequencyFt.QuadPart = t1.value.QuadPart;
			double f = static_cast<double>(t1.value.QuadPart);
			double t = 1.0 / f;
			snprintf(msg, APPCONST::MAX_TEXT_STRING,
				"TSC (by file time) frequency = %.3f MHz, period = %.3f nanoseconds.\r\n",
				f * 1E-6, t * 1E9);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		}
		else
		{
			AppLib::writeColor("Measure TSC by file time failed.", APPCONST::ERROR_COLOR);
		}

		// Measure TSC frequency by performance frequency, calculate period = 1/frequency.
		if ((performanceFrequency.QuadPart) &&
			(fl->dll_MeasureTscByPcounter(&t1.value, fl->api_QueryPerformanceCounter, performanceFrequency)))
		{
			tscFrequencyPerf.QuadPart = t1.value.QuadPart;
			double f = static_cast<double>(t1.value.QuadPart);
			double t = 1.0 / f;
			snprintf(msg, APPCONST::MAX_TEXT_STRING,
				"TSC (by performance counter) frequency = %.3f MHz, period = %.3f nanoseconds.\r\n",
				f * 1E-6, t * 1E9);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		}
		else
		{
			AppLib::writeColor("Measure TSC by performance frequency failed.", APPCONST::ERROR_COLOR);
		}

		// Measure TSC resolution.
		if (tscFrequencyPerf.QuadPart)
		{
			deltaMin = ULLONG_MAX;
			deltaMax = 0;
			t1.value.QuadPart = __rdtsc();
			for (int i = 0; i < 20; i++)
			{
				do {
					t2.value.QuadPart = __rdtsc();
					delta = t2.value.QuadPart - t1.value.QuadPart;
				} while (!delta);
				t1.value.QuadPart = t2.value.QuadPart;
				if (delta < deltaMin) deltaMin = delta;
				if (delta > deltaMax) deltaMax = delta;
			}
			nsMin = deltaMin * (1E9 / static_cast<double>(tscFrequencyPerf.QuadPart));
			nsMax = deltaMax * (1E9 / static_cast<double>(tscFrequencyPerf.QuadPart));
			snprintf(msg, APPCONST::MAX_TEXT_STRING,
				"TSC clock resolution: min = %llu (%.3f ns), max = %llu (%.3f ns).\r\n",
				deltaMin, nsMin, deltaMax, nsMax);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		}
		else
		{
			AppLib::writeColor("Measure TSC clock resolution failed.", APPCONST::ERROR_COLOR);
		}

		// Stability test for measurement TSC by file time and by performance counter.
		if ((tscFrequencyFt.QuadPart) && (tscFrequencyPerf.QuadPart))
		{
			AppLib::writeColor("\r\nTSC measurement stability test by FTIME/PCNT (MHz).\r\n", APPCONST::VALUE_COLOR);
			for (int i = 0; i < 10; i++)
			{
				if ((fl->dll_MeasureTsc(&t1.raw)) &&
					(fl->dll_MeasureTscByPcounter(&t2.value, fl->api_QueryPerformanceCounter, performanceFrequency)))
				{
					double f1 = static_cast<double>(t1.value.QuadPart) * 1E-6;
					double f2 = static_cast<double>(t2.value.QuadPart) * 1E-6;
					snprintf(msg, APPCONST::MAX_TEXT_STRING, "%.5f | %.5f.\r\n", f1, f2);
					AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
				}
				else
				{
					AppLib::writeColor("TSC measurement failed.\r\n", APPCONST::ERROR_COLOR);
					break;
				}
			}
		}
		else
		{
			AppLib::writeColor("TSC measurement stability test not available.\r\n", APPCONST::ERROR_COLOR);
		}
	}
	else
	{
		AppLib::writeColor("Timer test pointers initialization failed.\r\n", APPCONST::ERROR_COLOR);
	}
}
