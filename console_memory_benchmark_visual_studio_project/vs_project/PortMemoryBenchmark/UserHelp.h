/*
	MEMORY PERFORMANCE ENGINE FRAMEWORK.
		Header for User Help class.
*/

#pragma once
#ifndef USERHELP_H
#define USERHELP_H

#include <windows.h>
#include <cstdio>
#include "MainDefinitions.h"
#include "MainControlset.h"
#include "AppLib.h"
#include "AppConsole.h"

class UserHelp
{
public:
	UserHelp(char* pointer, size_t limit, int op, const OPTION_ENTRY* oplist);
	~UserHelp();
	void execute();
private:
	static char* saveDst;
	static size_t saveMax;
	static int enumOp;
	static const OPTION_ENTRY* optionsList;
	static const char* exampleString;
};

#endif  // USERHELP_H

