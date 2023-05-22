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

Class header for helpers library:
support display output and file report.
TODO.

*/

#pragma once
#ifndef APPLIB_H
#define APPLIB_H

#include <windows.h>
#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include "Constants.h"

class AppLib
{
public:
	static BOOL initConsole(bool, bool, char*, size_t);
	static size_t getCurrentOutSize();
	// Console output and save report to file.
	static void write(const char* string);
	static void writeColor(const char* string, WORD color);
	static void writeParmGroup(const char* string);
	static void writeParmError(const char* string);
	static void writeParmAndValue(const char* parmName, const char* parmValue, int nameWidth);
	static void writeHexDump(byte* ptr, int size);
	static void writeHexDumpUp();
	static void writeLine(int count);
	static void writeColorLine(int count, WORD color);
	// Write strings to buffer.
	static size_t storeHex8(char* buffer, size_t limit, DWORD32 value, bool h);
	static size_t storeHex16(char* buffer, size_t limit, DWORD32 value, bool h);
	static size_t storeHex32(char* buffer, size_t limit, DWORD32 value, bool h);
	static size_t storeHex64(char* buffer, size_t limit, DWORD64 value, bool h);
	static size_t storeHexPointer(char* buffer, size_t limit, LPVOID ptr, bool h);
	static void storeBitsList(char* buffer, size_t limit, DWORD64 x);
	static void storeCellMemorySize(char* buffer, size_t limit, DWORD64 x, size_t cell);
	static void storeCellMemorySizeInt(char* buffer, size_t limit, DWORD64 x, size_t cell, int mode);
	static void storeBaseAndSize(char* buffer, size_t limit, DWORD64 blockBase, DWORD64 blockSize);
	// Build execution path for save report to application directory (not current directory).
	static BOOL storeExecReportPath(char* buffer, size_t limit, const char* name);
	// Decode Windows error code to error description string.
	static void storeSystemErrorName(char* buffer, size_t limit, DWORD errorCode);
	// Calculate statistics for results vector: min, max, average, median.
	static void calculateStatistics(std::vector<double> data, double& min, double& max, double& average, double& median);
private:
	static bool screenMode;
	static bool fileMode;
	static char* outPointer;
	static size_t maxOutSize;
	static size_t currentOutSize;
	static HANDLE hStdout;
	static CONSOLE_SCREEN_BUFFER_INFO csbi;
	static void colorHelper(WORD color);
	static void colorRestoreHelper();
	static void cellPrintHelper(char* buffer, size_t limit, size_t cell, size_t count);
};

#endif  // APPLIB_H
