#ifndef SYSTEMLIBRARY_H
#define SYSTEMLIBRARY_H

#include <windows.h>
#include <cstdio>
#include "GlobalDeclarations.h"

// Class declaration
class SystemLibrary
{
	public:
	    SystemLibrary( );
        ~SystemLibrary( );
        DWORD loadSystemLibrary( );
        SYSTEM_FUNCTIONS_LIST* getSystemFunctionsList( );
        char* getStatusString( );
	private:
	    static const char* dllName;
        static const char* fname1;
        static const char* fname2;
        static const char* fname3;
        static const char* fname4;
        static const char* fname5;
        static const char* fname6;
        static const char* fname7;
        static HMODULE dllHandle;
        static SYSTEM_FUNCTIONS_LIST f;
        static char s[];
        // Helpers functions
        BOOL functionCheck( void *functionPointer, const char *functionName, const char *dllName, DWORD &errorCode );
};

#endif  // SYSTEMLIBRARY_H


