#ifndef PERFORMER_H
#define PERFORMER_H

#include <windows.h>
#include <cstdio>
#include "GlobalDeclarations.h"

// Class declaration
class Performer
{
	public:
		Performer( SYSTEM_FUNCTIONS_LIST* pFunctions, NUMA_CONTROL_SET* nFunctions );
		~Performer( );
		DWORD buildThreadsList( BENCHMARK_SCENARIO* pScenario );
		DWORD releaseThreadsList( BENCHMARK_SCENARIO* pScenario );
		DWORD threadsRun( BENCHMARK_SCENARIO* pScenario, DWORD64 &deltaTsc );
		DWORD threadsRestart( BENCHMARK_SCENARIO* pScenario, DWORD64 &deltaTsc );
		void threadsUpdate( BENCHMARK_SCENARIO* pScenario, DWORD64 x );
		void repeatsUpdate( BENCHMARK_SCENARIO* pScenario, DWORD64 x );
		void routineUpdate( BENCHMARK_SCENARIO* pScenario, DWORD64 x );
		BOOL freeThreadsList( BENCHMARK_SCENARIO* pScenario );
		char* getStatusString( );
	private:
		static SYSTEM_FUNCTIONS_LIST* pF;
		static NUMA_CONTROL_SET* pN;
		static char s[];
		// Helpers methods
		DWORD64 alignByFactor( DWORD64 value, DWORD64 factor );
};

#endif  // PERFORMER_H
