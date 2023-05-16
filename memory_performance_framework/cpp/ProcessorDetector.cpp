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

Class realization for processor features detector,
detects:
CPU features by CPUID and
OS context management features by XGETBV.
Measure CPU TSC clock by RDTSC.
TODO.
1) Make more precision CPU TSC clock measurement by OS API:
   QueryPerformanceFrequency, QueryPerformanceCounter.
2) Check for bug:
   "ProcessorDetector::bytesPerInstruction[]"
   "PERF_READ_MOV_32_64... PERF_COPY_STRING_32_64"
   is 8 bytes per instruction for 32-bit build.
3) Redundancy at mapCheck(). Remove functionality for
   too old compilers, verify result both for ia32 and x64.
*/

#include "ProcessorDetector.h"

// System processor summary report data
SYSTEM_PROCESSOR_DATA ProcessorDetector::processorData;
// Values Bytes Per Instruction for native methods.
// This table associated with native DLL procedures.
// Arrays length must be constant = MAXIMUM_ASM_METHOD.
const BYTE ProcessorDetector::bytesPerInstruction[] =
{
#ifdef NATIVE_WIDTH_64
	8,  8,  8,  8,  8,  8,                              // PERF_READ_MOV_32_64    ...  PERF_COPY_STRING_32_64
	8,  8,  8,	16, 16, 16, 32, 32, 32, 64, 64, 64,     // PERF_READ_MMX_64       ...  PERF_COPY_AVX_512
	32, 64,	32, 64, 64, 64,                             // PERF_DOT_FMA_256       ...  PERF_WRITE_CLZERO
	8,  8,  8,  8,  8,  8,                              // PERF_LATENCY_BUILD_LCM ...  PERF_LATENCY_WALK_32X2
	16, 16, 16, 16, 16, 16, 16,                         // PERF_NT_READ_SSE_128   ...  PERF_NTR_COPY_SSE_128
	32, 32, 32, 32, 32, 32, 32,                         // PERF_NT_READ_AVX_256   ...  PERF_NTR_COPY_AVX_256
	64, 64, 64, 64, 64, 64, 64                          // PERF_NT_READ_AVX_512   ...  PERF_NTR_COPY_AVX_512
#endif
#ifdef NATIVE_WIDTH_32
	4,  4,  4,  4,  4,  4,                              // PERF_READ_MOV_32_64    ...  PERF_COPY_STRING_32_64
	8,  8,  8,	16, 16, 16, 32, 32, 32, 64, 64, 64,     // PERF_READ_MMX_64       ...  PERF_COPY_AVX_512
	32, 64,	32, 64, 64, 64,                             // PERF_DOT_FMA_256       ...  PERF_WRITE_CLZERO
	4,  8,  4,  8,  4,  8,                              // PERF_LATENCY_BUILD_LCM ...  PERF_LATENCY_WALK_32X2
	16, 16, 16, 16, 16, 16, 16,                         // PERF_NT_READ_SSE_128   ...  PERF_NTR_COPY_SSE_128
	32, 32, 32, 32, 32, 32, 32,                         // PERF_NT_READ_AVX_256   ...  PERF_NTR_COPY_AVX_256
	64, 64, 64, 64, 64, 64, 64                          // PERF_NT_READ_AVX_512   ...  PERF_NTR_COPY_AVX_512
#endif
};
// Data for build strings about processor features
const char* ProcessorDetector::instructionStrings[] =
{
	// default temporal methods
#ifdef NATIVE_WIDTH_64
		"read 64-bit MOV",
		"write 64-bit MOV",
		"copy 64-bit MOV",
		"modify 64-bit NOT",
		"write 64-bit REP STOSQ",
		"copy 64-bit REP MOVSQ",
#endif
#ifdef NATIVE_WIDTH_32
		"read 32-bit MOV",
		"write 32-bit MOV",
		"copy 32-bit MOV",
		"modify 32-bit NOT",
		"write 32-bit REP STOSD",
		"copy 32-bit REP MOVSD",
#endif
		"read 64-bit MMX MOVQ",
		"write 64-bit MMX MOVQ",
		"copy 64-bit MMX MOVQ",
		"read SSE128 MOVAPS",
		"write SSE128 MOVAPS",
		"copy SSE128 MOVAPS",
		"read AVX256 VMOVAPD",
		"write AVX256 VMOVAPD",
		"copy AVX256 VMOVAPD",
		"read AVX512 VMOVAPD",
		"write AVX512 VMOVAPD",
		"copy AVX512 VMOVAPD",
		"dot product FMA256 VFMADD231PD",
		"dot product FMA512 VFMADD231PD",
"gather AVX256 VGATHERQPD",
"gather AVX512 VGATHERQPD",
"scatter AVX512 VSCATTERQPD",
// default non-temporal
"optimized write CLZERO (AMD)",
// latency
"prepare walk list for latency measure by LCM",
"prepare walk list for latency measure by LCM, 32x2",
"prepare walk list for latency measure by RDRAND",
"prepare walk list for latency measure by RDRAND, 32x2",
"latency measure by walk list",
"latency measure by walk list, 32x2",
// non-temporal SSE128
"non-temporal read SSE128 MOVNTDQA",
"non-temporal prefetched read SSE128 PREFETCHNTA+MOVAPS, default distance",
"non-temporal prefetched read SSE128 PREFETCHNTA+MOVAPS, medium distance",
"non-temporal prefetched read SSE128 PREFETCHNTA+MOVAPS, long distance",
"non-temporal write SSE128 MOVNTPS",
"non-temporal (for write) copy SSE128 MOVAPS+MOVNTPS",
"non-temporal (for read and write) copy SSE128 PREFETCHNTA+MOVAPS+MOVNTPS",
// non-temporal AVX256
"non-temporal read AVX256 VMOVNTDQA",
"non-temporal prefetched read AVX256 PREFETCHNTA+VMOVAPD, default distance",
"non-temporal prefetched read AVX256 PREFETCHNTA+VMOVAPD, medium distance",
"non-temporal prefetched read AVX256 PREFETCHNTA+VMOVAPD, long distance",
"non-temporal write AVX256 VMOVNTPD",
"non-temporal (for write) copy AVX256 VMOVAPD+VMOVNTPD",
"non-temporal (for read and write) copy AVX256 PREFETCHNTA+VMOVAPD+VMOVNTPD",
// non-temporal AVX512
"non-temporal read AVX512 VMOVNTDQA",
"non-temporal prefetched read AVX512 PREFETCHNTA+VMOVAPD, default distance",
"non-temporal prefetched read AVX512 PREFETCHNTA+VMOVAPD, medium distance",
"non-temporal prefetched read AVX512 PREFETCHNTA+VMOVAPD, long distance",
"non-temporal write AVX512 VMOVNTPD",
"non-temporal (for write) copy AVX512 VMOVAPD+VMOVNTPD",
"non-temporal (for read and write) copy AVX512 PREFETCHNTA+VMOVAPD+VMOVNTPD",
// names list termination
nullptr
};
// Control structure for detect optional supported CPU features, decoder for CPUID results.
const CPUID_CONDITION ProcessorDetector::mapCpu[] =
{
	// Unconditional x86-64 base instruction set.
	{ ORMASK      , 0          , 0          , NOREG  , 0                         , CPUCONST::CPU_FEATURES_UNCONDITIONAL    } ,
	// MMX required.
	{ ORMASK      , 0x00000001 , 0x00000000 , EDX    , 1 << 23                   , CPUCONST::CPU_FEATURES_MMX64            } ,
	// SSE required.
	{ ORMASK      , 0x00000001 , 0x00000000 , EDX    , 1 << 25                   , CPUCONST::CPU_FEATURES_SSE128           } ,
	// AVX256 include context management required.
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , (1 << 27) | (1 << 28)     , CPUCONST::CPU_FEATURES_AVX256           } ,
	// AVX512 include context management required.
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1 << 16                   , CPUCONST::CPU_FEATURES_AVX512           } ,
	{ ANDMASK     , 0x00000001 , 0x00000000 , ECX    , 1 << 27                   , CPUCONST::CPU_FEATURES_AVX512           } ,
	// FMA256 include context management required.
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , (1 << 27) | (1 << 28) | (1 << 12) , CPUCONST::CPU_FEATURES_FMA256   } ,
	// FMA512 include context management required.
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1 << 16                   , CPUCONST::CPU_FEATURES_FMA512           } ,
	{ ANDMASK     , 0x00000001 , 0x00000000 , ECX    , (1 << 27) | (1 << 12)     , CPUCONST::CPU_FEATURES_FMA512           } ,
	// SSE non-temporal read by MOVNTDQA (SSE4.1).
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , 1 << 19                   , CPUCONST::CPU_FEATURES_SSE_4_1          } ,
	// AVX256 non-temporal read by VMOVNTDQA (AVX2) include context management required.
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1 << 5                    , CPUCONST::CPU_FEATURES_AVX_2            } ,
	{ ANDMASK     , 0x00000001 , 0x00000000 , ECX    , 1 << 27                   , CPUCONST::CPU_FEATURES_AVX512           } ,
	// RDRAND instruction required.
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , 1 << 30                   , CPUCONST::CPU_FEATURES_RDRAND           } ,
	// CLZERO (AMD) instruction required.
	{ ORMASKLAST  , 0x80000008 , 0x00000000 , EBX    , 1 << 0                    , CPUCONST::CPU_FEATURES_CLZERO           }
};
// Control structure for detect optional supported OS context management features, decoder for XGETBV results.
const XGETBV_CONDITION ProcessorDetector::mapOs[] = {
	{ UNCOND     , 0                                         , CPUCONST::CPU_FEATURES_UNCONDITIONAL } ,
	{ UNCOND     , 0                                         , CPUCONST::CPU_FEATURES_MMX64         } ,
	{ UNCOND     , 0                                         , CPUCONST::CPU_FEATURES_SSE128        } ,
	{ UNCOND     , 0                                         , CPUCONST::CPU_FEATURES_SSE_4_1       } ,
	{ ORMASK     , 1 << 2                                    , CPUCONST::CPU_FEATURES_AVX256        } ,
	{ ORMASK     , (1 << 2) | (1 << 5) | (1 << 6) | (1 << 7) , CPUCONST::CPU_FEATURES_AVX512        } ,
	{ ORMASK     , 1 << 2                                    , CPUCONST::CPU_FEATURES_FMA256        } ,
	{ ORMASK     , (1 << 2) | (1 << 5) | (1 << 6) | (1 << 7) , CPUCONST::CPU_FEATURES_FMA512        } ,
	{ ORMASK     , 1 << 2                                    , CPUCONST::CPU_FEATURES_AVX_2         } ,
	{ UNCOND     , 0                                         , CPUCONST::CPU_FEATURES_RDRAND        } ,
	{ UNCONDLAST , 0                                         , CPUCONST::CPU_FEATURES_CLZERO        }
};
// String results of CPUID
constexpr int VENDOR_SIZE = 12 + 1;
constexpr int MODEL_SIZE = 48 + 1;
char ProcessorDetector::cpuVendorString[VENDOR_SIZE];
char ProcessorDetector::cpuModelString[MODEL_SIZE];

