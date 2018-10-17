#ifndef COMMANDLINE_H
#define COMMANDLINE_H

#include <windows.h>
#include <cstdio>
#include "GlobalDeclarations.h"

// Class declaration
class CommandLine
{
	public:
		CommandLine( );
		~CommandLine( );
		COMMAND_LINE_PARMS* getCommandLineParms( );
	    void resetBeforeParse( );
	    void correctAfterParse( );
	    DWORD parseCommandLine( int argc, char** argv );
		char* getStatusString( );
	private:
		static const char* keysAsm[];
		static const char* keysMemory[];
		static const char* keysPage[];
		static const OPTION_ENTRY options[];
		static COMMAND_LINE_PARMS parms;
		static char s[];
};

#endif  // COMMANDLINE_H
