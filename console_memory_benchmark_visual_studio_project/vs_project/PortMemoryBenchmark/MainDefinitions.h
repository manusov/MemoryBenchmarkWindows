/*
	MEMORY PERFORMANCE ENGINE FRAMEWORK.
		  Global definitions.
*/

#pragma once
#ifndef MAINDEFINITIONS_H
#define MAINDEFINITIONS_H

// Application common strings definition
#define APPLICATION_STRING  "NUMA CPU&RAM Benchmarks. Console Edition."
#define COPYRIGHT_STRING    "(C) 2022 Ilya Manusov."
#define ANY_KEY_STRING      "Press any key..."

// Build type strings definition
#if _WIN64
#define BUILD_STRING "v0.00.00 for Windows x64."
#define NATIVE_LIBRARY_NAME "ncrb64.dll"
#define NATIVE_WIDTH 64
#elif _WIN32
#define BUILD_STRING "v0.00.00 for Windows ia32."
#define NATIVE_LIBRARY_NAME "ncrb32.dll"

#define NATIVE_WIDTH 32
#else
#define BUILD_STRING "UNSUPPORTED PLATFORM."
#endif

// Input and output files default names
#define INPUT_FILE_NAME  "input.txt"
#define OUTPUT_FILE_NAME "output.txt"

// Operational parameters
#define MAXIMUM_THREADS           256
#define MAXIMUM_THREADS_PER_NODE  64
#define MAXIMUM_NODES             64

#define OPTION_NOT_SET -1
#define CONST_DRAM_BLOCK 32*1024*1024

#define DEFAULT_HELP_SCENARIO OPTION_NOT_SET
#define DEFAULT_INFO_SCENARIO OPTION_NOT_SET
#define DEFAULT_TEST_SCENARIO OPTION_NOT_SET
#define DEFAULT_INPUT IN_DEFAULT
#define DEFAULT_OUTPUT OUT_SCREEN

#define DEFAULT_ASM_METHOD OPTION_NOT_SET
#define DEFAULT_MEMORY_OBJECT OPTION_NOT_SET
#define DEFAULT_THREADS_COUNT OPTION_NOT_SET
#define DEFAULT_MEASUREMENT_REPEATS OPTION_NOT_SET
#define DEFAULT_ADAPTIVE_REPEATS OPTION_NOT_SET
#define DEFAULT_HT_MODE OPTION_NOT_SET
#define DEFAULT_NUMA_MODE OPTION_NOT_SET

// 0-50, total 51 methods
#define MAXIMUM_ASM_METHOD 50

#define APPROXIMATION_REPEATS       2000000
#define APPROXIMATION_REPEATS_L1    2000000
#define APPROXIMATION_REPEATS_L2    500000
#define APPROXIMATION_REPEATS_L3    10000
#define APPROXIMATION_REPEATS_L4    1000
#define APPROXIMATION_REPEATS_DRAM  200
#define REPEATS_DIVISOR_LATENCY     20
#define CALIBRATION_TARGET_TIME     1.0

#define DEFAULT_START_SIZE_BYTES 4096
#define DEFAULT_END_SIZE_BYTES 65536
#define DEFAULT_DELTA_SIZE_BYTES 4096

// Text report definitions
#define TEXT_SIZE 100*1024
#define TX_SIZE 4095


