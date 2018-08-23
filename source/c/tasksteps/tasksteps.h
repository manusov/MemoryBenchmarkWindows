/*
 *    Memory Performance Engine (MPE) main task routines-per-steps definitions.
 *    Each step make console output and/or modify state variables.
 */
 
 // Default options settings
#define NOT_SET -1           // constant means no overrides for option, set default or default=f(sys)
#define AUTO_SET -2          // constant means auto-detect by platform configuration
#define DEFAULT_RW_METHOD AUTO_SET       // default method for read-write memory
#define DEFAULT_RW_TARGET L1_CACHE       // default tested object is L1 cache memory
#define DEFAULT_RW_ACCESS AUTO_SET       // non-temporal data mode disabled by default
#define DEFAULT_THREADS_COUNT AUTO_SET   // number of execution threads, default single thread
#define DEFAULT_HYPER_THREADING HTOFF    // hyper-threading disabled by default
#define DEFAULT_PAGE_SIZE MIN_PAGES      // default pages is minimal size, 4KB
#define DEFAULT_NUMA_MODE NON_AWARE      // default NUMA mode is non aware
#define DEFAULT_PRECISION SLOW           // default test mode is precision
#define DEFAULT_MACHINEREADABLE MROFF    // machine readable output disabled by default
#define DEFAULT_MIN_BLOCK 4096           // minimum size of default data block
#define DEFAULT_MAX_BLOCK 65536          // maximum size of default data block
#define DEFAULT_STEP_BLOCK 1024          // default step from min to max is 512 bytes
#define DEFAULT_PAGE_MODE NORMAL         // default page mode, 0 means classic 4KB
#define DEFAULT_BUF_ALIGN 4096           // alignment factor, 4KB is default page size for x86/x64

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
	SLOW, FAST
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
    DWORD32 measurementRepeats;
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
    void ( __stdcall *DLL_ExecuteXgetbv   ) ( DWORDLONG* );
    BOOL ( __stdcall *DLL_MeasureTsc      ) ( DWORDLONG* );
    BOOL ( __stdcall *DLL_PerformanceGate ) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* );
    CHAR* name1;
    CHAR* name2;
    CHAR* name3;
    CHAR* name4;
    CHAR* name5;
    CHAR* name6;
} LIST_DLL_FUNCTIONS;

// Structure for list objects requires release
typedef struct {
    LPVOID bufferTarget;              // TODO: this requires change if NUMA and nulti-buffer, use pointer to list
    double* bufferStatistics;
    int statisticsCount;
    HANDLE nativeLibrary;
    CHAR* dllName;
} LIST_RELEASE_RESOURCES;

// Task routines sequencer
void taskRoot( int argc, char** argv );

// Task routines-per-steps, phase 1 = before target operation
void stepDefaults( MPE_USER_INPUT* xi, LIST_RELEASE_RESOURCES* xr );
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
void stepMemoryAlloc( MPE_USER_INPUT* xi, MPE_INPUT_PARAMETERS_BLOCK* ipb, LIST_RELEASE_RESOURCES* xr );
void stepStatisticsAlloc( MPE_INPUT_PARAMETERS_BLOCK* ipb, LIST_RELEASE_RESOURCES* xr );
// Task routines-per-steps, phase 3 = build Input Parameters Block
void stepBuildIpb( MPE_USER_INPUT* xu, MPE_PLATFORM_INPUT* xp, MPE_INPUT_PARAMETERS_BLOCK* ipb,
                   PRINT_ENTRY parmList[],
                   LIST_RELEASE_RESOURCES* xr );
// Benchmarks calibration, adjust measurement repeats count
void stepCalibration( MPE_CALIBRATION* xc, LIST_RELEASE_RESOURCES* xr );

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


