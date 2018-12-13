// THIS MODULE IS RESERVED FOR FUTURE USE

#ifndef USERHELP_H
#define USERHELP_H

#include <windows.h>
#include <cstdio>
#include "maindefinitions.h"
#include "maincontrolset.h"
#include "AppLib.h"

class UserHelp
{
	public:
		UserHelp( char* pointer, size_t limit, int op, const OPTION_ENTRY* oplist );
		~UserHelp( );
		void execute( );
	private:
		static char* saveDst;
		static size_t saveMax;
		static int enumOp;
		static const OPTION_ENTRY* optionsList;
		static const char* exampleString;
};

#endif  // USERHELP_H
