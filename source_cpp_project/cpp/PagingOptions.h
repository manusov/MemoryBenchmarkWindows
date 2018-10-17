#ifndef PAGINGOPTIONS_H
#define PAGINGOPTIONS_H

#include <windows.h>
#include <cstdio>
#include "GlobalDeclarations.h"

// Class declaration
class PagingOptions
{
	public:
		PagingOptions( );
	    ~PagingOptions( );
	    BOOL detectPaging( PAGING_OPTIONS_DATA* xp );
	    char* getStatusString( );
	private:
	    static SYSTEM_INFO sysinfo;
	    static const char* glpmLibrary;
	    static const char* glpmFunction;
	    static char s[];
};

#endif  // PAGINGOPTIONS_H

