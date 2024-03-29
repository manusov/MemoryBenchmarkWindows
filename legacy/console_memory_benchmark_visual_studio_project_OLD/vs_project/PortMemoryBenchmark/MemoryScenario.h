/*
					 MEMORY PERFORMANCE ENGINE FRAMEWORK.
				  Header for Memory Benchmark Scenario class.
	This class make sequence of different initialization end measurement routines.
*/

#pragma once
#ifndef MEMORYSCENARIO_H
#define MEMORYSCENARIO_H

#include <windows.h>
#include <cstdio>
#include "MainDefinitions.h"
#include "MainControlSet.h"
#include "AppLib.h"
#include "AppConsole.h"

class MemoryScenario
{
public:
	MemoryScenario(char* pointer, size_t limit, int op,
		SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp);
	~MemoryScenario();
	void execute();
private:
	static char* saveDst;
	static size_t saveMax;
	static int enumOp;
	static SYSTEM_CONTROL_SET* controlSet;
	static COMMAND_LINE_PARMS* pCmd;
};

#endif  // MEMORYSCENARIO_H

