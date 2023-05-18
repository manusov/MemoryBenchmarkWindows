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

Main source file.
TODO.

1)+  Memory size for benchmarks write at KB/MB/GB units, now bytes.
     See file Benchmark.cpp, line 554.
     But same units for all test session, even if optimal unit = f(block size) changed.
2)+  Correct bytes per instruction (BPI) table for MOV32/MOV64 and some other.
     Verify include STOS/MOVS.
3)+  Bug if block size = 2GB.
4)   Use Performance Counter instead File Time for precision measure TSC clock.
5)   Optimize benchmark running sequences and serialization for more accurate results.
6)   Can set name for output report file.
7)   Use input scenario file, can set name for it.
8)   Use inheritance for test scenarios: memory, storage, cpu, gpu, timers.
9)   Why console initialization error when output redirection.
10)  See "TODO" list below main() routine at this file.

*/

#include <windows.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include "Constants.h"
#include "AppLib.h"
#include "MainTask.h"

int main(int argc, char** argv)
{
	using namespace std;
	int status = 0;
	bool fileMode = true;
	bool screenMode = true;
	char msg[APPCONST::MAX_TEXT_STRING];
	const char* outName = APPCONST::DEFAULT_OUT_NAME;
	char* outPtr = (char*)malloc(APPCONST::MAX_OUT_FILE_SIZE);
	char* const backOutPtr = outPtr;
	cout << APPCONST::MSG_STARTING << endl;
	if (backOutPtr)
	{
		if (AppLib::initConsole(screenMode, fileMode, backOutPtr, APPCONST::MAX_OUT_FILE_SIZE))
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%s (%s).\r\n", APPCONST::MSG_APPLICATION, APPCONST::MSG_BUILD);
			AppLib::write(msg);

			// int localStatus = task();
			int localStatus = maintask(argc, argv);

			if (localStatus == 0)
			{
				snprintf(msg, APPCONST::MAX_TEXT_STRING, "\r\nOK.\r\n");
				AppLib::writeColor(msg, APPCONST::NO_ERROR_COLOR);
			}
			else
			{
				snprintf(msg, APPCONST::MAX_TEXT_STRING, "\r\nFAILED (%d).\r\n", localStatus);
				AppLib::writeColor(msg, APPCONST::ERROR_COLOR);
			}

			if (fileMode)
			{
				ofstream out;
				out.open(outName, ios::binary | ios::out);
				if (out.is_open())
				{
					out.write(backOutPtr, AppLib::getCurrentOutSize());
					if (out.fail())
					{
						cout << "Error writing output file." << endl;
						status = 4;
					}
					else
					{
						int n = (int)out.tellp();
						snprintf(msg, APPCONST::MAX_TEXT_STRING, "%d bytes saved to output file.", n);
						cout << msg << endl;
					}
					out.close();
				}
				else
				{
					cout << "Output file create error." << endl;
					status = 3;
				}
			}
			else
			{
				cout << "Report file disabled by option." << endl;
			}
		}
		else
		{
			cout << "Console initialization error." << endl;
			status = 2;
		}
		free(backOutPtr);
	}
	else
	{
		cout << "Memory allocation error." << endl;
		status = 1;
	}
	return status;
}


/*

MEMORY PERFORMANCE ENGINE FRAMEWORK: OLD COMMENTS, SEE ALSO:

https://github.com/manusov/MemoryBenchmarkWindows/tree/master/console_memory_benchmark_visual_studio_project
https://github.com/manusov/NCRBv2

F:\Data\Data_2023.rar\Data_2023\snapshots_2023
 - RAR архив, размер исходных файлов 5 121 113 487 байт

...\DeviceManagerOldBeforeCommitGitHub.rar\DeviceManagerOldBeforeCommitGithub
 - RAR архив, размер исходных файлов 397 940 байт

...\_GitHub_24_04_2023.rar\_GitHub\MemoryBenchmarkWindows
 - RAR архив, размер исходных файлов 971 146 244 байт

...\_GitHub_24_04_2023.rar\_GitHub\MemoryBenchmarkWindows\console_memory_benchmark_visual_studio_project
 - RAR архив, размер исходных файлов 971 146 244 байт

*/

