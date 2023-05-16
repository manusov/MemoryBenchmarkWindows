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

Class header for application command line support:
get command line and builds parameters list.
TODO.

*/

#pragma once
#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "UDM.h"
#include "AppLib.h"

class CommandLine : public UDM
{
public:
	CommandLine();
	~CommandLine();
	void writeReport();
	// Command line specific methods.
	COMMAND_LINE_PARMS* getCommandLineParms();      // Get pointer to structure with command line parameters values, variables.
	const OPTION_ENTRY* getOptionsList();           // Get pointer to structures array with options descriptors, constants.
	void resetBeforeParse();                        // Reset options (structure with command line parameters values) to defaults.
	void correctAfterParse();                       // Verify options structure (reserved).
	DWORD parseCommandLine(int argc, char** argv);  // Parse command line and extract parameters to structure.
private:
	// Scenario select data
	static const char* keysHelp[];
	static const char* keysInfo[];
	static const char* keysTest[];
	// Output mode data, screen or file
	static const char* keysOut[];
	// Benchmark scenario data
	static const char* keysAsm[];
	static const char* keysMemory[];
	static const char* keysPage[];
	static const char* keysHt[];
	static const char* keysNuma[];
	// Common data
	static const OPTION_ENTRY options[];
	static COMMAND_LINE_PARMS parms;
};

#endif  // COMMANDLINE_H
