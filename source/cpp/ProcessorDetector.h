#ifndef PROCESSORDETECTOR_H
#define PROCESSORDETECTOR_H

#include <windows.h>
#include <cstdio>
#include "maindefinitions.h"
#include "AppLib.h"

// Local declarations for Processor Detector class
typedef enum {
	ORMASK , ANDMASK , ORMASKLAST , ANDMASKLAST, UNCOND, UNCONDLAST
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
	DWORDLONG patchBitmap;
} CPUID_CONDITION;

typedef struct {
	MASK_TYPE maskType;
	DWORD testBitmap;
	DWORDLONG patchBitmap;
} XGETBV_CONDITION;

// This features unconditionally supported by x64 CPU
#define CPU_FEATURES_UNCONDITIONAL                      \
((DWORDLONG)1) << CPU_FEATURE_READ_IA32_X64 |           \
((DWORDLONG)1) << CPU_FEATURE_WRITE_IA32_X64 |          \
((DWORDLONG)1) << CPU_FEATURE_COPY_IA32_X64 |           \
((DWORDLONG)1) << CPU_FEATURE_MODIFY_IA32_X64 |         \
((DWORDLONG)1) << CPU_FEATURE_WRITE_STRINGS_IA32_X64 |  \
((DWORDLONG)1) << CPU_FEATURE_COPY_STRINGS_IA32_X64 |   \
((DWORDLONG)1) << CPU_FEATURE_LATENCY_LCM |             \
((DWORDLONG)1) << CPU_FEATURE_LATENCY_WALK

// This features validated by RDRAND feature bit
// CPUID function 00000001h register ECX bit 30
#define CPU_FEATURES_RDRAND                            \
((DWORDLONG)1) << CPU_FEATURE_LATENCY_RDRAND

// start of added at v0.60.03
// This features validated by MMX feature bit:
// CPUID function 00000001h register EDX bit 23
#define CPU_FEATURES_MMX64                         \
((DWORDLONG)1) << CPU_FEATURE_READ_MMX64 |         \
((DWORDLONG)1) << CPU_FEATURE_WRITE_MMX64 |        \
((DWORDLONG)1) << CPU_FEATURE_COPY_MMX64
// end of added at v0.60.03

// This features validated by SSE feature bit:
// CPUID function 00000001h register EDX bit 25
#define CPU_FEATURES_SSE128                        \
((DWORDLONG)1) << CPU_FEATURE_READ_SSE128 |        \
((DWORDLONG)1) << CPU_FEATURE_WRITE_SSE128 |       \
((DWORDLONG)1) << CPU_FEATURE_COPY_SSE128 |        \
((DWORDLONG)1) << CPU_FEATURE_NTW_WRITE_SSE128 |   \
((DWORDLONG)1) << CPU_FEATURE_NTW_COPY_SSE128 |    \
((DWORDLONG)1) << CPU_FEATURE_NTW_WRITE_SSE128 |   \
((DWORDLONG)1) << CPU_FEATURE_NTW_COPY_SSE128 |    \
((DWORDLONG)1) << CPU_FEATURE_NTPRW_READ_SSE128 |  \
((DWORDLONG)1) << CPU_FEATURE_NTPRW_COPY_SSE128

// This features validated by AVX256 feature bit:
// CPUID function 00000001h register ECX bit 28 = AVX,
// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
// conjunction with bit XCR0.2 = AVX state
#define CPU_FEATURES_AVX256                        \
((DWORDLONG)1) << CPU_FEATURE_READ_AVX256 |        \
((DWORDLONG)1) << CPU_FEATURE_WRITE_AVX256 |       \
((DWORDLONG)1) << CPU_FEATURE_COPY_AVX256 |        \
((DWORDLONG)1) << CPU_FEATURE_NTW_WRITE_AVX256 |   \
((DWORDLONG)1) << CPU_FEATURE_NTW_COPY_AVX256 |    \
((DWORDLONG)1) << CPU_FEATURE_NTPRW_READ_AVX256
// ((DWORDLONG)1) << CPU_FEATURE_NTPRW_COPY_AVX256

