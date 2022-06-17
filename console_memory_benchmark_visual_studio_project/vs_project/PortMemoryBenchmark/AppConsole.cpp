/*
	 MEMORY PERFORMANCE ENGINE FRAMEWORK.
		 Console input-output class.
*/

#include "AppConsole.h"

const char* AppConsole::inputName = INPUT_FILE_NAME;
const char* AppConsole::outputName = OUTPUT_FILE_NAME;
int AppConsole::inputOption;
int AppConsole::outputOption;
// FILE* AppConsole::inputFile;
// FILE* AppConsole::outputFile;

void AppConsole::setInputOption(int x, LPCSTR s)
{
	inputOption = x;
	inputName = s;
}

void AppConsole::setOutputOption(int x, LPCSTR s)
{
	outputOption = x;
	outputName = s;
}

BOOL AppConsole::initializeOutput()
{
	// outputFile = fopen( outputName, "w" );
	FILE* f = _fsopen(outputName, "w", _SH_DENYWR);
	// FILE* f = _fsopen( outputName, "w", _SH_DENYNO );
	// FILE* f = fopen( outputName, "w" );
	//
	// note Java MPE shell must delete this file after read it, this action must be ENABLED.
	if (f != NULL) fclose(f);
	// file can be shared with Java or Python GUI application
	return (f != NULL);
}

void AppConsole::transmit(LPCSTR s)
{
	if (outputOption == OUT_SCREEN)
	{
		printf(s);
	}
	else if (outputOption == OUT_FILE)
	{
		// FILE* f = fopen( outputName, "a" );
		FILE* f = _fsopen(outputName, "a", _SH_DENYWR);
		// file can be shared with Java or Python GUI application
		fprintf(f, s);
		fclose(f);
	}
	else
	{
		printf("\nOUTPUT OPTION ERROR.\n");
	}
}

void AppConsole::receive(LPSTR s, size_t& max)
{
	// Reserved	
}
