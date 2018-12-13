/*
     MEMORY PERFORMANCE ENGINE FRAMEWORK DEBUG MODULE
*/

/*

TODO:
1)  + Make minimal user help.
2)  Test builds ia32 and x64, verify at some options set.
3)  64-bit verify all options.
4)  32-bit verify all options.
5)  Bug return from memory test scenario when objects not released.
6)  Variables (some of): blockMax, blockSize, blockCount, blockDelta make 64-bit, prevent overflows. Plus other width regularity.
7)  Special visualization for latency.
8)  Some format changes, reduce number of strings, ( data ) after done, example ...done( dTSC= ... ).
9)  Detalize WinAPI errors codes.
10) Detailze status string error codes, use "s" at classes.
11) Return some log, rejected when go from v0.5x to v0.6x.
12) File report modes.
13) MMX modes.
14) Find optimal method for blank structures and arrays.
15) %I64d and 64-bit printf arguments can crush printf in the 32-bit mode.

*/

#include <cstdio>
#include <windows.h>
#include "maindefinitions.h"
#include "maincontrolset.h"
#include "AppLib.h"
#include "CommandLine.h"
#include "SysinfoScenario.h"
#include "MemoryScenario.h"
#include "StorageScenario.h"
#include "CommandLine.h"
#include "UserHelp.h"

// Title and service strings include copyright
const char* stringTitle1 = APPLICATION_STRING;
const char* stringTitle2 = BUILD_STRING;
const char* stringTitle3 = COPYRIGHT_STRING;
const char* stringAnyKey = ANY_KEY_STRING;
// Pointers to system classes objects, unconditionally created
SYSTEM_CONTROL_SET s;
// Pointer to service classes objects
CommandLine*       pCommandLine = NULL;
// Pointers to classes objects, conditionally created,
// by selected scenario
UserHelp*          pUserHelp = NULL;
SysinfoScenario*   pSysinfoScenario = NULL;
MemoryScenario*    pMemoryScenario = NULL;
StorageScenario*   pStorageScenario = NULL;
// Pointers to global visible structures
FUNCTIONS_LIST* pFunctionsList = NULL;
// Text report control
char* pTextAlloc = NULL;
char* pTextWrite = NULL;