// Class constructor, initialize pointers, get Processor data.
ProcessorDetector::ProcessorDetector(FUNCTIONS_LIST* pf)
{
	// Blank status string
	snprintf(statusString, APPCONST::MAX_TEXT_STRING, "No data.");
	status = FALSE;
	// global initialization and pre-blank output.
	f = pf;
	processorData.deltaTsc = 0;
	processorData.cpuBitmap = 0;
	processorData.osBitmap = 0;

	if ((f->dll_CheckCpuid)())
	{
		const char* s1 = "No data.";
		snprintf(cpuVendorString, VENDOR_SIZE, s1);
		snprintf(cpuModelString, MODEL_SIZE, s1);
		// Get processor vendor and model strings.
		getVendorAndModel(cpuVendorString, cpuModelString, VENDOR_SIZE, MODEL_SIZE);
		// Measure CPU TSC frequency by RDTSC instruction.
		// ... This fragment moved from constructor for show string "Measure TSC clock...".	
		// Detect CPU instruction set features by CPUID instruction.
		buildCpuBitmap(processorData.cpuBitmap);
		// Detect OS context management features by XGETBV instruction.
		buildOsBitmap(processorData.osBitmap);
		status = TRUE;
	}
}
// Class destructor, functionality reserved
ProcessorDetector::~ProcessorDetector()
{

}
void ProcessorDetector::writeReport()
{
	char msg[APPCONST::MAX_TEXT_STRING];
	// Processor vendor and model
	snprintf(msg, APPCONST::MAX_TEXT_STRING, "Processor vendor = %s\r\nprocessor model  = %s.\r\n",
		cpuVendorString, cpuModelString);
	// Processor frequency
	DWORD64 hz = processorData.deltaTsc;
	if (!hz)
	{
		snprintf(msg, APPCONST::MAX_TEXT_STRING, "Time stamp counter (TSC) frequency measurement error.\r\n");
	}
	else
	{
		double mhz = hz / 1000000.0;
		double ns = 1000000000.0 / hz;
		snprintf(msg, APPCONST::MAX_TEXT_STRING,
			"Time stamp counter (TSC) frequency = %.1f MHz, period = %.3f ns.\r\n",
			mhz, ns);
	}
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	// Processor and OS bitmaps as hex
	DWORD64 cpuBits = processorData.cpuBitmap;
	DWORD64 osBits = processorData.osBitmap;
	AppLib::writeColor("CPUID bitmap = ", APPCONST::TABLE_COLOR);
	AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, cpuBits, TRUE);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	AppLib::writeColor(", XGETBV bitmap = ", APPCONST::TABLE_COLOR);
	AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, osBits, TRUE);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	AppLib::writeColor(".\r\n", APPCONST::TABLE_COLOR);
	// Up string for functionality tables.
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	AppLib::writeColor(
		"  #   CPU  OS  CPU functionality and instructions\r\n",
		APPCONST::TABLE_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	// Initializing variables for functionality tables
	DWORD64 mask = 1;
	int cpuFlag = 0;
	int osFlag = 0;
	int i = 0;
	while (instructionStrings[i] != NULL)
	{
		cpuFlag = (cpuBits & mask) ? 1 : 0;
		osFlag = (osBits & mask) ? 1 : 0;
		snprintf(msg, APPCONST::MAX_TEXT_STRING, " %2d    %d   %d   %s\r\n", i, cpuFlag, osFlag, instructionStrings[i]);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		mask = mask << 1;
		i++;
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
}
// Return pointer to processor data
SYSTEM_PROCESSOR_DATA* ProcessorDetector::getProcessorList()
{
	return &processorData;
}
// Public method for check TSC (Time Stamp Counter) support and measure TSC clock
// This fragment moved from constructor for show string "Measure TSC clock..."	
BOOL ProcessorDetector::measureTSC()
{
	// Initializing variables
	DWORD rEax = 0;
	DWORD rEbx = 0;
	DWORD rEcx = 0;
	DWORD rEdx = 0;
	BOOL opStatus = FALSE;
	processorData.deltaTsc = 0;
	// Check RDTSC support
	(f->dll_ExecuteCpuid)(0, 0, &rEax, &rEbx, &rEcx, &rEdx);  // CPUID function=0, subfunction=n/a
	rEdx = 0;
	if (rEax >= 1)
	{
		(f->dll_ExecuteCpuid)(1, 0, &rEax, &rEbx, &rEcx, &rEdx);  // CPUID function=1, subfunction=n/a
	}
	if ((rEdx & 0x00000010) == 0)
	{
		snprintf(statusString, APPCONST::MAX_TEXT_STRING,
			"FAILED.\r\nRDTSC instruction not supported or locked by VMM.\r\n");
		return FALSE;
	}
	// TSC clock frequency measurement, update variables
	opStatus = (f->dll_MeasureTsc)(&processorData.deltaTsc);
	if (!opStatus)
	{
		snprintf(statusString, APPCONST::MAX_TEXT_STRING,
			"FAILED.\r\nTSC clock measurement error.\r\n");
		return FALSE;
	}
	if (processorData.deltaTsc == 0)
	{
		snprintf(statusString, APPCONST::MAX_TEXT_STRING,
			"FAILED.\r\nTSC clock zero frequency returned.\r\n");
		return FALSE;
	}
	return TRUE;
}
// Public method for detect bytes per instruction depend on benchmark routine
BYTE ProcessorDetector::getBytesPerInstruction(int i)
{
	return bytesPerInstruction[i];
}
const char* ProcessorDetector::getInstructionString(int i)
{
	return instructionStrings[i];
}
// Public methods for CPUID and XGETBV support.
// This method used for auto select method, for temporal mode (cacheable)
DWORD ProcessorDetector::findMaxMethodTemporal(DWORD64 bitmapCpu, DWORD64 bitmapOs)
{
	DWORD64 bitmap = bitmapCpu & bitmapOs;
	int select = PERF_READ_AVX_512;
	if (mapCheck(bitmap, select)) return select;
	select = PERF_READ_AVX_256;
	if (mapCheck(bitmap, select)) return select;
	select = PERF_READ_SSE_128;
	if (mapCheck(bitmap, select)) return select;
	select = PERF_READ_MMX_64;
	if (mapCheck(bitmap, select)) return select;
	select = PERF_READ_MOV_32_64;
	if (mapCheck(bitmap, select)) return select;
	return APPCONST::OPTION_NOT_SET;
}
// This method used for auto select method, for non-temporal mode (uncacheable)
DWORD ProcessorDetector::findMaxMethodNonTemporal(DWORD64 bitmapCpu, DWORD64 bitmapOs)
{
	DWORD64 bitmap = bitmapCpu & bitmapOs;
	int select = PERF_NT_READ_AVX_512;
	if (mapCheck(bitmap, select)) return select;
	select = PERF_NTP_READ_AVX_256_DEFAULT_DISTANCE;
	if (mapCheck(bitmap, select)) return select;
	select = PERF_NTP_READ_SSE_128_DEFAULT_DISTANCE;
	if (mapCheck(bitmap, select)) return select;
	select = PERF_READ_MMX_64;
	if (mapCheck(bitmap, select)) return select;
	select = PERF_READ_MOV_32_64;
	if (mapCheck(bitmap, select)) return select;
	return APPCONST::OPTION_NOT_SET;
}
// Private helpers methods.
// Get processor vendor and model strings, by CPUID instruction.
BOOL ProcessorDetector::getVendorAndModel(char* cpuVendorString, char* cpuModelString, int nV, int nM)
{
	BOOL status = FALSE;
	if ((nV < VENDOR_SIZE) || (nM < MODEL_SIZE))  // verify strings size limits for output data
	{
		return status;
	}

	DWORD eax, ebx, ecx, edx;
	status = (f->dll_CheckCpuid)();
	if (status != 0)
	{
		// Get platform parameters: get CPU strings
		(f->dll_ExecuteCpuid)(0, 0, &eax, &ebx, &ecx, &edx);
		typedef union {
			CHAR s8[52]; DWORD s32[13];
		} CPUID_STRING;
		CPUID_STRING cpuidString;
		// Vendor string
		cpuidString.s32[0] = ebx;
		cpuidString.s32[1] = edx;
		cpuidString.s32[2] = ecx;
		cpuidString.s32[3] = 0;
		CHAR* p1 = cpuVendorString;
		CHAR* p2 = cpuidString.s8;
		while (*p1++ = *p2++);
		// Model string
		strcpy(cpuidString.s8, "n/a");
		(f->dll_ExecuteCpuid)(0x80000000, 0, &eax, &ebx, &ecx, &edx);
		if (eax >= 0x80000004)
		{
			DWORD function = 0x80000002;
			int i = 0;
			for (i = 0; i < 12; i += 4)
			{
				(f->dll_ExecuteCpuid)(function, 0, &eax, &ebx, &ecx, &edx);
				cpuidString.s32[i + 0] = eax;
				cpuidString.s32[i + 1] = ebx;
				cpuidString.s32[i + 2] = ecx;
				cpuidString.s32[i + 3] = edx;
				function++;
			}
			cpuidString.s32[i] = 0;
			p1 = cpuidString.s8;
			p2 = cpuModelString;
			BOOLEAN flag = FALSE;
			while (*p1 != 0)
			{
				if ((*p1 != ' ') || (flag == TRUE))
				{
					flag = TRUE;
					*p2 = *p1;
					p2++;
				}
				p1++;
			}
			*p2 = 0;
		}
	}
	return status;
}
// Build bitmap of supported read-write methods, CPU bitmap = f( CPUID features ).
void ProcessorDetector::buildCpuBitmap(DWORD64& cpuBitmap)
{
	cpuBitmap = 0;
	int i = 0;
	BOOLEAN featureSupported = FALSE;
	do
	{
		featureSupported = getCpuidFeature(mapCpu[i].function, mapCpu[i].subfunction, mapCpu[i].reg, mapCpu[i].testBitmap);
		if ((mapCpu[i].maskType == UNCOND) || (mapCpu[i].maskType == UNCONDLAST))
		{
			cpuBitmap |= mapCpu[i].patchBitmap;
		}
		else if ((mapCpu[i].maskType == ORMASK) || (mapCpu[i].maskType == ORMASKLAST))
		{
			if (featureSupported)
			{
				cpuBitmap |= mapCpu[i].patchBitmap;
			}
		}
		else if ((mapCpu[i].maskType == ANDMASK) || (mapCpu[i].maskType == ANDMASKLAST))
		{
			if (!featureSupported)
			{
				cpuBitmap &= (~(mapCpu[i].patchBitmap));
			}
		}
		i++;
	} while ((mapCpu[i - 1].maskType != ORMASKLAST) && (mapCpu[i - 1].maskType != ANDMASKLAST) && (mapCpu[i - 1].maskType != UNCONDLAST));
}
// Helper method for detect CPUID features.
BOOL ProcessorDetector::getCpuidFeature(DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask)
{
	BOOL result = FALSE;
	DWORD eax = 0, ebx = 0, ecx = 0, edx = 0;
	DWORD temp = 0;
	(f->dll_ExecuteCpuid)(function & 0x80000000, 0, &eax, &ebx, &ecx, &edx);
	if (eax >= function)
	{
		(f->dll_ExecuteCpuid)(function, subfunction, &eax, &ebx, &ecx, &edx);
		switch (reg)
		{
		case EAX:
			temp = eax;
			break;
		case EBX:
			temp = ebx;
			break;
		case ECX:
			temp = ecx;
			break;
		case EDX:
			temp = edx;
			break;
		default:
			temp = 0;
			break;
		}
		temp &= bitmask;
		result = (temp == bitmask);
	}
	return result;
}
// Build bitmap of supported read-write methods, OS bitmap = f( OS context management features ).
void ProcessorDetector::buildOsBitmap(DWORD64& osBitmap)
{
	osBitmap = 0;
	int i = 0;
	BOOLEAN featureSupported = FALSE;
	do
	{
		featureSupported = getXgetbvFeature(mapOs[i].testBitmap);
		if ((mapOs[i].maskType == UNCOND) || (mapOs[i].maskType == UNCONDLAST))
		{
			osBitmap |= mapOs[i].patchBitmap;
		}
		else if ((mapOs[i].maskType == ORMASK) || (mapOs[i].maskType == ORMASKLAST))
		{
			if (featureSupported)
			{
				osBitmap |= mapOs[i].patchBitmap;
			}
		}
		else if ((mapOs[i].maskType == ANDMASK) || (mapOs[i].maskType == ANDMASKLAST))
		{
			if (!featureSupported)
			{
				osBitmap &= (~(mapOs[i].patchBitmap));
			}
		}
		i++;
	} while ((mapOs[i - 1].maskType != ORMASKLAST) && (mapOs[i - 1].maskType != ANDMASKLAST) && (mapOs[i - 1].maskType != UNCONDLAST));
}
// Helper method for detect OS context management features
BOOL ProcessorDetector::getXgetbvFeature(DWORD bitmask)
{
	BOOL result = FALSE;
	DWORDLONG temp = 0;
	result = getCpuidFeature(0x00000001, 0x00000000, ECX, ((DWORD)1) << 27);
	if (result)
	{
		(f->dll_ExecuteXgetbv)(&temp);
		temp &= bitmask;
		result = (temp == bitmask);
	}
	return result;
}
// Helper method for test 64-bit map, prevent overflow of 32-bit operations
BOOL ProcessorDetector::mapCheck(DWORD64 map, int index)
{
	int i1 = index;
	int i2 = 0;
	if (index > 31)
	{
		i1 = 31;
		i2 = index - 31;
	}
	map = map >> i1;
	map = map >> i2;
	return (map & 1L);
}
