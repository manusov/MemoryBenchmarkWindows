/*
           MEMORY PERFORMANCE ENGINE FRAMEWORK.
    Header for CPU and OS paging features detector class.
*/

#ifndef PAGINGDETECTOR_H
#define PAGINGDETECTOR_H

#include <windows.h>
#include <cstdio>
#include "maindefinitions.h"
#include "AppLib.h"

class PagingDetector
{
	public:
		PagingDetector( FUNCTIONS_LIST* functions );
		~PagingDetector( );
		SYSTEM_PAGING_DATA* getPagingList( );
		void getPagingText( LPSTR &dst, size_t &max );
        char* getStatusString( );
	private:
		static FUNCTIONS_LIST* f;
		static SYSTEM_PAGING_DATA pagingData;
		static char s[];
};

#endif  // PAGINGDETECTOR_H