// Application entry point
int main(int argc, char** argv) 
{

/*
	DEBUG
*/

/*
	BOOL status = FALSE;
	// Allocate memory for text buffer
	int mText = sizeof( char ) * TEXT_SIZE;
	pTextAlloc = ( char* )malloc( mText+1 );
	if ( pTextAlloc == NULL )
	{
		printf( "\nError at memory allocation for text report buffer.\n" );
		return 1;
	}
	// Create classes
	pFunctionsLoader = new FunctionsLoader( );
	pFunctionsList = pFunctionsLoader->getFunctionsList( );
	pProcessorDetector = new ProcessorDetector( pFunctionsList );
	pTopologyDetector = new TopologyDetector( pFunctionsList );
	pMemoryDetector = new MemoryDetector( pFunctionsList );
	pPagingDetector = new PagingDetector( pFunctionsList );
	pDomainsBuilder = new DomainsBuilder( pFunctionsList );
	pThreadsBuilder = new ThreadsBuilder( pFunctionsList );
	pCommandLine = new CommandLine( );
*/

/*	
	// Initializing get text report
	pTextWrite = pTextAlloc;
	size_t max = TEXT_SIZE;
	// Special step for measure CPU TSC clock
	printf( "\nMeasure TSC clock..." );
	status = pProcessorDetector->measureTSC( );
	if ( status )
	{
		printf( "OK.\n\n" );
	}
	else
	{
		printf( pProcessorDetector->getStatusString( ) );
	}
	// Report about loaded functions: WinAPI and this application DLL
	pFunctionsLoader->getFunctionsText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	// Report about processor features and frequency
	pProcessorDetector->getProcessorText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	// Report about system topology, standard version
	pTopologyDetector->getTopologyText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	// Report about system topology, extended version
	pTopologyDetector->getTopologyTextEx( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	// Report about system memory
	pMemoryDetector->getMemoryText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	// Report about paging options
	pPagingDetector->getPagingText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	// Report about NUMA domains
	pDomainsBuilder->getNumaText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	// Report about execution threads
	pThreadsBuilder->getThreadsText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	// Done reports
	*pTextWrite = 0;  // Terminator must be char = 0
	pTextWrite++;
	// Show text report
	printf( "\n[    DEBUG STRINGS: SYSTEM INFORMATION    ]\n\n%s\n", pTextAlloc );
*/

/*	
	#define TEST_BLOCK 1024*1024
	pTextWrite = pTextAlloc;
	max = TEXT_SIZE;
	pDomainsBuilder->allocateNodesList( TEST_BLOCK, 0, 4096, FALSE );
	pDomainsBuilder->getNumaText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	pDomainsBuilder->freeNodesList();
	pDomainsBuilder->getNumaText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	*pTextWrite = 0;  // Terminator must be char = 0
	pTextWrite++;
	printf( "\n[   DEBUG STRINGS: NUMA-AWARE MEMORY ALLOCATION   ]\n\n%s\n", pTextAlloc );
*/

/*
	#define TEST_BLOCK 1024*1024
	pTextWrite = pTextAlloc;
	max = TEXT_SIZE;
	pDomainsBuilder->allocateSimpleList( TEST_BLOCK, 0, 4096 );
	pDomainsBuilder->getSimpleText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	pDomainsBuilder->freeSimpleList();
	pDomainsBuilder->getSimpleText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	*pTextWrite = 0;  // Terminator must be char = 0
	pTextWrite++;
	printf( "\n[   DEBUG STRINGS: NUMA-UNAWARE MEMORY ALLOCATION   ]\n\n%s\n", pTextAlloc );
*/

/*
	#define TEST_THREADS 4
	#define TEST_BLOCK_PER_THREAD  1024*1024
	#define TEST_BLOCK_ALL_THREADS TEST_BLOCK_PER_THREAD * TEST_THREADS
	#define TEST_SIZE_INSTRUCTIONS 4096
	#define TEST_REPEATS 100000
	#define TEST_LARGE_PAGE 0
	#define TEST_METHOD_ID 0
	
	INPUT_CONSTANTS ic;
	INPUT_VARIABLES iv;
	OUTPUT_VARIABLES ov;
	ic.threadCount = TEST_THREADS;
	ic.maxSizeBytes = TEST_BLOCK_PER_THREAD;
	iv.currentSizeInstructions = TEST_SIZE_INSTRUCTIONS;
	iv.currentMeasurementRepeats = TEST_REPEATS;
	ic.pagingMode = TEST_LARGE_PAGE;
	iv.currentMethodId = TEST_METHOD_ID;
	ic.htMode = HT_NOT_USED;
	
	pTextWrite = pTextAlloc;
	max = TEXT_SIZE;
	
	pDomainsBuilder->allocateSimpleList( TEST_BLOCK_ALL_THREADS, 0, 4096 );
	pDomainsBuilder->getSimpleText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	SYSTEM_NUMA_DATA* pn = pDomainsBuilder->getSimpleList();
	
	pThreadsBuilder->buildThreadsList( &ic, &iv, pn );
	pThreadsBuilder->getThreadsText( pTextWrite, max );
	AppLib::printCrLf( pTextWrite, max );
	*pTextWrite = 0;  // Terminator must be char = 0
	pTextWrite++;
	printf( "\n[   DEBUG STRINGS: NODES AND THREADS ALLOCATION   ]\n\n%s\n", pTextAlloc );
	
	printf( "start thread..." );
	pThreadsBuilder->runThreads( &ov );
	printf( " done.\n" );
	printf( "re-start thread..." );
	pThreadsBuilder->restartThreads( &ov );
	printf( " done.\n\n" );
*/

/*
	// Release objects and exit
	delete pCommandLine;
	delete pThreadsBuilder;
	delete pDomainsBuilder;
	delete pPagingDetector;
	delete pMemoryDetector;
	delete pTopologyDetector;
	delete pProcessorDetector;
	delete pFunctionsLoader;
	free( pTextAlloc );
	return 0;
*/

/*
	END OF DEBUG
*/

	// Show title
	printf( "\r\n%s %s %s\r\n", stringTitle1, stringTitle2, stringTitle3 );
	// Control and status variables
	COMMAND_LINE_PARMS* pp = NULL;
	BOOL opStatus;
	DWORD osErrorCode;
	LPCSTR statusString;
	// Allocate memory for text buffer
	int mText = sizeof( char ) * TEXT_SIZE;
	pTextAlloc = ( char* )malloc( mText+1 );
	if ( pTextAlloc == NULL )
	{
		printf( "\nError at memory allocation for text report buffer.\n" );
		return 1;
	}
	// Blank system classes pointers
	s.pFunctionsLoader = NULL;
	s.pProcessorDetector = NULL;
	s.pTopologyDetector = NULL;
	s.pMemoryDetector = NULL;
	s.pPagingDetector = NULL;
	s.pDomainsBuilder = NULL;
	s.pThreadsBuilder = NULL;
	// Create classes
	printf( "load API..." );
	s.pFunctionsLoader = new FunctionsLoader( );
	pFunctionsList = s.pFunctionsLoader->getFunctionsList( );
	printf( "done\r\ndetect processor features..." );
	s.pProcessorDetector = new ProcessorDetector( pFunctionsList );
	printf( "done\r\ndetect SMP and cache levels..." );
	s.pTopologyDetector = new TopologyDetector( pFunctionsList );
	printf( "done\r\ndetect memory..." );
	s.pMemoryDetector = new MemoryDetector( pFunctionsList );
	printf( "done\r\ndetect paging..." );
	s.pPagingDetector = new PagingDetector( pFunctionsList );
	printf( "done\r\ninitializing domains builder..." );
	s.pDomainsBuilder = new DomainsBuilder( pFunctionsList );
	printf( "done\r\ninitializing threads builder..." );
	s.pThreadsBuilder = new ThreadsBuilder( pFunctionsList );
	printf( "done\r\nmeasure TSC clock..." );
	s.pProcessorDetector->measureTSC( );
	// Get application native library info, show string
	printf( "done\r\nget library data..." );
	char *dllProduct, *dllVersion, *dllVendor;
	pFunctionsList->DLL_GetDllStrings( &dllProduct, &dllVersion, &dllVendor );
	printf( "done\r\n%s %s %s\n", dllProduct, dllVersion, dllVendor );
	// Get command line parameters
	printf( "get command line parameters..." );
	pCommandLine = new CommandLine( );
	pCommandLine->resetBeforeParse( );
	osErrorCode = pCommandLine->parseCommandLine( argc, argv );
	if ( osErrorCode )
	{
		printf( "FAILED\r\n" );
		statusString = pCommandLine->getStatusString( );
        printf( "\r\nERROR: %s\r\n\r\n", statusString );
        if ( osErrorCode > 0 )
        {
        	AppLib::printSystemError( osErrorCode );
    	}
	}
	else
	{
		printf( "done\r\n" );
		pCommandLine->correctAfterParse( );
		pp = pCommandLine->getCommandLineParms( );
		// Select scenario
		int opHelp = pp->optionHelp;
		int opInfo = pp->optionInfo;
		int opTest = pp->optionTest;
		// User help scenario
		if ( opHelp != OPTION_NOT_SET )
		{
			printf( "run user help scenario.\r\n" );
			// printf( "\r\nTHIS BRANCH IS UNDER CONSTRUCTION.\r\n\r\n" );
			const OPTION_ENTRY* oplist = pCommandLine->getOptionsList( );
			pUserHelp = new UserHelp( pTextAlloc, mText, opHelp, oplist );
			pUserHelp->execute( );
		}
		// System information scenario
		if ( opInfo != OPTION_NOT_SET )
		{
			printf( "run system information scenario.\r\n" );
			pSysinfoScenario = new SysinfoScenario( pTextAlloc, mText, opInfo, &s );
			pSysinfoScenario->execute( );
		}
		// Benchmark scenario
		if ( opTest != OPTION_NOT_SET )
		{
			// Benchmark scenario: memory
			if ( opTest == TEST_MEMORY )
			{
				printf( "run memory benchmark scenario.\r\n" );
				pMemoryScenario = new MemoryScenario( pTextAlloc, mText, opTest, &s, pp );
				pMemoryScenario->execute( );
			}
			// Benchmark scenario: mass storage
			if ( opTest == TEST_STORAGE )
			{
				printf( "run mass storage benchmark scenario.\r\n" );
				printf( "\r\nTHIS BRANCH IS UNDER CONSTRUCTION.\r\n\r\n" );
			}
		}
		// Default scenario
		if ( ( opHelp == OPTION_NOT_SET )&&( opInfo == OPTION_NOT_SET )&&( opTest == OPTION_NOT_SET ) )
		{
			printf( "run default scenario.\r\n" );
			// printf( "\r\nTHIS BRANCH IS UNDER CONSTRUCTION.\r\n\r\n" );
			printf( "\r\nNO PARAMETERS, USE \"help=full\".\r\n\r\n" );
		}
	}
	// Scenario done, print message, release (delete) objects and exit
	printf( "delete global objects..." );
	// Release conditionally created objects
	if ( pUserHelp != NULL )        delete pUserHelp;
	if ( pSysinfoScenario != NULL ) delete pSysinfoScenario;
	if ( pMemoryScenario != NULL )  delete pMemoryScenario;
	if ( pStorageScenario != NULL ) delete pStorageScenario;
	// Release unconditionally created objects
	delete pCommandLine;
	delete s.pThreadsBuilder;
	delete s.pDomainsBuilder;
	delete s.pPagingDetector;
	delete s.pMemoryDetector;
	delete s.pTopologyDetector;
	delete s.pProcessorDetector;
	delete s.pFunctionsLoader;
	free( pTextAlloc );
	// Exit application with "done" message
	printf( "done\r\n\r\n" );
	return 0;
}

