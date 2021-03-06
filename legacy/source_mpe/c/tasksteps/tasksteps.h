/*
 *    Memory Performance Engine (MPE) main task routines-per-steps definitions.
 *    Each step make console output and/or modify state variables.
 */
 
 // Default options settings
#define NOT_SET -1           // constant means no overrides for option, set default or default=f(sys)
#define AUTO_SET -2          // constant means auto-detect by platform configuration
#define DEFAULT_RW_METHOD AUTO_SET        // default method for read-write memory
#define DEFAULT_RW_TARGET L1_CACHE        // default tested object is L1 cache memory
#define DEFAULT_RW_ACCESS NOT_SET         // non-temporal data mode disabled by default
#define DEFAULT_THREADS_COUNT AUTO_SET    // number of execution threads, default single thread
#define DEFAULT_HYPER_THREADING HTOFF     // hyper-threading disabled by default
#define DEFAULT_PAGE_SIZE MIN_PAGES       // default pages is minimal size, 4KB
#define DEFAULT_NUMA_MODE NON_AWARE       // default NUMA mode is non aware
#define DEFAULT_PRECISION AUTO_CALIBRATE  // default test precision mode is auto calibration
#define DEFAULT_MACHINEREADABLE MROFF     // machine readable output disabled by default
// Memory buffer settings
#define DEFAULT_MIN_BLOCK NOT_SET         // 4096  // minimum size of default data block
#define DEFAULT_MAX_BLOCK NOT_SET         // 65536 // maximum size of default data block
#define DEFAULT_STEP_BLOCK NOT_SET        // 1024  // default step from min to max is 512 bytes
#define DEFAULT_PAGE_MODE NORMAL          // default page mode, 0 means classic 4KB
#define DEFAULT_BUF_ALIGN 4096            // alignment factor, 4KB is default page size for x86/x64
// Benchmarks measurements settings
#define DEFAULT_MEASUREMENT_TIME 1.0               // target time for adjust measurement repeats, seconds
#define DEFAULT_MEASUREMENT_APPROXIMATION 5000     // start value for measurement repeats, iterations
#define DEFAULT_FAST_L1 100000           // Number of measurement iterations for objects, fast mode defaults
#define DEFAULT_FAST_L2 50000
#define DEFAULT_FAST_L3 1000
#define DEFAULT_FAST_DRAM 100
#define DEFAULT_FAST_CUSTOM 100000
#define DEFAULT_SLOW_L1 2000000          // Number of measurement iterations for objects, slow mode defaults
#define DEFAULT_SLOW_L2 500000
#define DEFAULT_SLOW_L3 10000
#define DEFAULT_SLOW_DRAM 200
#define DEFAULT_SLOW_CUSTOM 1000000

// Enumerations for options settings
// Bitmaps generation for CPU and OS support (see associated structure at constants.c)
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
	SLOW, FAST, AUTO_CALIBRATE
} PRECISION_MODE;

// Enumeration of machine readable output modes
typedef enum {
	MROFF, MRON
} MACHINEREADABLE_MODE;

// Bitmap of supported read/write methods = f ( CPU features )
typedef struct {
    DWORD64 bitmapCpu;
    DWORD64 bitmapOs;
    DWORD32 maxMethod;
} MPE_CPU_BITMAP;

// CPU Time Stamp Counter (TSC) clock measurement results
typedef struct {
    double hzTsc;
    double nanosecondsTsc;
    DWORD64 clkTsc;
} MPE_CPU_MEASURE;

// Cches list (by WinAPI) data.
// For benchmark actual DATA and UNIFIED caches, other show but rejected
typedef struct {
    DWORD64 pointL1;
    DWORD64 pointL2;
    DWORD64 pointL3;
    DWORD32 hyperThreadingFlag;
} MPE_CACHE_DATA;

// System memory information data
typedef struct {
    DWORD64 pysicalMemory;
    DWORD64 freeMemory;
} MPE_MEMORY_DATA;

// Nested structure for NUMA information data
typedef struct {
	DWORD32 numaId;
	KAFFINITY numaAffinity;
} MPE_NUMA_DOMAIN;

// NUMA information data
typedef struct {
    KAFFINITY systemAffinity;
    DWORD32 domainsCount;
    MPE_NUMA_DOMAIN* domainsList;
} MPE_NUMA_DATA;

// Paging information data
typedef struct {
    DWORD32 defaultPage;
    DWORD32 largePage;
    DWORD32 pagingRights;
} MPE_PAGING_DATA;

// ACPI information data
typedef struct {
    DWORD32 acpiCpuCount;
    DWORD32 acpiNumaNodesCount;
} MPE_ACPI_DATA;

