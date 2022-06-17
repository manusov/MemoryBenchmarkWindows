/*
					 MEMORY PERFORMANCE ENGINE FRAMEWORK.
					  Header for functions loader class.
	This class make dynamical import for Windows API and this application DLL.
*/

#pragma once
#ifndef FUNCTIONSLOADER_H
#define FUNCTIONSLOADER_H

#include <windows.h>
#include <cstdio>
#include "MainDefinitions.h"
#include "AppLib.h"

typedef struct {
	LPCSTR libName;
	LPCSTR* libList;
} PRINT_LOADED_FUNCTION;

class FunctionsLoader
{
public:
	FunctionsLoader();
	~FunctionsLoader();
	FUNCTIONS_LIST* getFunctionsList();
	void getFunctionsText(LPSTR& dst, size_t& max);
	char* getStatusString();
private:
	static const char* sysName;
	static const char* dllName;
	static const char* sysFunctions[];
	static const char* dllFunctions[];
	static const char* tableUp;
	static const PRINT_LOADED_FUNCTION printLoaded[];
	static HMODULE sysHandle;
	static HMODULE dllHandle;
	static FUNCTIONS_LIST f;
	static char s[];
	// Helpers functions
	BOOL functionCheck(void* functionPointer, const char* functionName, const char* libName, DWORD& errorCode);
	BOOL libraryLoad(HMODULE& moduleHandle, const char* moduleName, DWORD& error);
	void libraryUnload(HMODULE moduleHandle, const char* moduleName);
};

#endif  // FUNCTIONSLOADER_H
