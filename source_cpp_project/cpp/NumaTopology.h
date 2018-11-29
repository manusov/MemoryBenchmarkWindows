#ifndef NUMATOPOLOGY_H
#define NUMATOPOLOGY_H

#include <windows.h>
#include <cstdio>
#include "GlobalDeclarations.h"

// Class declaration
class NumaTopology
{
	public:
		NumaTopology( );
		~NumaTopology( );
		void loadControlSet( );
		NUMA_CONTROL_SET* getControlSet( );
		int buildNodesList( NUMA_NODE_ENTRY* xp );
		BOOL allocateNodesList( size_t xs, int n, NUMA_NODE_ENTRY* xp );
		BOOL freeNodesList( int n, NUMA_NODE_ENTRY* xp );
		BOOL allocateThreadsList( int nx, int ny, NUMA_NODE_ENTRY* xp, THREAD_CONTROL_ENTRY* yp );
		char* getStatusString( );
	private:
		static const char* ncsLibrary;
		static NUMA_CONTROL_SET ncs;
		static char s[];
};

#endif  // NUMATOPOLOGY_H
