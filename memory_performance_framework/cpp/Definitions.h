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

Application definitions header file.
TODO.

*/

#pragma once
#ifndef DEFINITIONS_H
#define DEFINITIONS_H

#include <windows.h>

// List of dynamically load libraries (DLLs) and paths. OS API DLLs and this application FASM DLL.
typedef struct
{
	const LPCSTR libName;
	const LPCSTR libPathName;
	const LPCSTR* fncName;
} LIBRARY_ENTRY;
// Declaration of Unified functions, OS API dynamical import and native DLL functions.
// Unified placeholder for pointer in the all functions load cycle.
typedef void(__stdcall* API_UNI_PTR)();
// Unified placeholder for C++ functions.
// CPU performance gate = same style as ASM DLL interface.
// Compiler performance gate = optimized for C++ functions.
#ifdef _WIN64
typedef BOOL(__stdcall* CPU_PerformanceGate) (DWORD, byte*, byte*, size_t, size_t, size_t, DWORDLONG*);
#elif _WIN32
typedef BOOL(__cdecl* CPU_PerformanceGate) (DWORD, byte*, byte*, size_t, size_t, size_t, DWORDLONG*);
#endif
// OS API, KERNEL32.DLL.
typedef BOOL(__stdcall* API_QueryPerformanceFrequency)(LARGE_INTEGER*);
typedef BOOL(__stdcall* API_QueryPerformanceCounter)(LARGE_INTEGER*);
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
// OS API, ADVAPI32.DLL.
typedef BOOL(__stdcall* API_OpenProcessToken)(HANDLE, DWORD, PHANDLE);
typedef BOOL(__stdcall* API_LookupPrivilegeValue)(LPCSTR, LPCSTR, PLUID);
typedef BOOL(__stdcall* API_AdjustTokenPrivileges)(HANDLE, BOOL, PTOKEN_PRIVILEGES, DWORD, PTOKEN_PRIVILEGES, PDWORD);
// Assembler DLL.
typedef void(__stdcall* DLL_GetDllStrings) (const char**, const char**, const char**);
typedef BOOL(__stdcall* DLL_CheckCpuid) (void);
typedef void(__stdcall* DLL_ExecuteCpuid) (DWORD, DWORD, DWORD*, DWORD*, DWORD*, DWORD*);
typedef void(__stdcall* DLL_ExecuteRdtsc) (DWORDLONG*);
typedef void(__stdcall* DLL_ExecuteXgetbv) (DWORDLONG*);
typedef BOOL(__stdcall* DLL_MeasureTsc) (DWORD64*);
typedef BOOL(__stdcall* DLL_MeasureTscByFileTime) (LARGE_INTEGER*);
typedef BOOL(__stdcall* DLL_MeasureTscByPcounter) (LARGE_INTEGER*, API_QueryPerformanceCounter, LARGE_INTEGER);
typedef BOOL(__stdcall* DLL_PerformanceGate) (DWORD, byte*, byte*, size_t, size_t, size_t, DWORDLONG*);
// Structure for OS API dynamical import and native DLL functions entry points list.
typedef struct {
	// OS API functions dynamical import, KERNEL32.DLL.
	API_QueryPerformanceFrequency         api_QueryPerformanceFrequency;
	API_QueryPerformanceCounter           api_QueryPerformanceCounter;
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
	// OS API functions dynamical import, ADVAPI32.DLL.
	API_OpenProcessToken                  api_OpenProcessToken;
	API_LookupPrivilegeValue              api_LookupPrivilegeValue;
	API_AdjustTokenPrivileges             api_AdjustTokenPrivileges;
	// This application assembler DLL functions.
	DLL_GetDllStrings                     dll_GetDllStrings;
	DLL_CheckCpuid                        dll_CheckCpuid;
	DLL_ExecuteCpuid                      dll_ExecuteCpuid;
	DLL_ExecuteRdtsc                      dll_ExecuteRdtsc;
	DLL_ExecuteXgetbv                     dll_ExecuteXgetbv;
	DLL_MeasureTsc                        dll_MeasureTsc;
	DLL_MeasureTscByFileTime              dll_MeasureTscByFileTime;
	DLL_MeasureTscByPcounter              dll_MeasureTscByPcounter;
	DLL_PerformanceGate                   dll_PerformanceGate;
	// Selected function pointer for given thread, optionally used if assembler DLL
	// performance gate not used (for C++ code: tests with vectors, intrinsics and same).
	// Use this position instead per-thread data for minimize per-thread data size,
	// replicated for all threads.
	CPU_PerformanceGate                   selected_pattern;
	// Load status.
	DWORD loadStatus;
} FUNCTIONS_LIST;
// Processor information, required for benchmarks scenarios.
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
// System memory information data.
typedef struct {
	DWORD64 physicalMemory;
	DWORD64 freeMemory;
} SYSTEM_MEMORY_DATA;
// Paging information data.
typedef struct {
	SIZE_T defaultPage;
	SIZE_T largePage;
	DWORD32 pagingRights;
} SYSTEM_PAGING_DATA;
// This structure used per each NUMA node.
// Array of this structures is get platform NUMA topology result.
typedef struct {
	DWORD nodeId;              // NUMA Node number, if no NUMA, this field = 0 for all entries.
	GROUP_AFFINITY nodeGaff;   // NUMA Node affinity mask and processor group id.
	LPVOID baseAtNode;         // Block base address after alignment, useable for r/w operation.
	SIZE_T sizeAtNode;         // Block size after alignment, useable for read/write operation.
	LPVOID trueBaseAtNode;     // Base address, returned when allocated, for release, 0=not used.
} NUMA_NODE_ENTRY;
// This structure for list of NUMA nodes.
typedef struct {
	DWORD nodeCount;
	NUMA_NODE_ENTRY* nodeList;
} SYSTEM_NUMA_DATA;
// This structure used per thread.
// Array of this structures is multithread context control data. 
typedef struct {
	HANDLE threadHandle;          // Thread Handle for execution thread.
	HANDLE rxEventHandle;         // Event Handle for daughter thread operation complete signal.
	HANDLE txEventHandle;         // Event Handle for daughter thread operation continue enable signal.
	LPVOID base1;                 // Source for read, destination for write and modify.
	LPVOID base2;                 // Destination for copy.
	DWORD64 sizeInstructions;     // Block size, units = instructions, for benchmarking.
	DWORD64 measurementRepeats;   // Number of measurement repeats.
	BYTE largePagesMode;          // Bit D0=Large Pages, other bits [1-31/63] = reserved.
	BYTE methodId;                // Entry point to Target operation method subroutine ID.
	BYTE asmOrCpp;                // Added for support C++ tests: vectors, intrinsics.
	BYTE terminateThread;         // Flag for thread termination by return from callback routine.
	GROUP_AFFINITY optimalGaff;   // Affinity mask and group id, OPTIMAL for this thread.
	GROUP_AFFINITY originalGaff;  // Affinity mask and group id, ORIGINAL for this thread.
	FUNCTIONS_LIST* pRoutines;    // Pointers for dynamical import and this application DLL functions.
} THREAD_CONTROL_ENTRY;
// This structure for list of threads.
typedef struct {
	// Reference to list of threads.
	DWORD threadCount;
	THREAD_CONTROL_ENTRY* pThreadsList;
	// Reference to list of operation done events.
	HANDLE* pSignalsList;
} SYSTEM_THREADS_DATA;
// This structure for benchmark iterations initializing,
// constants, not changed for sequence of iterations.
typedef struct {
	DWORD64 startSize;
	DWORD64 endSize;
	DWORD64 deltaSize;
	DWORD64 maxSizeBytes;  // This for memory allocation.
	DWORD64 pageSize;
	WORD threadCount;
	BYTE pagingMode;
	BYTE htMode;
	BYTE numaMode;
} INPUT_CONSTANTS;
// This structure for benchmark iteration parameters update
// variables, must be updated for each iteration.
typedef struct {
	DWORD64 currentSizeInstructions;
	DWORD64 currentMeasurementRepeats;
	CPU_PerformanceGate currentRoutine;  // This procedure pointer optionally used if asmOrCpp = 1.
	BYTE currentMethodId;                // Can dynamically change method for latency measurement: prepare/walk.
	BYTE asmOrCpp;                       // Added for support C++ tests: vectors, intrinsics.
	BYTE terminateThread;                // Added for thread termination without unsafe function TerminateThread.
} INPUT_VARIABLES;
// This structure for output data of iteration.
typedef struct {
	DWORD64 resultInstructions;  // This field yet reserved.
	DWORD64 resultDeltaTsc;
} OUTPUT_VARIABLES;
// Enumerations for options states.
typedef enum {
	HELP_BRIEF, HELP_FULL
} HELP_KEYS;
typedef enum {
	INFO_CPU, INFO_TOPOLOGY, INFO_TOPOLOGY_EX, INFO_MEMORY, INFO_PAGING,  // yet reserved: INFO_CACHE
	INFO_API, INFO_DOMAINS, INFO_THREADS,
	INFO_ALL
} INFO_KEYS;
typedef enum {
	TEST_MEMORY, TEST_STORAGE, TEST_CPU, TEST_GPU, TEST_TIMERS, TEST_SORTING
} TEST_KEYS;
typedef enum {
	IN_DEFAULT
} IN_KEYS;
typedef enum {
	OUT_SCREEN, OUT_FILE
} OUT_KEYS;
// Enumeration of target tested objects, argument for select block size and number of threads.
typedef enum {
	CACHE_L1, CACHE_L2, CACHE_L3, CACHE_L4, DRAM
} MEMORY_OBJECTS;
// Enumeration for page size modes.
typedef enum {
	LP_NOT_SUPPORTED, LP_NOT_USED, LP_USED
} PAGING_MODES;
// Enumeration of Hyper-Threading (HT) mode.
typedef enum {
	HT_NOT_SUPPORTED, HT_NOT_USED, HT_USED
} HT_MODES;
// Enumeration of NUMA modes.
typedef enum {
	NUMA_NOT_SUPPORTED, NUMA_UNAWARE, NUMA_LOCAL, NUMA_REMOTE
} NUMA_MODES;
// Parameters Block for user input.
// This data = f ( user settings at command line or defaults ).
typedef struct {
	// Scenario selectors.
	DWORD optionHelp;
	DWORD optionInfo;
	DWORD optionTest;
	// Output mode, screen or file.
	DWORD optionOut;
	// Benchmark base option.
	DWORD optionAsm;
	DWORD optionMemory;
	DWORD optionThreads;
	// Large pages control.
	DWORD optionPageSize;
	// Constant repeats control.
	DWORD optionRepeats;
	// Advanced adaptive repeats control.
	DWORD64 optionAdaptive;
	// Topology options.
	DWORD optionHt;
	DWORD optionNuma;
	// Block sizes explicit set.
	DWORD64 optionBlockStart;
	DWORD64 optionBlockStop;
	DWORD64 optionBlockDelta;
} COMMAND_LINE_PARMS;
// Types of command line options.
typedef enum {
	NOPARM, INTPARM, MEMPARM, SELPARM, STRPARM
} OPTION_TYPES;
// Command line option description entry.
typedef struct {
	const char* name;             // Pointer to parm. name for recognition NAME=VALUE.
	const char** values;          // Pointer to array of strings pointers, text opt.
	const void* data;             // Pointer to updated option variable.
	const OPTION_TYPES routine;   // Select handling method for this entry.
} OPTION_ENTRY;
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
// Options for memory block size print as integer
typedef enum {
	PRINT_SIZE_AUTO, 
	PRINT_SIZE_BYTES,
	PRINT_SIZE_KB,
	PRINT_SIZE_MB,
	PRINT_SIZE_GB
} PRINT_SIZE_MODES;

#endif  //  DEFINITIONS_H
