/*
    MEMORY PERFORMANCE ENGINE FRAMEWORK.
     Header for Threads Builder class.
*/

#ifndef THREADSBUILDER_H
#define THREADSBUILDER_H

#include <windows.h>
#include <cstdio>
#include "maindefinitions.h"
#include "AppLib.h"

class ThreadsBuilder
{
	public:
		ThreadsBuilder( FUNCTIONS_LIST* functions );
		~ThreadsBuilder( );
		SYSTEM_THREADS_DATA* getThreadsList( );
		void getThreadsText( LPSTR &dst, size_t &max );
		char* getStatusString( );
		// Threads management functions
		BOOL buildThreadsList( INPUT_CONSTANTS* pInputC, INPUT_VARIABLES* pInputV, SYSTEM_NUMA_DATA* pDomain );
		BOOL updateThreadsList( INPUT_VARIABLES* pInputV );
		BOOL releaseThreadsList( );
		BOOL runThreads( OUTPUT_VARIABLES* pOutputV );
		BOOL restartThreads( OUTPUT_VARIABLES* pOutputV );
	private:
		static FUNCTIONS_LIST* f;
		static SYSTEM_THREADS_DATA threadsData;
		static const char* tableUpThreads;
		static char s[];
		// Helpers methods
		void blankThread( THREAD_CONTROL_ENTRY* pT );
		void blankAffinity( GROUP_AFFINITY* pA );
};

#endif  // THREADSBUILDER_H