// Enumerations for options settings
// Also used for bitmaps generation for CPU and OS support
typedef enum
{
	PERF_READ_MOV_32_64,    // ID=0
	PERF_WRITE_MOV_32_64,
	PERF_COPY_MOV_32_64,
	PERF_MODIFY_NOT_32_64,
	PERF_WRITE_STRING_32_64,
	PERF_COPY_STRING_32_64,
	PERF_READ_MMX_64,
	PERF_WRITE_MMX_64,
	PERF_COPY_MMX_64,
	PERF_READ_SSE_128,
	PERF_WRITE_SSE_128,
	PERF_COPY_SSE_128,
	PERF_READ_AVX_256,
	PERF_WRITE_AVX_256,
	PERF_COPY_AVX_256,
	PERF_READ_AVX_512,
	PERF_WRITE_AVX_512,
	PERF_COPY_AVX_512,
	PERF_DOT_FMA_256,
	PERF_DOT_FMA_512,
	PERF_GATHER_AVX_256,
	PERF_GATHER_AVX_512,
	PERF_SCATTER_AVX_512,
	PERF_WRITE_CLZERO,
	PERF_LATENCY_BUILD_LCM,
	PERF_LATENCY_BUILD_LCM_32X2,
	PERF_LATENCY_BUILD_RDRAND,
	PERF_LATENCY_BUILD_RDRAND_32X2,
	PERF_LATENCY_WALK,
	PERF_LATENCY_WALK_32X2,
	PERF_NT_READ_SSE_128,
	PERF_NTP_READ_SSE_128_DEFAULT_DISTANCE,
	PERF_NTP_READ_SSE_128_MEDIUM_DISTANCE,
	PERF_NTP_READ_SSE_128_LONG_DISTANCE,
	PERF_NT_WRITE_SSE_128,
	PERF_NT_COPY_SSE_128,
	PERF_NTR_COPY_SSE_128,
	PERF_NT_READ_AVX_256,
	PERF_NTP_READ_AVX_256_DEFAULT_DISTANCE,
	PERF_NTP_READ_AVX_256_MEDIUM_DISTANCE,
	PERF_NTP_READ_AVX_256_LONG_DISTANCE,
	PERF_NT_WRITE_AVX_256,
	PERF_NT_COPY_AVX_256,
	PERF_NTR_COPY_AVX_256,
	PERF_NT_READ_AVX_512,
	PERF_NTP_READ_AVX_512_DEFAULT_DISTANCE,
	PERF_NTP_READ_AVX_512_MEDIUM_DISTANCE,
	PERF_NTP_READ_AVX_512_LONG_DISTANCE,
	PERF_NT_WRITE_AVX_512,
	PERF_NT_COPY_AVX_512,
	PERF_NTR_COPY_AVX_512    // ID = 50 = MAXIMUM_ASM_METHOD
} CPU_PERFORMANCE_FEATURES;

// Declaration of OS API dynamical import and native DLL functions
// OS API
typedef BOOL(__stdcall* API_GetLogicalProcessorInformation) (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD);
typedef BOOL(__stdcall* API_GetLogicalProcessorInformationEx) (LOGICAL_PROCESSOR_RELATIONSHIP, PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, PDWORD);
typedef BOOL(__stdcall* API_GlobalMemoryStatusEx) (LPMEMORYSTATUSEX);
typedef DWORD(__stdcall* API_GetActiveProcessorCount) (WORD);
typedef WORD(__stdcall* API_GetActiveProcessorGroupCount) ();
typedef BOOL(__stdcall* API_GetNumaHighestNodeNumber) (PULONG);
typedef BOOL(__stdcall* API_GetNumaNodeProcessorMask) (UCHAR, PULONGLONG);
typedef BOOL(__stdcall* API_GetNumaNodeProcessorMaskEx) (USHORT, PGROUP_AFFINITY);
typedef LPVOID(__stdcall* API_VirtualAllocExNuma) (HANDLE, LPVOID, SIZE_T, DWORD, DWORD, DWORD);
typedef DWORD_PTR(__stdcall* API_SetThreadAffinityMask) (HANDLE, DWORD_PTR);
typedef BOOL(__stdcall* API_SetThreadGroupAffinity) (HANDLE, const GROUP_AFFINITY*, PGROUP_AFFINITY);
typedef SIZE_T(__stdcall* API_GetLargePageMinimum) ();
typedef UINT(__stdcall* API_GetSystemFirmwareTable) (DWORD, DWORD, PVOID, DWORD);
// Native DLL
typedef void(__stdcall* DLL_GetDllStrings) (char**, char**, char**);
typedef BOOL(__stdcall* DLL_CheckCpuid) (void);
typedef void(__stdcall* DLL_ExecuteCpuid) (DWORD, DWORD, DWORD*, DWORD*, DWORD*, DWORD*);
typedef void(__stdcall* DLL_ExecuteRdtsc) (DWORDLONG*);
typedef void(__stdcall* DLL_ExecuteXgetbv) (DWORDLONG*);
typedef BOOL(__stdcall* DLL_MeasureTsc) (DWORDLONG*);
typedef BOOL(__stdcall* DLL_PerformanceGate) (DWORD, byte*, byte*, size_t, size_t, DWORDLONG*);

