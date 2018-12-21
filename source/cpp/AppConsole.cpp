#include "AppConsole.h"

const char* AppConsole::inputName = INPUT_FILE_NAME;
const char* AppConsole::outputName = OUTPUT_FILE_NAME;
int AppConsole::inputOption;
int AppConsole::outputOption;
FILE* AppConsole::inputFile;
FILE* AppConsole::outputFile;

void AppConsole::setInputOption( int x )
{
	inputOption = x;
}

void AppConsole::setOutputOption( int x )
{
	outputOption = x;
}

BOOL AppConsole::initializeOutput( )
{
	outputFile = fopen( outputName, "w" );
	return ( outputFile != NULL );
}

void AppConsole::transmit( LPCSTR s )
{
	if ( outputOption == OUT_SCREEN )
	{
		printf( s );
	}
	else if ( outputOption == OUT_FILE )
	{
		FILE* f = outputFile = fopen( outputName, "a" );
		fprintf( f, s );
		fclose( f );
	}
	else
	{
		printf( "\r\nOUTPUT OPTION ERROR.\r\n" );
	}
}

void AppConsole::receive( LPSTR s, size_t &max )
{
	// Reserved	
}

