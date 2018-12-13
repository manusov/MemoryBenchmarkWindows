/*
 *   Header for Memory Performance Engine command line options support.
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

// Enumeration of target tested objects, argument for select block size and number of threads
typedef enum {
	L1_CACHE, L2_CACHE, L3_CACHE, L4_CACHE, DRAM, USER_DEFINED_TARGET
} RW_TARGET;

// Enumeration of speculative (cacheable) memory access modes
// temporal means cacheable, non-temporal means non cacheable
typedef enum {
	TEMPORAL, NON_TEMPORAL, DEFAULT_CACHING
} RW_ACCESS;

// Enumeration of hyper-threading support options
typedef enum {
	HTOFF, HTON
} HYPER_THREADING;

// Enumeration of paging options
typedef enum {
	MIN_PAGES, LARGE_PAGES
} PAGE_SIZE;

// Enumeration of NUMA topology support options
typedef enum {
	NON_AWARE, FORCE_OPTIMAL, FORCE_NON_OPTIMAL
} NUMA_MODE;

// Enumeration of benchmark precision modes,
// can select FAST or SLOW (better precision, but slow)
typedef enum {
	SLOW, FAST
} PRECISION_MODE;

// Enumeration of machine readable output modes
typedef enum {
	MROFF, MRON
} MACHINEREADABLE_MODE;

// Options control routines
void detectMethods( DWORD *select, DWORDLONG *bitmap, DWORDLONG bitmapCpu, DWORDLONG bitmapOs );
void printMethods( DWORD select, DWORDLONG bitmap, DWORDLONG bitmapCpu, DWORDLONG bitmapOs, CHAR *methodsNames[] );



