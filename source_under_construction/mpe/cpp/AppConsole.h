/*
     MEMORY PERFORMANCE ENGINE FRAMEWORK.
    Header for console input-output class.
*/

#ifndef APPCONSOLE_H
#define APPCONSOLE_H

#include <windows.h>
#include <share.h>    // this required for _fsopen _SH_DENYWR
#include <cstdio>
#include "maindefinitions.h"

class AppConsole
{
	public:
		static void setInputOption( int x, LPCSTR s );
		static void setOutputOption( int x, LPCSTR s  );
		static BOOL initializeOutput( );
		static void receive( LPSTR s, size_t &max );
		static void transmit( LPCSTR s );
	private:
		static const char* inputName;
		static const char* outputName;
		static int inputOption;
		static int outputOption;
//		static FILE* inputFile;
//		static FILE* outputFile;
};

#endif  // APPCONSOLE_H
