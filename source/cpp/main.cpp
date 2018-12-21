/*

     MEMORY PERFORMANCE ENGINE FRAMEWORK DEBUG MODULE.
     
*/

/*

TODO:
1)  + Make minimal user help.
2)  + Test builds ia32 and x64, verify at some options set.
3)  + Inspect printf/snprintf usage. 64-bit printf arguments can crush printf in the 32-bit mode. Not use %I64D.
4)  + Some format changes, reduce number of strings, ( data ) after done, example ...done( dTSC= ... ).
5)  + Special visualization for latency.
--- v0.60.03 ---
6) + Bug with 32-bit copy, address *8, required *4.
7) + Crush: 32-bit mode: writemmx64.
8) + Crush: 32-bit mode: latencylcm, latencyrdrand.
9) + Add MMX modes. This can be highest speed for 32-bit CPU, example Athlon/Duron for socket A (Socket 462).
	  + backup v0.60.02 to "C:\2-cpp\06-mpe-v06x-OLD", update this version number to v0.60.03
	  + add MMX methods to ia32 library, reorder methods list is INCOMPATIBLE modification, this application must be updated
	  + add MMX methods to x64 library, reorder methods list is INCOMPATIBLE modification, this application must be updated.
      + maindefinitions.h ( +42, +69 )
      + commandline.cpp ( +38 )
      + processordetector.h ( +49 )
      + processordetector.cpp ( +18, +28, +54, +95, +123, +320, +364 ).
10) + Asm method id must be decoded as routine name, otherwise user view unknown method, numeric id only.
11) + Remove "GUI" option from DLLs. Recompile for 32/64.
12) + Bug with select max temporal and max non temporal methods, shifts and comparing 32 instead 64.

13) File report modes.
	 - base functionality.
     - refactor saveDst vs txp operations, sometimes can remove txp.
		use saveDst, saveMax for AppConsole::transmit, REMOVE txp and MAX_TRANSIT.
	 - report copyright when out=file.
	 - number of spaces when exit on different scenarios.

14) Bug return from memory test scenario when objects not released, if errors detected, by error handling branch.
15) Variables (some of): blockMax, blockSize, blockCount, blockDelta make 64-bit, prevent overflows. Plus other width regularity.
16) Detalize WinAPI errors codes. Function printSystemError, replace printf to snprintf at this routine.
17) Detailze status string error codes, use "s" at classes.
18) Return some log, rejected when go from v0.5x to v0.6x. Status string "s", plus OS error code. See 2 previous items.
19) Find optimal method for blank structures and arrays.
20) Sequencing: user help must be without hardware initialization, get command line must be before hardware initialization.
21) Bug if ht=off when HT not supported, ThreadsBuilder.cpp, line 260. Or correct before call buildThreadsList().

22) 64-bit verify all options. Include AVX512, NUMA, Processor Groups. Required platform.
23) 32-bit verify all options. Include AVX512, NUMA, Processor Groups. Required platform.


*/

