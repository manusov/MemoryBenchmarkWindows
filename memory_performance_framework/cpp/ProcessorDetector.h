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

Class header for processor features detector,
detects:
CPU features by CPUID and
OS context management features by XGETBV.
Measure CPU TSC clock by RDTSC.
TODO.
1) Make more precision CPU TSC clock measurement by OS API:
   QueryPerformanceFrequency, QueryPerformanceCounter.

*/

#pragma once
#ifndef PROCESSORDETECTOR_H
#define PROCESSORDETECTOR_H

#include <windows.h>
#include "Constants.h"
#include "Definitions.h"
#include "UDM.h"
#include "AppLib.h"

// Local declarations for Processor Detector class
typedef enum {
	ORMASK, ANDMASK, ORMASKLAST, ANDMASKLAST, UNCOND, UNCONDLAST
} MASK_TYPE;
typedef enum {
	EAX, EBX, ECX, EDX, NOREG
} CPUID_OUTPUT;
typedef struct {
	MASK_TYPE maskType;
	DWORD function;
	DWORD subfunction;
	CPUID_OUTPUT reg;
	DWORD testBitmap;
	DWORD64 patchBitmap;
} CPUID_CONDITION;
typedef struct {
	MASK_TYPE maskType;
	DWORD testBitmap;
	DWORD64 patchBitmap;
} XGETBV_CONDITION;

namespace CPUCONST
{
	// This features unconditionally supported by x64 CPU.
	constexpr DWORD64 CPU_FEATURES_UNCONDITIONAL =
		((DWORD64)1) << PERF_READ_MOV_32_64 |
		((DWORD64)1) << PERF_WRITE_MOV_32_64 |
		((DWORD64)1) << PERF_COPY_MOV_32_64 |
		((DWORD64)1) << PERF_MODIFY_NOT_32_64 |
		((DWORD64)1) << PERF_WRITE_STRING_32_64 |
		((DWORD64)1) << PERF_COPY_STRING_32_64 |
		((DWORD64)1) << PERF_LATENCY_BUILD_LCM |
		((DWORD64)1) << PERF_LATENCY_BUILD_LCM_32X2 |
		((DWORD64)1) << PERF_LATENCY_WALK |
		((DWORD64)1) << PERF_LATENCY_WALK_32X2;
	// This features validated by RDRAND feature bit:
	// CPUID function 00000001h register ECX bit 30.
	constexpr DWORD64 CPU_FEATURES_RDRAND =
		((DWORD64)1) << PERF_LATENCY_BUILD_RDRAND |
		((DWORD64)1) << PERF_LATENCY_BUILD_RDRAND_32X2;
	// This features validated by MMX feature bit:
	// CPUID function 00000001h register EDX bit 23.
	constexpr DWORD64 CPU_FEATURES_MMX64 =
		((DWORD64)1) << PERF_READ_MMX_64 |
		((DWORD64)1) << PERF_WRITE_MMX_64 |
		((DWORD64)1) << PERF_COPY_MMX_64;
	// This features validated by SSE feature bit:
	// CPUID function 00000001h register EDX bit 25.
	constexpr DWORD64 CPU_FEATURES_SSE128 =
		((DWORD64)1) << PERF_READ_SSE_128 |
		((DWORD64)1) << PERF_WRITE_SSE_128 |
		((DWORD64)1) << PERF_COPY_SSE_128 |
		((DWORD64)1) << PERF_NTP_READ_SSE_128_DEFAULT_DISTANCE |
		((DWORD64)1) << PERF_NTP_READ_SSE_128_MEDIUM_DISTANCE |
		((DWORD64)1) << PERF_NTP_READ_SSE_128_LONG_DISTANCE |
		((DWORD64)1) << PERF_NT_WRITE_SSE_128 |
		((DWORD64)1) << PERF_NT_COPY_SSE_128;
	// This features validated by AVX256 feature bit:
	// CPUID function 00000001h register ECX bit 28 = AVX,
	// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE,
	// conjunction with bit XCR0.2 = AVX state.
	constexpr DWORD64 CPU_FEATURES_AVX256 =
		((DWORD64)1) << PERF_READ_AVX_256 |
		((DWORD64)1) << PERF_WRITE_AVX_256 |
		((DWORD64)1) << PERF_COPY_AVX_256 |
		((DWORD64)1) << PERF_NTP_READ_AVX_256_DEFAULT_DISTANCE |
		((DWORD64)1) << PERF_NTP_READ_AVX_256_MEDIUM_DISTANCE |
		((DWORD64)1) << PERF_NTP_READ_AVX_256_LONG_DISTANCE |
		((DWORD64)1) << PERF_NT_WRITE_AVX_256 |
		((DWORD64)1) << PERF_NT_COPY_AVX_256;
	// This features validated by AVX512F feature bit:
	// CPUID function 7 subfunction 0 register EBX bit 16 = AVX512F,
	// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE,
	// conjunction with bit XCR0. [2,5,6,7].
	constexpr DWORD64 CPU_FEATURES_AVX512 =
		((DWORD64)1) << PERF_READ_AVX_512 |
		((DWORD64)1) << PERF_WRITE_AVX_512 |
		((DWORD64)1) << PERF_COPY_AVX_512 |
		((DWORD64)1) << PERF_NT_READ_AVX_512 |
		((DWORD64)1) << PERF_NTP_READ_AVX_512_DEFAULT_DISTANCE |
		((DWORD64)1) << PERF_NTP_READ_AVX_512_MEDIUM_DISTANCE |
		((DWORD64)1) << PERF_NTP_READ_AVX_512_LONG_DISTANCE |
		((DWORD64)1) << PERF_NT_WRITE_AVX_512 |
		((DWORD64)1) << PERF_NT_COPY_AVX_512 |
		((DWORD64)1) << PERF_NTR_COPY_AVX_512 |
		((DWORD64)1) << PERF_GATHER_AVX_512 |
		((DWORD64)1) << PERF_SCATTER_AVX_512;
	// This features validated by AVX256 and FMA features bits conjunction:
	// CPUID function 00000001h register ECX bit 28 = AVX,
	// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
	// CPUID function 00000001h register ECX bit 12 = FMA,
	// conjunction with bit XCR0.2 = AVX state.
	constexpr DWORD64 CPU_FEATURES_FMA256 =
		((DWORD64)1) << PERF_DOT_FMA_256;
	// This features validated by AVX512 and FMA features bits conjunction:
	// CPUID function 7 subfunction 0 register EBX bit 16 = AVX512,
	// CPUID function 00000001h register ECX bit 12 = FMA,
	// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE,
	// conjunction with bit XCR0. [2,5,6,7].
	constexpr DWORD64 CPU_FEATURES_FMA512 =
		((DWORD64)1) << PERF_DOT_FMA_512;
	// This features validated by SSE4.1 feature bit:
	// CPUID function 00000001h register ECX bit 19 = SSE4.1.
	constexpr DWORD64 CPU_FEATURES_SSE_4_1 =
		((DWORD64)1) << PERF_NT_READ_SSE_128 |
		((DWORD64)1) << PERF_NTR_COPY_SSE_128;
	// This features validated by AVX2 feature bit:
	// CPUID function 00000007h subfunction 0 register EBX bit 5 = AVX2,
	// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE,
	// conjunction with bit XCR0.2 = AVX state.
	constexpr DWORD64 CPU_FEATURES_AVX_2 =
		((DWORD64)1) << PERF_NT_READ_AVX_256 |
		((DWORD64)1) << PERF_NTR_COPY_AVX_256 |
		((DWORD64)1) << PERF_GATHER_AVX_256;
	// This features validated by AMD CLZERO feature bit:
	// CPUID function 80000008h register EBX bit 0 = CLZERO,
	constexpr DWORD64 CPU_FEATURES_CLZERO =
		((DWORD64)1) << PERF_WRITE_CLZERO;
}

