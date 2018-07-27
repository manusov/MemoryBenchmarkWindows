/*
 *   Read-write methods definitions.
 */

// Bitmaps generation for CPU and OS support (see associated structure at constants.c)
typedef enum {
CPU_FEATURE_READ_X64,
CPU_FEATURE_WRITE_X64,
CPU_FEATURE_COPY_X64,
CPU_FEATURE_MODIFY_X64,
CPU_FEATURE_WRITE_STRINGS_X64,
CPU_FEATURE_COPY_STRINGS_X64,
// Start optionally supported features
CPU_FEATURE_READ_SSE128,
CPU_FEATURE_WRITE_SSE128,
CPU_FEATURE_COPY_SSE128,
CPU_FEATURE_READ_AVX256,
CPU_FEATURE_WRITE_AVX256,
CPU_FEATURE_COPY_AVX256,
CPU_FEATURE_READ_AVX512,
CPU_FEATURE_WRITE_AVX512,
CPU_FEATURE_COPY_AVX512,
CPU_FEATURE_DOT_FMA256,
CPU_FEATURE_DOT_FMA512,
// Additions for non-temporal Write, 
// NTW = non-temporal write ((V)MOVNTPS)
CPU_FEATURE_NTW_WRITE_SSE128,
CPU_FEATURE_NTW_COPY_SSE128,    // this duplicated 1
CPU_FEATURE_NTW_WRITE_AVX256,
CPU_FEATURE_NTW_COPY_AVX256,    // this duplicated 2, wrong
CPU_FEATURE_NTW_WRITE_AVX512,
CPU_FEATURE_NTW_COPY_AVX512,
// Additions for non-temporal Read, 
// NTRW = non-temporal read and write ((V)MOVNTDQA)
CPU_FEATURE_NTRW_READ_SSE128,
CPU_FEATURE_NTRW_COPY_SSE128,
CPU_FEATURE_NTRW_READ_AVX256,
CPU_FEATURE_NTRW_COPY_AVX256,
CPU_FEATURE_NTRW_READ_AVX512,
CPU_FEATURE_NTRW_COPY_AVX512,
// Additions for non-temporal by Prefetch Read, 
// NTPRW = non-temporal prefetched read and write (PREFETCHNTA)
CPU_FEATURE_NTPRW_READ_SSE128,
CPU_FEATURE_NTPRW_COPY_SSE128,   // this duplicated 1
CPU_FEATURE_NTPRW_READ_AVX256,
// Reserved for extensions
// CPU_FEATURE_NTPRW_COPY_AVX256,
// CPU_FEATURE_NTPRW_READ_AVX512,
// CPU_FEATURE_NTPRW_COPY_AVX512
// Reserved for FMA with non-temporal store
} CPU_FEATURES;

// Structures for CPU optional supported features detect
typedef enum {
	ORMASK , ANDMASK , ORMASKLAST , ANDMASKLAST, UNCOND, UNCONDLAST
} MASK_TYPE;

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
#define CPU_FEATURES_UNCONDITIONAL                 \
((DWORDLONG)1) << CPU_FEATURE_READ_X64 |           \
((DWORDLONG)1) << CPU_FEATURE_WRITE_X64 |          \
((DWORDLONG)1) << CPU_FEATURE_COPY_X64 |           \
((DWORDLONG)1) << CPU_FEATURE_MODIFY_X64 |         \
((DWORDLONG)1) << CPU_FEATURE_WRITE_STRINGS_X64 |  \
((DWORDLONG)1) << CPU_FEATURE_COPY_STRINGS_X64

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

DWORDLONG buildCpuidBitmap( CPUID_CONDITION x[] );
DWORDLONG buildXgetbvBitmap( XGETBV_CONDITION x[] );

void detectMethods( DWORD *select, DWORDLONG *bitmap, DWORDLONG *bitmapCpu, DWORDLONG *bitmapOs );
void printMethods( DWORD select, DWORDLONG bitmap, DWORDLONG bitmapCpu, DWORDLONG bitmapOs, CHAR *methodsNames[] );