#include <cstdio>
#include <windows.h>
#include "maindefinitions.h"
#include "maincontrolset.h"
#include "AppLib.h"
#include "AppConsole.h"
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

	// Allocate memory for text buffer
	int mText = sizeof( char ) * TEXT_SIZE;
	pTextAlloc = ( char* )malloc( mText+1 );
	if ( pTextAlloc == NULL )
	{
		// Yet simple printf, because buffer not allocated
		printf( "\r\nError at memory allocation for text report buffer.\r\n" );
		return 1;
	}
	// Show title
	AppConsole::setOutputOption( OUT_SCREEN );
	snprintf( pTextAlloc, mText, "\r\n%s %s %s\r\n", stringTitle1, stringTitle2, stringTitle3 );
	AppConsole::transmit( pTextAlloc );
	
	// Control and status variables
	COMMAND_LINE_PARMS* pp = NULL;
	BOOL opStatus;
	DWORD osErrorCode;
	LPCSTR statusString;
	
	// Get command line parameters
	AppConsole::transmit( "get command line parameters..." );
	pCommandLine = new CommandLine( );
	pCommandLine->resetBeforeParse( );
	osErrorCode = pCommandLine->parseCommandLine( argc, argv );
	if ( osErrorCode )
	{
		AppConsole::transmit( "FAILED\r\n" );
		statusString = pCommandLine->getStatusString( );
        snprintf( pTextAlloc, mText, "\r\nERROR: %s\r\n\r\n", statusString );
        AppConsole::transmit( pTextAlloc );
        if ( osErrorCode > 0 )
        {
        	AppLib::printSystemError( osErrorCode );
        	return 1;
    	}
	}
	else
	{
		AppConsole::transmit( "done\r\n" );
		pCommandLine->correctAfterParse( );
		pp = pCommandLine->getCommandLineParms( );
		
		// Support special file redirection mode
		AppConsole::setOutputOption( pp->optionOut );
		if ( pp->optionOut == OUT_FILE )
		{
			AppConsole::initializeOutput( );
			AppConsole::transmit( pTextAlloc );  // copyright string to file
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
		AppConsole::transmit( "load API..." );
		s.pFunctionsLoader = new FunctionsLoader( );
		pFunctionsList = s.pFunctionsLoader->getFunctionsList( );
		AppConsole::transmit( "done\r\ndetect processor features..." );
		s.pProcessorDetector = new ProcessorDetector( pFunctionsList );
		AppConsole::transmit( "done\r\ndetect SMP and cache levels..." );
		s.pTopologyDetector = new TopologyDetector( pFunctionsList );
		AppConsole::transmit( "done\r\ndetect memory..." );
		s.pMemoryDetector = new MemoryDetector( pFunctionsList );
		AppConsole::transmit( "done\r\ndetect paging..." );
		s.pPagingDetector = new PagingDetector( pFunctionsList );
		AppConsole::transmit( "done\r\ninitializing domains builder..." );
		s.pDomainsBuilder = new DomainsBuilder( pFunctionsList );
		AppConsole::transmit( "done\r\ninitializing threads builder..." );
		s.pThreadsBuilder = new ThreadsBuilder( pFunctionsList );
		AppConsole::transmit( "done\r\nmeasure TSC clock..." );
		s.pProcessorDetector->measureTSC( );
		// Get application native library info, show string
		AppConsole::transmit( "done\r\nget library data..." );
		char *dllProduct, *dllVersion, *dllVendor;
		pFunctionsList->DLL_GetDllStrings( &dllProduct, &dllVersion, &dllVendor );
		snprintf( pTextAlloc, mText, "done\r\n%s %s %s\n", dllProduct, dllVersion, dllVendor );
		AppConsole::transmit( pTextAlloc );

		// Select scenario
		int opHelp = pp->optionHelp;
		int opInfo = pp->optionInfo;
		int opTest = pp->optionTest;
		// User help scenario
		if ( opHelp != OPTION_NOT_SET )
		{
			AppConsole::transmit( "run user help scenario.\r\n" );
			const OPTION_ENTRY* oplist = pCommandLine->getOptionsList( );
			pUserHelp = new UserHelp( pTextAlloc, mText, opHelp, oplist );
			pUserHelp->execute( );
		}
		// System information scenario
		if ( opInfo != OPTION_NOT_SET )
		{
			AppConsole::transmit( "run system information scenario.\r\n" );
			pSysinfoScenario = new SysinfoScenario( pTextAlloc, mText, opInfo, &s );
			pSysinfoScenario->execute( );
		}
		// Benchmark scenario
		if ( opTest != OPTION_NOT_SET )
		{
			// Benchmark scenario: memory
			if ( opTest == TEST_MEMORY )
			{
				AppConsole::transmit( "run memory benchmark scenario.\r\n" );
				pMemoryScenario = new MemoryScenario( pTextAlloc, mText, opTest, &s, pp );
				pMemoryScenario->execute( );
			}
			// Benchmark scenario: mass storage
			if ( opTest == TEST_STORAGE )
			{
				AppConsole::transmit( "run mass storage benchmark scenario.\r\n" );
				AppConsole::transmit( "\r\nTHIS BRANCH IS UNDER CONSTRUCTION.\r\n\r\n" );
			}
		}
		// Default scenario
		if ( ( opHelp == OPTION_NOT_SET )&&( opInfo == OPTION_NOT_SET )&&( opTest == OPTION_NOT_SET ) )
		{
			AppConsole::transmit( "run default scenario.\r\n" );
			AppConsole::transmit( "\r\nNO PARAMETERS, USE \"help=full\".\r\n\r\n" );
		}
	}
	// Scenario done, print message, release (delete) objects and exit
	AppConsole::transmit( "delete global objects..." );
	// Delete scenario modules
	if ( pUserHelp        != NULL ) delete pUserHelp;
	if ( pSysinfoScenario != NULL ) delete pSysinfoScenario;
	if ( pMemoryScenario  != NULL ) delete pMemoryScenario;
	if ( pStorageScenario != NULL ) delete pStorageScenario;
	// Delete classes
	if ( s.pThreadsBuilder    != NULL ) delete s.pThreadsBuilder;
	if ( s.pDomainsBuilder    != NULL ) delete s.pDomainsBuilder;
	if ( s.pPagingDetector    != NULL ) delete s.pPagingDetector;
	if ( s.pMemoryDetector    != NULL ) delete s.pMemoryDetector;
	if ( s.pTopologyDetector  != NULL ) delete s.pTopologyDetector;
	if ( s.pProcessorDetector != NULL ) delete s.pProcessorDetector;
	if ( s.pFunctionsLoader   != NULL ) delete s.pFunctionsLoader;
	if ( pCommandLine         != NULL ) delete pCommandLine;
	// Release text buffer memory
	if ( pTextAlloc != NULL ) free( pTextAlloc );
	// Exit application with "done" message
	AppConsole::transmit( "done\r\n\r\n" );
	return 0;
}