// Structure for OS API dynamical import and native DLL functions entry points list
typedef struct {
	DWORD loadStatus;
	// Dynamical import OS API functions
	API_GetLogicalProcessorInformation    api_GetLogicalProcessorInformation;
	API_GetLogicalProcessorInformationEx  api_GetLogicalProcessorInformationEx;
	API_GlobalMemoryStatusEx              api_GlobalMemoryStatusEx;
	API_GetActiveProcessorCount           api_GetActiveProcessorCount;
	API_GetActiveProcessorGroupCount      api_GetActiveProcessorGroupCount;
	API_GetNumaHighestNodeNumber          api_GetNumaHighestNodeNumber;
	API_GetNumaNodeProcessorMask          api_GetNumaNodeProcessorMask;
	API_GetNumaNodeProcessorMaskEx        api_GetNumaNodeProcessorMaskEx;
	API_VirtualAllocExNuma                api_VirtualAllocExNuma;
	API_SetThreadAffinityMask             api_SetThreadAffinityMask;
	API_SetThreadGroupAffinity            api_SetThreadGroupAffinity;
	API_GetLargePageMinimum               api_GetLargePageMinimum;
	API_GetSystemFirmwareTable            api_GetSystemFirmwareTable;
	// This application DLL functions
	DLL_GetDllStrings                     dll_GetDllStrings;
	DLL_CheckCpuid                        dll_CheckCpuid;
	DLL_ExecuteCpuid                      dll_ExecuteCpuid;
	DLL_ExecuteRdtsc                      dll_ExecuteRdtsc;
	DLL_ExecuteXgetbv                     dll_ExecuteXgetbv;
	DLL_MeasureTsc                        dll_MeasureTsc;
	DLL_PerformanceGate                   dll_PerformanceGate;
} FUNCTIONS_LIST;

// Processor information, required for benchmarks scenarios
typedef struct {
	DWORD64 deltaTsc;
	DWORD64 cpuBitmap;
	DWORD64 osBitmap;
} SYSTEM_PROCESSOR_DATA;

// Caches list (by WinAPI) and system SMP topology data.
// For benchmark actual DATA and UNIFIED caches, other show but rejected
typedef struct {
	DWORD64 pointL1;
	DWORD64 pointL2;
	DWORD64 pointL3;
	DWORD64 pointL4;
	DWORD32 coresCount;
	DWORD32 domainsCount;
	DWORD32 packagesCount;
	DWORD32 hyperThreadingFlag;
} SYSTEM_TOPOLOGY_DATA;

// System memory information data
typedef struct {
	DWORD64 physicalMemory;
	DWORD64 freeMemory;
} SYSTEM_MEMORY_DATA;

// Paging information data
typedef struct {
	SIZE_T defaultPage;
	SIZE_T largePage;
	DWORD32 pagingRights;
} SYSTEM_PAGING_DATA;

// This structure used per each NUMA node.
// Array of this structures is get platform NUMA topology result.
typedef struct {
	DWORD nodeId;              // NUMA Node number, if no NUMA, this field = 0 for all entries
	GROUP_AFFINITY nodeGaff;   // NUMA Node affinity mask and processor group id
	LPVOID baseAtNode;         // Block base address after alignment, useable for r/w operation 
	SIZE_T sizeAtNode;         // Block size after alignment, useable for read/write operation
	LPVOID trueBaseAtNode;     // Base address, returned when allocated, for release, 0=not used 
} NUMA_NODE_ENTRY;

// This structure for list of NUMA nodes
typedef struct {
	DWORD nodeCount;
	NUMA_NODE_ENTRY* nodeList;
} SYSTEM_NUMA_DATA;

