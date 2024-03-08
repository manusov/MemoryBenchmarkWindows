/*

MEMORY PERFORMANCE ENGINE (MPE) FRAMEWORK.
------------------------------------------
System informationand performance analysing application,
include console version of NCRB(NUMA CPUand RAM Benchmark).
See also memory benchmark projects :
https://github.com/manusov/NCRBv2
https://github.com/manusov/MemoryBenchmarkWindows
Experimental samples :
https://github.com/manusov/Prototyping
Special thanks for C++ lessons :
https://ravesli.com/uroki-cpp/

Class header for application constants.
TODO.
1) Check this data not duplicated in the exe file
by header duplication in the *.cpp files.

*/

#pragma once
#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <windows.h>
#include "Definitions.h"

namespace APPCONST
{
	// Application strings and report file name.
	const char* const MSG_APPLICATION = "Debug template v0.00.03.";
	const char* const MSG_STARTING = "Starting...";
	const char* const DEFAULT_OUT_NAME = "output.txt";

	// Application strings for console NCRB.
	const char* const APPLICATION_STRING  = "NUMA CPU&RAM Benchmarks. Console Edition.";
	const char* const COPYRIGHT_STRING    = "(C) 2024 Ilya Manusov.";
	const char* const ANY_KEY_STRING      = "Press any key...";
	const char* const INPUT_FILE_NAME     = "input.txt";
	const char* const OUTPUT_FILE_NAME    = "output.txt";
	constexpr size_t PATH_SIZE = 1024;
	constexpr size_t TEXT_SIZE = 100 * 1024;
	constexpr size_t TX_SIZE   = 4095;
	#if _WIN64
	const char* const BUILD_STRING        = "v0.02.03 for Windows x64.";
	#elif _WIN32
	const char* const BUILD_STRING        = "v0.02.03 for Windows ia32.";
	#endif

	// Parameters depend on x64 or ia32 build.
	#if _WIN64
	#define NATIVE_WIDTH_64
	const char* const MSG_BUILD     = "Windows x64";
	const char* const DLL_NAME      = "MPE64.DLL";
	const char* const DLL_PATH_NAME = "MPE64.DLL";
	#elif _WIN32
	#define NATIVE_WIDTH_32
	const char* const MSG_BUILD     = "Windows ia32";
	const char* const DLL_NAME      = "MPE32.DLL";
	const char* const DLL_PATH_NAME = "MPE32.DLL";
	#endif

	// Console output parameters: colors.
	constexpr WORD PARM_COLOR         = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	constexpr WORD VALUE_COLOR        = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	constexpr WORD GROUP_COLOR        = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	constexpr WORD NO_ERROR_COLOR     = FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	constexpr WORD ERROR_COLOR        = FOREGROUND_RED | FOREGROUND_INTENSITY;
	constexpr WORD DUMP_ADDRESS_COLOR = FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	constexpr WORD DUMP_DATA_COLOR    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
	constexpr WORD DUMP_TEXT_COLOR    = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
	constexpr WORD TABLE_COLOR        = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;

	// Console output parameters: sizes.
	constexpr int MAX_TEXT_STRING      = 160;
	constexpr int DUMP_BYTES_PER_LINE  = 16;
	constexpr int DUMP_WIDTH           = 80;
	constexpr int TABLE_WIDTH          = 80;
	constexpr size_t MAX_OUT_FILE_SIZE = 4 * 1024 * 1024;

	// Topology and threads timing operational parameters.
	constexpr int MAXIMUM_THREADS          = 256;
	constexpr int MAXIMUM_THREADS_PER_NODE = 64;
	constexpr int MAXIMUM_NODES            = 64;
	constexpr int THREAD_TIMEOUT           = 60000;

	// Benchmark scenario options.
	constexpr int OPTION_NOT_SET = -1;
	constexpr int CONST_DRAM_BLOCK = 32 * 1024 * 1024;

	constexpr int DEFAULT_HELP_SCENARIO = OPTION_NOT_SET;
	constexpr int DEFAULT_INFO_SCENARIO = OPTION_NOT_SET;
	constexpr int DEFAULT_TEST_SCENARIO = OPTION_NOT_SET;
	constexpr int DEFAULT_INPUT         = IN_DEFAULT;
	constexpr int DEFAULT_OUTPUT        = OUT_SCREEN;

	constexpr int DEFAULT_ASM_METHOD          = OPTION_NOT_SET;
	constexpr int DEFAULT_MEMORY_OBJECT       = OPTION_NOT_SET;
	constexpr int DEFAULT_THREADS_COUNT       = OPTION_NOT_SET;
	constexpr int DEFAULT_MEASUREMENT_REPEATS = OPTION_NOT_SET;
	constexpr int DEFAULT_ADAPTIVE_REPEATS    = OPTION_NOT_SET;
	constexpr int DEFAULT_HT_MODE             = OPTION_NOT_SET;
	constexpr int DEFAULT_NUMA_MODE           = OPTION_NOT_SET;

	// 0-50, total 51 methods.
	constexpr int MAXIMUM_ASM_METHOD = 50;

	constexpr int APPROXIMATION_REPEATS      = 2000000;
	constexpr int APPROXIMATION_REPEATS_L1   = 2000000;
	constexpr int APPROXIMATION_REPEATS_L2   = 500000;
	constexpr int APPROXIMATION_REPEATS_L3   = 10000;
	constexpr int APPROXIMATION_REPEATS_L4   = 1000;
	constexpr int APPROXIMATION_REPEATS_DRAM = 200;
	constexpr int REPEATS_DIVISOR_LATENCY    = 20;
	constexpr double CALIBRATION_TARGET_TIME = 1.0;

	constexpr int DEFAULT_START_SIZE_BYTES = 4096;
	constexpr int DEFAULT_END_SIZE_BYTES   = 65536;
	constexpr int DEFAULT_DELTA_SIZE_BYTES = 4096;

	// Constants used for print memory block size.
	constexpr int KILO = 1024;
	constexpr int MEGA = 1024 * 1024;
	constexpr int GIGA = 1024 * 1024 * 1024;
}

#endif  // CONSTANTS_H