// Benchmark measurement repeats count calibration
typedef struct {
    double measurementTime;
    DWORD32 measurementApproximation;
    DWORD32 measurementRepeats;
    DWORD32 fastL1;
    DWORD32 fastL2;
    DWORD32 fastL3;
    DWORD32 fastDRAM;
    DWORD32 fastCustom;
    DWORD32 slowL1;
    DWORD32 slowL2;
    DWORD32 slowL3;
    DWORD32 slowDRAM;
    DWORD32 slowCustom;
} MPE_CALIBRATION;

// Parameters Block for user input
// This data = f ( user settings at command line or defaults )
typedef struct {
    DWORD optionRwMethod;
    DWORD optionRwTarget;
    DWORD optionNonTemporal;
    DWORD optionThreadsCount;
    DWORD optionHyperThreading;
    DWORD optionPageSize;
    DWORD optionNuma;
    DWORD optionPrecision;
    DWORD optionMachineReadable;
    DWORD64 optionBlockStart; 
    DWORD64 optionBlockStop;
    DWORD64 optionBlockDelta;
} MPE_USER_INPUT;

// Parameters Block fot user output
// This data = f ( benchmarks results )
typedef struct {
	double minMBps;
	double maxMBps;
	double averageMBps;
	double medianMBps;
	double measuredThreshold;
	double medianHitMBps;
	double medianMissMBps;
} MPE_USER_OUTPUT;

// Parameters Block for platform and OS configuration description
// This data = f ( get system information )
typedef struct {
    MPE_CPU_BITMAP platformFeatures;
    MPE_CPU_MEASURE platformTimings;
    MPE_CACHE_DATA platformCache;
    MPE_MEMORY_DATA platformMemory;
    MPE_NUMA_DATA platformNuma;
    MPE_PAGING_DATA platformPaging;
    MPE_ACPI_DATA platformAcpi;
} MPE_PLATFORM_INPUT;

// Input Parameters Block (IPB) for Target Routine
// This data = f ( MPE_USER_INPUT , MPE_PLATFORM_INPUT )
typedef struct {
    DWORD selectRwMethod;
    DWORD selectRwTarget;
    DWORD selectNonTemporal;
    DWORD selectThreadsCount;
    DWORD selectHyperThreading;
    DWORD selectPageSize;
    DWORD selectNuma;
    DWORD selectPrecision;
    DWORD selectMachineReadable;
    LPVOID baseOriginal;               // TODO: this requires change if NUMA and nulti-buffer, use pointer to list
    LPVOID baseSrcAligned;
    LPVOID baseDstAligned;
    SIZE_T selectBlockStart; 
    SIZE_T selectBlockStop;
    SIZE_T selectBlockDelta;
} MPE_INPUT_PARAMETERS_BLOCK;

// Output Parameters Block (OPB) for Target Routine
// MPE_USER_OUTPUT = f ( this data )
typedef struct {
	int countMBps;
	double* arrayMBps;
} MPE_OUTPUT_PARAMETERS_BLOCK;

// Structure for native DLL functions entry points list,
// with functions text names for error reporting
typedef struct {
    void ( __stdcall *DLL_GetDllStrings   ) ( char** , char** , char** );
    BOOL ( __stdcall *DLL_CheckCpuid      ) ( void );
    void ( __stdcall *DLL_ExecuteCpuid    ) ( DWORD, DWORD, DWORD* , DWORD* , DWORD* , DWORD* );
    void ( __stdcall *DLL_ExecuteRdtsc    ) ( DWORDLONG* );
    void ( __stdcall *DLL_ExecuteXgetbv   ) ( DWORDLONG* );
    BOOL ( __stdcall *DLL_MeasureTsc      ) ( DWORDLONG* );
    BOOL ( __stdcall *DLL_PerformanceGate ) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* );
    CHAR* name1;
    CHAR* name2;
    CHAR* name3;
    CHAR* name4;
    CHAR* name5;
    CHAR* name6;
    CHAR* name7;
} LIST_DLL_FUNCTIONS;

// This control structure used per thread.
// Array of this structures is multithread context. 
// Note 1. TrueSize not used when memory release, block identified by base.
// Note 2. Last 3 entries reserved yet.
typedef struct
{
    HANDLE eventHandle;         // Event Handle for this thread operation complete signal
    HANDLE threadHandle;        // Thread Handle for execution thread
    KAFFINITY threadAffinity;   // Affinity Mask = F (True Affinity Mask, Options)
    BOOL ( __stdcall *DLL_PerformanceGate )
         ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* );  // Low-level function entry point
    DWORD_PTR methodId;         // Entry point to Target operation method subroutine ID
    LPVOID base1;               // Source for read, destination for write and modify
    LPVOID base2;               // Destination for copy
    SIZE_T sizeBytes;           // Block size, units = bytes, for memory allocation
    SIZE_T sizeInstructions;    // Block size, units = instructions, for benchmarking
    DWORD_PTR largePages;       // Bit D0=Large Pages, other bits [1-31/63] = reserved
    SIZE_T measurementRepeats;  // Number of measurement repeats
    KAFFINITY trueAffinity;     // True affinity mask, because modified as f(options)
    LPVOID trueBase;            // True (before alignment) memory block base for release
    PDWORD64 returnDeltaTSC;    // Pointer to output list of returned dTSC
    DWORD32 returnCount;        // Output list entries counter
    DWORD32 returnLimit;        // Output list number of entries
} THREAD_CONTROL_ENTRY;