// This structure used per thread.
// Array of this structures is multithread context control data. 
typedef struct {
	HANDLE threadHandle;          // Thread Handle for execution thread
	HANDLE rxEventHandle;         // Event Handle for daughter thread operation complete signal
	HANDLE txEventHandle;         // Event Handle for daughter thread operation continue enable signal
	LPVOID base1;                 // Source for read, destination for write and modify
	LPVOID base2;                 // Destination for copy
	DWORD64 sizeInstructions;     // Block size, units = instructions, for benchmarking
	DWORD64 measurementRepeats;   // Number of measurement repeats
	WORD largePagesMode;          // Bit D0=Large Pages, other bits [1-31/63] = reserved
	WORD methodId;                // Entry point to Target operation method subroutine ID
	GROUP_AFFINITY optimalGaff;   // Affinity mask and group id, OPTIMAL for this thread
	GROUP_AFFINITY originalGaff;  // Affinity mask and group id, ORIGINAL for this thread
	FUNCTIONS_LIST* pRoutines;    // Pointers for dynamical import and this application DLL functions
} THREAD_CONTROL_ENTRY;

// This structure for list of threads
typedef struct {
	// Reference to list of threads
	DWORD threadCount;
	THREAD_CONTROL_ENTRY* pThreadsList;
	// Reference to list of operation done events
	HANDLE* pSignalsList;
} SYSTEM_THREADS_DATA;

// This structure for benchmark iterations initializing,
// constants, not changed for sequence of iterations
typedef struct {
	DWORD64 startSize;
	DWORD64 endSize;
	DWORD64 deltaSize;
	DWORD64 maxSizeBytes;  // this for memory allocation
	DWORD64 pageSize;
	WORD pagingMode;
	WORD htMode;
	WORD numaMode;
	WORD threadCount;
} INPUT_CONSTANTS;

// This structure for benchmark iteration parameters update
// variables, must be updated for each iteration
typedef struct {
	DWORD64 currentSizeInstructions;
	DWORD64 currentMeasurementRepeats;
	WORD currentMethodId;  // can change for latency measurement: prepare/walk
} INPUT_VARIABLES;

// This structure for output data of iteration
typedef struct {
	DWORD64 resultInstructions;  // this field yet reserved
	DWORD64 resultDeltaTsc;
} OUTPUT_VARIABLES;

// Enumerations for options states

typedef enum {
	HELP_BRIEF, HELP_FULL
} HELP_KEYS;

typedef enum {
	INFO_CPU, INFO_TOPOLOGY, INFO_TOPOLOGY_EX, INFO_MEMORY, INFO_PAGING,  // yet reserved: INFO_CACHE
	INFO_API, INFO_DOMAINS, INFO_THREADS,
	INFO_ALL
} INFO_KEYS;

typedef enum {
	TEST_MEMORY, TEST_STORAGE
} TEST_KEYS;

typedef enum {
	IN_DEFAULT
} IN_KEYS;

typedef enum {
	OUT_SCREEN, OUT_FILE
} OUT_KEYS;

// Enumeration of target tested objects, argument for select block size and number of threads
typedef enum {
	CACHE_L1, CACHE_L2, CACHE_L3, CACHE_L4, DRAM
} MEMORY_OBJECTS;

// Enumeration for page size modes
typedef enum {
	LP_NOT_SUPPORTED, LP_NOT_USED, LP_USED
} PAGING_MODES;

// Enumeration of Hyper-Threading (HT) mode
typedef enum {
	HT_NOT_SUPPORTED, HT_NOT_USED, HT_USED
} HT_MODES;

// Enumeration of NUMA modes
typedef enum {
	NUMA_NOT_SUPPORTED, NUMA_UNAWARE, NUMA_LOCAL, NUMA_REMOTE
} NUMA_MODES;

// Parameters Block for user input
// This data = f ( user settings at command line or defaults )
typedef struct {
	// Scenario selectors
	DWORD optionHelp;
	DWORD optionInfo;
	DWORD optionTest;
	// Output mode, screen or file
	DWORD optionOut;
	// Benchmark base option
	DWORD optionAsm;
	DWORD optionMemory;
	DWORD optionThreads;
	// Large pages control
	DWORD optionPageSize;
	// Constant repeats control
	DWORD optionRepeats;
	// Advanced adaptive repeats control
	DWORD64 optionAdaptive;
	// Topology options
	DWORD optionHt;
	DWORD optionNuma;
	// Block sizes explicit set
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

#endif  // MAINDEFINITIONS_H
