#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <windows.h>
#include <cstdio>
#include "maindefinitions.h"
#include "AppLib.h"

class CommandLine
{
	public:
		CommandLine( );
		~CommandLine( );
		COMMAND_LINE_PARMS* getCommandLineParms( );
		const OPTION_ENTRY* getOptionsList( );
	    void resetBeforeParse( );
	    void correctAfterParse( );
	    DWORD parseCommandLine( int argc, char** argv );
		char* getStatusString( );
	private:
		// Scenario select data
		static const char* keysHelp[];
		static const char* keysInfo[];
		static const char* keysTest[];
		// Benchmark scenario data
		static const char* keysAsm[];
		static const char* keysMemory[];
		static const char* keysPage[];
		static const char* keysHt[];
		static const char* keysNuma[];
		// Common data
		static const OPTION_ENTRY options[];
		static COMMAND_LINE_PARMS parms;
		static char s[];
};

#endif  // COMMANDLINE_H
