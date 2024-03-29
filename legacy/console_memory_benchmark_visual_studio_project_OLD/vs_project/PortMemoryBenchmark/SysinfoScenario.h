/*
				  MEMORY PERFORMANCE ENGINE FRAMEWORK.
			  Header for System Information Scenario class.
	This class make sequence of different components detectors routines.
*/

#pragma once
#ifndef SYSINFOSCENARIO_H
#define SYSINFOSCENARIO_H

#include <windows.h>
#include <cstdio>
#include "MainDefinitions.h"
#include "MainControlset.h"
#include "AppLib.h"
#include "AppConsole.h"

class SysinfoScenario
{
public:
	SysinfoScenario(char* pointer, size_t limit, int op, SYSTEM_CONTROL_SET* cs);
	~SysinfoScenario();
	void execute();
private:
	static char* saveDst;
	static size_t saveMax;
	static int enumOp;
	static SYSTEM_CONTROL_SET* controlSet;
};

#endif  // SYSINFOSCENARIO_H

