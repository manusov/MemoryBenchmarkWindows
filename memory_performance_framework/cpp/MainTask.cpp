/*

MEMORY PERFORMANCE ENGINE (MPE) FRAMEWORK.
-------------------------------------------
System information and performance analysing application,
include console version of NCRB (NUMA CPU and RAM Benchmark).
See also memory benchmark projects:
https://github.com/manusov/NCRBv2
https://github.com/manusov/MemoryBenchmarkWindows
Experimental samples:
https://github.com/manusov/Prototyping
Special thanks for C++ lessons:
https://ravesli.com/uroki-cpp/

NCRB tasks selector:
benchmark, system information, user help.
TODO.

*/

#include "Constants.h"
#include "Definitions.h"
#include "ControlSet.h"
#include "AppLib.h"
#include "CommandLine.h"
#include "MemoryDetector.h"
#include "PagingDetector.h"
#include "SysInfo.h"
#include "Benchmark.h"
#include "UserHelp.h"

int maintask(int argc, char** argv)
{
	// int status = 0;
	// AppLib::write("\r\nNCRB CONSOLE integration test.\r\n");

	char msg[APPCONST::MAX_TEXT_STRING];

	// Title and service strings include copyright
	const char* stringTitle1 = APPCONST::APPLICATION_STRING;
	const char* stringTitle2 = APPCONST::BUILD_STRING;
	const char* stringTitle3 = APPCONST::COPYRIGHT_STRING;
	const char* stringAnyKey = APPCONST::ANY_KEY_STRING;
	// Pointers to system classes objects, unconditionally created.
	SYSTEM_CONTROL_SET s;
	// Pointer to service classes objects.
	CommandLine* pCommandLine = nullptr;
	// Pointers to classes objects, conditionally created,
	// by selected scenario.
	UserHelp* pUserHelp = nullptr;
	SysInfo* pSysinfoScenario = nullptr;
	Benchmark* pMemoryScenario = nullptr;
	// Pointers to global visible structures.
	FUNCTIONS_LIST* pFunctionsList = nullptr;
	// Text report control.
	char* pExecDir = nullptr;
	char* pTextBuf = nullptr;
	char* pTextWrite = nullptr;

	// Control and status variables.
	COMMAND_LINE_PARMS* pp = nullptr;   // Pointer to structure for numeric values of extracted command line parameters.
	BOOL opStatus;                      // Temporary variable for operations status.
	DWORD osErrorCode;                  // Temporary variable for detail information about error.
	LPCSTR statusString;                // Pointer to temporary string with detail error description.

	// Allocate memory for path to executable file directory
	size_t pathSize = sizeof(char) * APPCONST::PATH_SIZE;
	pExecDir = (char*)malloc(pathSize);
	if (!pExecDir)
	{
		// Yet simple printf, because buffer not allocated.
		printf("\r\nError at memory allocation for executable path.\r\n");
		return 1;
	}

	// Build executable path for create output report file
	opStatus = AppLib::storeExecReportPath(pExecDir, pathSize, APPCONST::OUTPUT_FILE_NAME);
	if (!opStatus)
	{
		// Yet simple printf, because buffer not allocated
		printf("\r\nError at build executable path.\r\n");
		return 1;
	}

	// Allocate memory for text buffer
	// int mText = sizeof(char) * APPCONST::TEXT_SIZE;
	//pTextBuf = (char*)malloc(mText + 1);
	//if (!pTextBuf)
	//{
	//	// Yet simple printf, because buffer not allocated
	//	printf("\r\nError at memory allocation for text report buffer.\r\n");
	//	return 1;
	//}

	// Set input scenario file path (reserved) and output report mode (screen)
	// AppConsole::setInputOption(IN_DEFAULT, APPCONST::INPUT_FILE_NAME);
	// AppConsole::setOutputOption(OUT_SCREEN, pExecDir);

	// Initialize console output and report mode, yet ON, ON.
	// Review this for screen mode = f(options settings). Change flags.
	// AppLib::initConsole(TRUE, TRUE, pTextBuf, mText);

	// Show title strings
	snprintf(msg, APPCONST::MAX_TEXT_STRING, "\r\n%s %s %s\r\n", stringTitle1, stringTitle2, stringTitle3);
	AppLib::write(msg);

	// Get command line parameters
	AppLib::write("Get command line parameters...");
	pCommandLine = new CommandLine();
	pCommandLine->resetBeforeParse();
	osErrorCode = pCommandLine->parseCommandLine(argc, argv);
	if (osErrorCode)
	{   // Report error if command line parse failed
		AppLib::write("FAILED\r\n");
		statusString = pCommandLine->getStatusString();
		snprintf(msg, APPCONST::MAX_TEXT_STRING, "\r\nERROR: %s\r\n\r\n", statusString);
		AppLib::write(msg);
		if (osErrorCode > 0)
		{
			AppLib::storeSystemErrorName(msg, APPCONST::MAX_TEXT_STRING, osErrorCode);
			AppLib::write(msg);
			return 1;
		}
	}
	else
	{   // Verify and assign parameters if command line parse successful
		AppLib::write("done.\r\n");
		pCommandLine->correctAfterParse();
		pp = pCommandLine->getCommandLineParms();

		// Support special file redirection mode
		//AppConsole::setOutputOption(pp->optionOut, pExecDir);
		//if (pp->optionOut == OUT_FILE)
		//{
		//	AppConsole::initializeOutput();
		//	// copyright string to file
		//	snprintf(pTextAlloc, mText, "%s %s %s\n", stringTitle1, stringTitle2, stringTitle3);
		//	AppConsole::transmit(pTextAlloc);
		//}
		// AppLib::initConsole(TRUE, (pp->optionOut == OUT_FILE), pTextAlloc, mText);  // Review this for screen mode = f(options settings). Change flags.

		// Blank system classes pointers
		s.pFunctionsLoader = nullptr;
		s.pProcessorDetector = nullptr;
		s.pTopologyDetector = nullptr;
		s.pMemoryDetector = nullptr;
		s.pPagingDetector = nullptr;
		s.pDomainsBuilder = nullptr;
		s.pThreadsBuilder = nullptr;

		// Start create system classes, load native DLL and functions
		AppLib::write("Load API...");
		s.pFunctionsLoader = new FncLoader();
		pFunctionsList = s.pFunctionsLoader->getFunctionsList();

		// Check native DLL load
		if (pFunctionsList->loadStatus == 0)
		{   // Continue initialization if native DLL load successful
			AppLib::write("done.\r\nDetect processor features...");
			s.pProcessorDetector = new ProcessorDetector(pFunctionsList);
			AppLib::write("done.\r\nDetect SMP and cache levels...");
			s.pTopologyDetector = new TopologyDetector(pFunctionsList);
			AppLib::write("done.\r\nDetect memory...");
			s.pMemoryDetector = new MemoryDetector(pFunctionsList);
			AppLib::write("done.\r\nDetect paging...");
			s.pPagingDetector = new PagingDetector(pFunctionsList);
			AppLib::write("done.\r\nInitializing domains builder...");
			s.pDomainsBuilder = new DomainsBuilder(pFunctionsList);
			AppLib::write("done.\r\nInitializing threads builder...");
			s.pThreadsBuilder = new ThreadsBuilder(pFunctionsList);
			AppLib::write("done.\r\nMeasure TSC clock...");
			s.pProcessorDetector->measureTSC();

			// Get application native library info, show string
			AppLib::write("done.\r\nGet library data...");
			const char* dllProduct, * dllVersion, * dllVendor;
			pFunctionsList->dll_GetDllStrings(&dllProduct, &dllVersion, &dllVendor);
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "done.\r\n%s %s %s\r\n", dllProduct, dllVersion, dllVendor);
			AppLib::write(msg);

			// Select scenario
			int opHelp = pp->optionHelp;
			int opInfo = pp->optionInfo;
			int opTest = pp->optionTest;

			// Run user help scenario if selected by command line
			if (opHelp != APPCONST::OPTION_NOT_SET)
			{
				AppLib::write("Run user help scenario.\r\n");
				const OPTION_ENTRY* oplist = pCommandLine->getOptionsList();
				pUserHelp = new UserHelp(opHelp, oplist);
				pUserHelp->execute();
			}

			// Run system information scenario if selected by command line
			if (opInfo != APPCONST::OPTION_NOT_SET)
			{
				AppLib::write("Run system information scenario.\r\n");
				pSysinfoScenario = new SysInfo(opInfo, &s);
				pSysinfoScenario->execute();
			}

			// Run benchmark scenario if selected by command line
			if (opTest != APPCONST::OPTION_NOT_SET)
			{
				// Benchmark scenario: memory
				if (opTest == TEST_MEMORY)
				{
					AppLib::write("Run memory benchmark scenario.\r\n\r\n");
					pMemoryScenario = new Benchmark(opTest, &s, pp);
					pMemoryScenario->execute();
				}
			}

			// Default scenario, show run help recommendation if scenario not selected
			if ((opHelp == APPCONST::OPTION_NOT_SET) && (opInfo == APPCONST::OPTION_NOT_SET) && (opTest == APPCONST::OPTION_NOT_SET))
			{
				AppLib::write("Run default scenario.\n");
				AppLib::writeColor("\r\nNO PARAMETERS, USE \"help=full\".\r\n", APPCONST::VALUE_COLOR);
			}
		}
		else
		{   // Error message if native DLL load failed, operation skipped at this point
			AppLib::write("\r\nError loading DLL.\r\n");
		}
	}

	// Scenario done, print message, release (delete) objects and exit
	AppLib::write("\r\nDelete headblock global objects...");

	// Delete scenario modules
	if (pUserHelp)        delete pUserHelp;
	if (pSysinfoScenario) delete pSysinfoScenario;
	if (pMemoryScenario)  delete pMemoryScenario;

	// Delete system support classes
	if (s.pThreadsBuilder)    delete s.pThreadsBuilder;
	if (s.pDomainsBuilder)    delete s.pDomainsBuilder;
	if (s.pPagingDetector)    delete s.pPagingDetector;
	if (s.pMemoryDetector)    delete s.pMemoryDetector;
	if (s.pTopologyDetector)  delete s.pTopologyDetector;
	if (s.pProcessorDetector) delete s.pProcessorDetector;
	if (s.pFunctionsLoader)   delete s.pFunctionsLoader;
	if (pCommandLine)         delete pCommandLine;

	// Exit application with "done" message
	AppLib::write("done.\r\n");

	// Release text buffer memory
	// if (!pTextBuf) free(pTextBuf);

	// Release executable file path, it used for transmit report, exit application
	if (pExecDir != NULL) free(pExecDir);
	return 0;

	// return status;
}
