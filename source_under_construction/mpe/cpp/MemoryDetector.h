/*
         MEMORY PERFORMANCE ENGINE FRAMEWORK.
    Header for Memory Information Windows API class.
*/

#ifndef MEMORYDETECTOR_H
#define MEMORYDETECTOR_H

#include <windows.h>
#include <cstdio>
#include "maindefinitions.h"
#include "AppLib.h"

class MemoryDetector
{
	public:
		MemoryDetector( FUNCTIONS_LIST* functions );
		~MemoryDetector( );
		SYSTEM_MEMORY_DATA* getMemoryList( );
		void getMemoryText( LPSTR &dst, size_t &max );
        char* getStatusString( );
	private:
		static FUNCTIONS_LIST* f;
		static SYSTEM_MEMORY_DATA memoryData;
		static char s[];
};

#endif  // MEMORYDETECTOR_H
