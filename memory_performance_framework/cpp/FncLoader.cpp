/*

MEMORY PERFORMANCE ENGINE (MPE) FRAMEWORK.
-------------------------------------------
System informationand performance analysing application,
include console version of NCRB(NUMA CPUand RAM Benchmark).
See also memory benchmark projects :
https://github.com/manusov/NCRBv2
https://github.com/manusov/MemoryBenchmarkWindows
Experimental samples :
https://github.com/manusov/Prototyping
Special thanks for C++ lessons :
https://ravesli.com/uroki-cpp/

Class realization for functions loader:
supports OS DLL and ASM DLL functions dynamical import.
TODO.

*/

#include "FncLoader.h"

const LIBRARY_ENTRY FncLoader::libDirectory[]
{
	{ "KERNEL32.DLL"     , "KERNEL32.DLL"          , fncNamesKernel32 },
	{ "ADVAPI32.DLL"     , "ADVAPI32.DLL"          , fncNamesAdvapi32 },
	{ APPCONST::DLL_NAME , APPCONST::DLL_PATH_NAME , fncNamesAsm      },
	{ nullptr            , nullptr                 , nullptr          }
};
const LPCSTR FncLoader::fncNamesKernel32[]
{
	"QueryPerformanceFrequency",
	"QueryPerformanceCounter",
	"GetLogicalProcessorInformation",
	"GetLogicalProcessorInformationEx",
	"GlobalMemoryStatusEx",
	"GetActiveProcessorCount",
	"GetActiveProcessorGroupCount",
	"GetNumaHighestNodeNumber",
	"GetNumaNodeProcessorMask",
	"GetNumaNodeProcessorMaskEx",
	"VirtualAllocExNuma",
	"SetThreadAffinityMask",
	"SetThreadGroupAffinity",
	"GetLargePageMinimum",
	"GetSystemFirmwareTable",
	nullptr
};
const LPCSTR FncLoader::fncNamesAdvapi32[]
{
	"OpenProcessToken",
	"LookupPrivilegeValueA",  // Use ASCII version of WinAPI function.
	"AdjustTokenPrivileges",
	nullptr
};
const LPCSTR FncLoader::fncNamesAsm[]
{
	"GetDllStrings",
	"CheckCpuid",
	"ExecuteCpuid",
	"ExecuteRdtsc",
	"ExecuteXgetbv",
	"MeasureTsc",
	"MeasureTscByFileTime",
	"MeasureTscByPcounter",
	"PerformanceGate",
	nullptr
};

FncLoader::FncLoader()
{
	status = false;
	f = new FUNCTIONS_LIST;
	API_UNI_PTR* pFncPtrs = reinterpret_cast<API_UNI_PTR*>(f);
	if (pFncPtrs)
	{
		f->loadStatus = 0;
		snprintf(statusString, APPCONST::MAX_TEXT_STRING, "No errors.");
		const LIBRARY_ENTRY* pDir = libDirectory;
		while (pDir->libPathName)
		{
			HMODULE hDll = LoadLibrary(pDir->libPathName);
			if (!hDll)
			{
				f->loadStatus = 1;
				snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Error loading library: %s.", pDir->libPathName);
			}
			const LPCSTR* pFncNames = pDir->fncName;
			while (*pFncNames)
			{
				if (hDll)
				{
					void* p = GetProcAddress(hDll, *pFncNames);
					*pFncPtrs = reinterpret_cast<API_UNI_PTR>(p);
				}
				else
				{
					*pFncPtrs = nullptr;
				}
				pFncPtrs++;
				pFncNames++;
			}
			pDir++;
		}
		if (!f->loadStatus) status = true;
	}
}
FncLoader::~FncLoader()
{
	if (f)
	{
		delete f;
		f = nullptr;
	}
}
void FncLoader::writeReport()
{
	const char* tableUp =
		" Module           Function                            Pointer (hex)\r\n";

	char msg[APPCONST::MAX_TEXT_STRING];
	char s[APPCONST::MAX_TEXT_STRING];
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	AppLib::writeColor(tableUp, APPCONST::TABLE_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	API_UNI_PTR* pFncPtrs = reinterpret_cast<API_UNI_PTR*>(f);
	if (pFncPtrs)
	{
		const LIBRARY_ENTRY* pDir = libDirectory;
		while (pDir->libName)
		{
			const LPCSTR* pFncNames = pDir->fncName;
			while (*pFncNames)
			{
				AppLib::storeHexPointer(s, APPCONST::MAX_TEXT_STRING, *pFncPtrs, false);
				snprintf(msg, APPCONST::MAX_TEXT_STRING, " %-17s%-36s%s\r\n", pDir->libName, *pFncNames, s);
				WORD color = (*pFncPtrs) ? APPCONST::VALUE_COLOR : APPCONST::ERROR_COLOR;
				AppLib::writeColor(msg, color);
				pFncPtrs++;
				pFncNames++;
			}
			pDir++;
		}
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
}
