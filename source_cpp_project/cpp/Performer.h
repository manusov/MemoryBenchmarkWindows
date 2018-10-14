#ifndef PERFORMER_H
#define PERFORMER_H

#include <windows.h>
#include <cstdio>
#include "GlobalDeclarations.h"

// Class declaration
class Performer
{
	public:
		Performer( SYSTEM_FUNCTIONS_LIST* pFunctions );
		~Performer( );
		BOOL buildThreadsList( BENCHMARK_SCENARIO* pScenario );
		BOOL releaseThreadsList( BENCHMARK_SCENARIO* pScenario );
		BOOL threadsRun( BENCHMARK_SCENARIO* pScenario, DWORD64 &deltaTsc );
		BOOL threadsRestart( BENCHMARK_SCENARIO* pScenario, DWORD64 &deltaTsc );
		void threadsUpdate( BENCHMARK_SCENARIO* pScenario, DWORD64 x );
		void repeatsUpdate( BENCHMARK_SCENARIO* pScenario, DWORD64 x );
		char* getStatusString( );
	private:
		static SYSTEM_FUNCTIONS_LIST* pF;
		static char s[];
};

#endif  // PERFORMER_H