// Data structures for call benchmark patterns in the multi-thread mode.
typedef struct
{
    THREAD_CONTROL_ENTRY* threadsControl;
    HANDLE* signalsHandles;
    int threadsCount;
} MT_DATA;

// Structure for list objects requires release
typedef struct {
    LPVOID bufferTarget;              // TODO: this requires change if NUMA and nulti-buffer, use pointer to list
    double* bufferStatistics;
    int statisticsCount;
    HANDLE nativeLibrary;
    CHAR* dllName;
    THREAD_CONTROL_ENTRY* threadsControl;
    HANDLE* signalsHandles;
} LIST_RELEASE_RESOURCES;

// Task routines sequencer
void taskRoot( int argc, char** argv );

// Task routines-per-steps, phase 1 = before target operation
void stepDefaults( MPE_USER_INPUT* xi, MPE_CALIBRATION* xc, LIST_RELEASE_RESOURCES* xr );
void stepCommandLine( int argc, char** argv, OPTION_ENTRY x[] );
void stepOptionCheck( DWORD currentSetting, DWORD defaultSetting, CHAR *optionName );
void stepLoadLibrary( LIST_DLL_FUNCTIONS* xf, LIST_RELEASE_RESOURCES* xr );
void stepFunctionCheck( void* functionPointer, CHAR* functionName, CHAR* dllName );
void stepDetectCpu( LIST_DLL_FUNCTIONS* xf, MPE_CPU_BITMAP* xb, LIST_RELEASE_RESOURCES* xr );
void stepMeasureCpu( LIST_DLL_FUNCTIONS* xf, MPE_CPU_MEASURE* xm, LIST_RELEASE_RESOURCES* xr );
void stepMpCache( MPE_CACHE_DATA* xc, LIST_RELEASE_RESOURCES* xr );
void stepMemory( MPE_MEMORY_DATA* xm, LIST_RELEASE_RESOURCES* xr );
void stepNuma( MPE_NUMA_DATA* xn, LIST_RELEASE_RESOURCES* xr );
void stepPaging( MPE_PAGING_DATA* xp, LIST_RELEASE_RESOURCES* xr );
void stepAcpi( MPE_ACPI_DATA* xa, LIST_RELEASE_RESOURCES* xr );

// Task routines-per-steps, phase 2 = allocate resources
void stepMemoryAlloc( MPE_INPUT_PARAMETERS_BLOCK* ipb, LIST_RELEASE_RESOURCES* xr );
void stepStatisticsAlloc( MPE_INPUT_PARAMETERS_BLOCK* ipb, LIST_RELEASE_RESOURCES* xr );
// Task routines-per-steps, phase 3 = build Input Parameters Block
void stepBuildIpb( MPE_USER_INPUT* xu, MPE_PLATFORM_INPUT* xp, MPE_INPUT_PARAMETERS_BLOCK* ipb,
                   PRINT_ENTRY parmList[],
                   LIST_RELEASE_RESOURCES* xr );
// Show message about ready to start and parameters list
void stepReadyToStart( PRINT_ENTRY parmList[], LIST_RELEASE_RESOURCES* xr );
// Benchmarks calibration, adjust measurement repeats count
void stepCalibration( MPE_CALIBRATION* xc,
                      LIST_DLL_FUNCTIONS* xf, 
                      MPE_PLATFORM_INPUT* xp, MPE_INPUT_PARAMETERS_BLOCK* ipb,
                      LIST_RELEASE_RESOURCES* xr );

// Task routines-per-steps, phase 4 = target operation
void stepPerformance( LIST_DLL_FUNCTIONS* xf,
                      MPE_PLATFORM_INPUT* xp,
                      MPE_INPUT_PARAMETERS_BLOCK* ipb, MPE_OUTPUT_PARAMETERS_BLOCK* opb,
                      MPE_CALIBRATION* xc, 
                      LIST_RELEASE_RESOURCES* xr );

// Task routines-per-steps, phase 5 = after target operation
void stepInterpretingOpb( MPE_OUTPUT_PARAMETERS_BLOCK* opb, MPE_USER_OUTPUT* uout,
                          LIST_RELEASE_RESOURCES* xr );
void stepRelease( LIST_RELEASE_RESOURCES* xr );

// Helper for release allocated resources
void helperRelease( LIST_RELEASE_RESOURCES* xr );