/*

MEMORY PERFORMANCE ENGINE FRAMEWORK.
MAIN MODULE.

*/

/*

TODO:
1)  + Make minimal user help.
2)  + Test builds ia32 and x64, verify at some options set.
3)  + Inspect printf/snprintf usage. 64-bit printf arguments can crush printf in the 32-bit mode. Not use %I64D.
4)  + Some format changes, reduce number of strings, ( data ) after done, example ...done( dTSC= ... ).
5)  + Special visualization for latency.
--- v0.60.03 ---
6) + Bug with 32-bit copy, address *8, required *4.
7) + Crush: 32-bit mode: writemmx64.
8) + Crush: 32-bit mode: latencylcm, latencyrdrand.
9) + Add MMX modes. This can be highest speed for 32-bit CPU, example Athlon/Duron for socket A (Socket 462).
	  + backup v0.60.02 to "C:\2-cpp\06-mpe-v06x-OLD", update this version number to v0.60.03
	  + add MMX methods to ia32 library, reorder methods list is INCOMPATIBLE modification, this application must be updated
	  + add MMX methods to x64 library, reorder methods list is INCOMPATIBLE modification, this application must be updated.
	  + maindefinitions.h ( +42, +69 )
	  + commandline.cpp ( +38 )
	  + processordetector.h ( +49 )
	  + processordetector.cpp ( +18, +28, +54, +95, +123, +320, +364 ).
10) + Asm method id must be decoded as routine name, otherwise user view unknown method, numeric id only.
11) + Remove "GUI" option from DLLs. Recompile for 32/64.
12) + Bug with select max temporal and max non temporal methods, shifts and comparing 32 instead 64.
13) + File report modes.
	 + base functionality.
	 + refactor saveDst vs txp operations, sometimes can remove txp.
		use saveDst, saveMax for AppConsole::transmit, REMOVE txp and MAX_TRANSIT.
	 + report copyright when out=file.
	 + number of spaces when exit on different scenarios.

---
14) Bug return from memory test scenario when objects not released, if errors detected, by error handling branch.
15) Variables (some of): blockMax, blockSize, blockCount, blockDelta make 64-bit, prevent overflows. Plus other width regularity.
16) Detalize WinAPI errors codes. Function printSystemError, replace printf to snprintf at this routine.
17) Detailze status string error codes, use "s" at classes.
18) Return some log, rejected when go from v0.5x to v0.6x. Status string "s", plus OS error code. See 2 previous items.
19) Find optimal method for blank structures and arrays.
20) Sequencing: user help must be without hardware initialization, get command line must be before hardware initialization.
21) Bug if ht=off when HT not supported, ThreadsBuilder.cpp, line 260. Or correct before call buildThreadsList().

22) 64-bit verify all options. Include AVX512, NUMA, Processor Groups. Required platform.
23) 32-bit verify all options. Include AVX512, NUMA, Processor Groups. Required platform.

24) ThreadsBuilder.cpp , line 333, use terminate threads by return and wait by thread handle, not by TerminateThread().

25) learn warnings:

Remove this requirements:
 - C/C++ - Preprocessor - Preprocessor definitions
_CRT_SECURE_NO_WARNINGS

Correct:
"MPE native library" to "NCRB ..."

For ia32 и x64.
For Debug и Release.

1) EBX:EBP = repeats counter for ia32 version, must be 64-bit even for 32-bit version.
2) Also correct C++ code, repeats counter must be 64-bit.
3) Special verify adaptive mode.
4) Latency measurement bug for multi-thread.

ThreadsBuilder.cpp (47)
ThreadsBuilder.cpp (334).

Customized options:

 - Char set - Unicode.

 - C/C++ - Preprocessor - Preprocessor definitions
_CRT_SECURE_NO_WARNINGS

Project Properties ->
 C/C++ ->
  Code Generation ->
   Runtime Library = Multi-threaded (/MT)

Options string example:
 test=memory start=1M end=128M step=1M asm=ntpreadsse128long threads=8 ht=off repeats=60

*/