class ProcessorDetector : public UDM
{
public:
	ProcessorDetector(FUNCTIONS_LIST* pf);
	~ProcessorDetector();
	void writeReport();
	// Processor detector specific methods.
	SYSTEM_PROCESSOR_DATA* getProcessorList();                            // Get pointer to structure: dtsc, cpubitmap, osbitmap.
	DWORD findMaxMethodTemporal(DWORD64 bitmapCpu, DWORD64 bitmapOs);     // Get maximum temporal bandwidth method available for platform.
	DWORD findMaxMethodNonTemporal(DWORD64 bitmapCpu, DWORD64 bitmapOs);  // Get maximum non-temporal bandwidth method available for platform.
	BOOL measureTSC();                        // Measure TSC frequency, can get value by other functions.
	BYTE getBytesPerInstruction(int i);       // Get bytes per instruction for given method.
	const char* getInstructionString(int i);  // Get pointer to instruction name string for given method.
private:
	// Helper methods for CPUID and XGETBV.
	BOOL getVendorAndModel(char* cpuVendorString, char* cpuModelString, int nV, int nM);
	void buildCpuBitmap(DWORD64& cpuBitmap);
	void buildOsBitmap(DWORD64& osBitmap);
	BOOL getCpuidFeature(DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask);
	BOOL getXgetbvFeature(DWORD bitmask);
	// Helper methods for 64-bit mask checks.
	BOOL mapCheck(DWORD64 map, int index);
	// Static data, constants.
	static const BYTE bytesPerInstruction[];
	static const char* instructionStrings[];
	// Static data, variables.
	static char cpuVendorString[];
	static char cpuModelString[];
	static SYSTEM_PROCESSOR_DATA processorData;
	// Control tables for CPUID decoding.
	static const CPUID_CONDITION mapCpu[];
	static const XGETBV_CONDITION mapOs[];
};

#endif  //  PROCESSORDETECTOR_H
