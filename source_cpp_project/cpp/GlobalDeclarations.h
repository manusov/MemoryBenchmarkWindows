#ifndef GLOBALDECLARATIONS_H
#define GLOBALDECLARATIONS_H

// Build type string definition
#if __i386__ & _WIN32
#define BUILD_STRING "v0.50.02 for Windows ia32."
#define NATIVE_LIBRARY_NAME "mpe_w_32.dll"
#define NATIVE_WIDTH 32
#elif __x86_64__ & _WIN64
#define BUILD_STRING "v0.50.02 for Windows x64."
#define NATIVE_LIBRARY_NAME "mpe_w_64.dll"
#define NATIVE_WIDTH 64
#else
#define BUILD_STRING "UNSUPPORTED PLATFORM."
#endif

// Enumerations for options settings
// Also used for bitmaps generation for CPU and OS support
typedef enum {
CPU_FEATURE_READ_IA32_X64,
CPU_FEATURE_WRITE_IA32_X64,
CPU_FEATURE_COPY_IA32_X64,
CPU_FEATURE_MODIFY_IA32_X64,
CPU_FEATURE_WRITE_STRINGS_IA32_X64,
CPU_FEATURE_COPY_STRINGS_IA32_X64,
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
	L1_CACHE, L2_CACHE, L3_CACHE, DRAM
} MEMORY_OBJECTS;

// Structure for native DLL functions entry points list,
typedef struct {
    void ( __stdcall *DLL_GetDllStrings   ) ( char** , char** , char** );
    BOOL ( __stdcall *DLL_CheckCpuid      ) ( void );
    void ( __stdcall *DLL_ExecuteCpuid    ) ( DWORD, DWORD, DWORD* , DWORD* , DWORD* , DWORD* );
    void ( __stdcall *DLL_ExecuteRdtsc    ) ( DWORDLONG* );
    void ( __stdcall *DLL_ExecuteXgetbv   ) ( DWORDLONG* );
    BOOL ( __stdcall *DLL_MeasureTsc      ) ( DWORDLONG* );
    BOOL ( __stdcall *DLL_PerformanceGate ) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* );
} SYSTEM_FUNCTIONS_LIST;

// This control structure used per thread.
// Array of this structures is multithread context. 
typedef struct
{
    HANDLE rxEventHandle;       // Event Handle for daughter thread operation complete signal
    HANDLE txEventHandle;       // Event Handle for daughter thread operation continue enable signal
    HANDLE threadHandle;        // Thread Handle for execution thread
    LPVOID base1;               // Source for read, destination for write and modify
    LPVOID base2;               // Destination for copy
    LPVOID trueBase;            // True (before alignment) memory block base for release
    SIZE_T sizeInstructions;    // Block size, units = instructions, for benchmarking
    SIZE_T measurementRepeats;  // Number of measurement repeats
    KAFFINITY threadAffinity;   // Affinity Mask = F (True Affinity Mask, Options)
    KAFFINITY trueAffinity;     // True affinity mask, because modified as f(options)
    WORD threadGroup;           // Processor group, associated with set affinity mask
    WORD trueGroup;             // Processor group, associated with true affinity mask
    WORD largePages;            // Bit D0=Large Pages, other bits [1-31/63] = reserved
    WORD methodId;              // Entry point to Target operation method subroutine ID
    BOOL ( __stdcall *DLL_PerformanceGate )
         ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* );  // Low-level function entry point
} THREAD_CONTROL_ENTRY;

// Multithread context: pointer to structures list and number of structures
typedef struct
{
	// Variables for build threads list
	LONG64 startSize;
	LONG64 endSize;
	LONG64 deltaSize;
	LONG64 maxSizeBytes;
	LONG64 currentSizeInstructions;
	LONG64 measurementRepeats;
	LONG64 methodId;
	LONG64 pageSize;
	DWORD pagingMode;
	// Threads list
	DWORD nThreadsList;
	THREAD_CONTROL_ENTRY* pThreadsList;
	HANDLE* pSignalsList;
} BENCHMARK_SCENARIO;

// Parameters Block for user input
// This data = f ( user settings at command line or defaults )
typedef struct {
    DWORD optionAsm;
    DWORD optionMemory;
    DWORD optionThreads;
    DWORD optionPageSize;
    DWORD optionRepeats;
    // Reserved for extensions
    DWORD64 optionBlockStart; 
    DWORD64 optionBlockStop;
    DWORD64 optionBlockDelta;
} COMMAND_LINE_PARMS;

// Types of command line options
typedef enum { 
	NOPARM, INTPARM, MEMPARM, SELPARM, STRPARM 
} OPTION_TYPES;

// Command line option description entry
typedef struct {
    const char* name;             // pointer to parm. name for recognition NAME=VALUE
    const char** values;          // pointer to array of strings pointers, text opt.
    const void* data;             // pointer to updated option variable
    const OPTION_TYPES routine;   // select handling method for this entry
} OPTION_ENTRY;

#define OPTION_NOT_SET -1
#define CONST_DRAM_BLOCK 32*1024*1024

#define DEFAULT_ASM_METHOD OPTION_NOT_SET
#define DEFAULT_MEMORY_OBJECT OPTION_NOT_SET
#define MAXIMUM_ASM_METHOD 31
#define DEFAULT_THREADS_COUNT 1
#define DEFAULT_MEASUREMENT_REPEATS OPTION_NOT_SET

#define APPROXIMATION_REPEATS      2000000
#define APPROXIMATION_REPEATS_L1   2000000
#define APPROXIMATION_REPEATS_L2   500000
#define APPROXIMATION_REPEATS_L3   10000
#define APPROXIMATION_REPEATS_DRAM 200
#define CALIBRATION_TARGET_TIME    1.0

#define DEFAULT_START_SIZE_BYTES 4096
#define DEFAULT_END_SIZE_BYTES 65536
#define DEFAULT_DELTA_SIZE_BYTES 4096

// System memory information data
typedef struct {
    DWORD64 physicalMemory;
    DWORD64 freeMemory;
} SYSTEM_MEMORY_DATA;

// Caches list (by WinAPI) and system SMP topology data.
// For benchmark actual DATA and UNIFIED caches, other show but rejected
typedef struct {
    DWORD64 pointL1;
    DWORD64 pointL2;
    DWORD64 pointL3;
    DWORD32 coreCount;
    DWORD32 hyperThreadingFlag;
} SYSTEM_TOPOLOGY_DATA;

// Paging information data
typedef struct {
    DWORD32 defaultPage;
    DWORD32 largePage;
    DWORD32 pagingRights;
} PAGING_OPTIONS_DATA;


#endif  // GLOBALDECLARATIONS_H


