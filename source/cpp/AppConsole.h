/*
     MEMORY PERFORMANCE ENGINE FRAMEWORK.
    Header for console input-output class.
*/

#ifndef APPCONSOLE_H
#define APPCONSOLE_H

#include <windows.h>
#include <cstdio>
#include "maindefinitions.h"

class AppConsole
{
	public:
		static void setInputOption( int x );
		static void setOutputOption( int x );
		static BOOL initializeOutput( );
		static void receive( LPSTR s, size_t &max );
		static void transmit( LPCSTR s );
	private:
		static const char* inputName;
		static const char* outputName;
		static int inputOption;
		static int outputOption;
		static FILE* inputFile;
		static FILE* outputFile;
};

#endif  // APPCONSOLE_H
