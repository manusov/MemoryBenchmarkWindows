#ifndef DECODERCPUID_H
#define DECODERCPUID_H

#include <windows.h>
#include <cstdio>
#include "GlobalDeclarations.h"

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
((DWORDLONG)1) << CPU_FEATURE_COPY_STRINGS_IA32_X64

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

// Class declaration
class DecoderCpuid
{
	public:
		DecoderCpuid( SYSTEM_FUNCTIONS_LIST* pFunctions );
		~DecoderCpuid( );
		BOOL getVendorAndModel( char* cpuVendorString, char* cpuModelString, int NV, int NM );
		void buildCpuBitmap( DWORD64 &cpuBitmap );
		void buldOsBitmap( DWORD64 &osBitmap );
		DWORD findMaxMethodTemporal( DWORD64 bitmapCpu, DWORD64 bitmapOs );
		DWORD findMaxMethodNonTemporal( DWORD64 bitmapCpu, DWORD64 bitmapOs );
		char* getStatusString( );
	private:
		static SYSTEM_FUNCTIONS_LIST* pF;
		static const CPUID_CONDITION x[];
	    static const XGETBV_CONDITION y[];
		static char s[];
		// Helper methods
		BOOL getCpuidFeature( DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask );
		BOOL getXgetbvFeature( DWORD bitmask );
};

#endif  // DECODERCPUID_H


