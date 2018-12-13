#ifndef SYSTEMTOPOLOGY_H
#define SYSTEMTOPOLOGY_H

#include <windows.h>
#include <cstdio>
#include "GlobalDeclarations.h"

class SystemTopology
{
	public:
		SystemTopology( );
		~SystemTopology( );
		DWORD detectTopology( SYSTEM_TOPOLOGY_DATA* xc );
		char* getStatusString( );
	private:
		static char s[];
};

#endif  // SYSTEMTOPOLOGY_H