// This features validated by AVX512F feature bit:
// CPUID function 7 subfunction 0 register EBX bit 16 = AVX512F,
// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
// conjunction with bit XCR0. [2,5,6,7]
#define CPU_FEATURES_AVX512                        \
((DWORDLONG)1) << CPU_FEATURE_READ_AVX512 |        \
((DWORDLONG)1) << CPU_FEATURE_WRITE_AVX512 |       \
((DWORDLONG)1) << CPU_FEATURE_COPY_AVX512 |        \
((DWORDLONG)1) << CPU_FEATURE_NTW_WRITE_AVX512 |   \
((DWORDLONG)1) << CPU_FEATURE_NTW_COPY_AVX512 |    \
((DWORDLONG)1) << CPU_FEATURE_NTRW_READ_AVX512 |   \
((DWORDLONG)1) << CPU_FEATURE_NTRW_COPY_AVX512
// ((DWORDLONG)1) << CPU_FEATURE_NTPRW_READ_AVX512 |  \
// ((DWORDLONG)1) << CPU_FEATURE_NTPRW_COPY_AVX512 |  \

// This features validated by AVX256 and FMA features bits conjunction:
// CPUID function 00000001h register ECX bit 28 = AVX,
// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
// CPUID function 00000001h register ECX bit 12 = FMA,
// conjunction with bit XCR0.2 = AVX state
#define CPU_FEATURES_FMA256                        \
((DWORDLONG)1) << CPU_FEATURE_DOT_FMA256

// This features validated by AVX512 and FMA features bits conjunction:
// CPUID function 7 subfunction 0 register EBX bit 16 = AVX512,
// CPUID function 00000001h register ECX bit 12 = FMA,
// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
// conjunction with bit XCR0. [2,5,6,7]
#define CPU_FEATURES_FMA512                        \
((DWORDLONG)1) << CPU_FEATURE_DOT_FMA512

// This features validated by SSE4.1 feature bit:
// CPUID function 00000001h register ECX bit 19 = SSE4.1
#define CPU_FEATURES_MOVNTDQA128                   \
((DWORDLONG)1) << CPU_FEATURE_NTRW_READ_SSE128 |   \
((DWORDLONG)1) << CPU_FEATURE_NTRW_COPY_SSE128

// This features validated by AVX2 feature bit:
// CPUID function 7 subfunction 0 register EBX bit 5 = AVX2,
// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
// conjunction with bit XCR0.2 = AVX state
#define CPU_FEATURES_MOVNTDQA256                   \
((DWORDLONG)1) << CPU_FEATURE_NTRW_READ_AVX256 |   \
((DWORDLONG)1) << CPU_FEATURE_NTRW_COPY_AVX256

class ProcessorDetector
{
	public:
		ProcessorDetector( FUNCTIONS_LIST* functions );
		~ProcessorDetector( );
		SYSTEM_PROCESSOR_DATA* getProcessorList( );
		void getProcessorText( LPSTR &dst, size_t &max );
		DWORD findMaxMethodTemporal( DWORD64 bitmapCpu, DWORD64 bitmapOs );
		DWORD findMaxMethodNonTemporal( DWORD64 bitmapCpu, DWORD64 bitmapOs );
		BOOL measureTSC( );
		BYTE getBytesPerInstruction( int i );
		const char* getInstructionString( int i );
        char* getStatusString( );
	private:
		static const BYTE bytesPerInstruction[];
		static const char* instructionStrings[];
		static const char* tableUpFunctionality;
		static char cpuVendorString[];
		static char cpuModelString[];
		static FUNCTIONS_LIST* f;
		static SYSTEM_PROCESSOR_DATA processorData;
		static char s[];
		// Control tables for CPUID decoding
		static const CPUID_CONDITION x[];
	    static const XGETBV_CONDITION y[];
		// Helper methods for CPUID and XGETBV
		BOOL getVendorAndModel( char* cpuVendorString, char* cpuModelString, int nV, int nM );
		void buildCpuBitmap( DWORD64 &cpuBitmap );
		void buildOsBitmap( DWORD64 &osBitmap );
		BOOL getCpuidFeature( DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask );
		BOOL getXgetbvFeature( DWORD bitmask );
		// Helper methods for 64-bit mask checks
		BOOL mapCheck( DWORD64 map, int index );
};

#endif  // PROCESSORDETECTOR_H

