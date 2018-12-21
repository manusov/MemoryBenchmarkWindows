#ifndef SYSINFOSCENARIO_H
#define SYSINFOSCENARIO_H

#include <windows.h>
#include <cstdio>
#include "maindefinitions.h"
#include "maincontrolset.h"
#include "AppLib.h"
#include "AppConsole.h"

class SysinfoScenario
{
	public:
		SysinfoScenario( char* pointer, size_t limit, int op, SYSTEM_CONTROL_SET* cs );
		~SysinfoScenario( );
		void execute( );
	private:
		static char* saveDst;
		static size_t saveMax;
		static int enumOp;
		static SYSTEM_CONTROL_SET* controlSet; 
};

#endif  // SYSINFOSCENARIO_H
