#ifndef SYSTEMMEMORY_H
#define SYSTEMMEMORY_H

#include <windows.h>
#include <cstdio>
#include "GlobalDeclarations.h"

// Class declaration
class SystemMemory
{
	public:
		SystemMemory( );
		~SystemMemory( );
		DWORD detectMemory( SYSTEM_MEMORY_DATA* xm );
		char* getStatusString( );
	private:
		static MEMORYSTATUSEX memstatus;
		static char s[];
};

#endif  // SYSTEMMEMORY_H
