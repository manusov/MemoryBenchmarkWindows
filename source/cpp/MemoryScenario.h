#ifndef MEMORYSCENARIO_H
#define MEMORYSCENARIO_H

#include <windows.h>
#include <cstdio>
#include "maindefinitions.h"
#include "maincontrolset.h"
#include "AppLib.h"
#include "AppConsole.h"

class MemoryScenario
{
	public:
		MemoryScenario( char* pointer, size_t limit, int op, 
						SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp );
		~MemoryScenario( );
		void execute( );
	private:
		static char* saveDst;
		static size_t saveMax;
		static int enumOp;
		static SYSTEM_CONTROL_SET* controlSet;
		static COMMAND_LINE_PARMS* pCmd;
};

#endif  // MEMORYSCENARIO_H
