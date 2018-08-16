/*
 *   Memory Performance Engine (MPE) main task routines definitions.
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

// This data = f ( platform configuration )
typedef struct {
    MPE_CPU_BITMAP platformFeatures;
    MPE_CPU_MEASURE platformTimings;
    MPE_TOPOLOGY_DATA platformTopology;
    MPE_MEMORY_DATA platformMemory;
    MPE_NUMA_COUNT platformNuma;
    MPE_PAGING_DATA platformPaging;
} MPE_PLATFORM_DATA;

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

// This data used as input before benchmark task start
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
    LPVOID baseOriginal;
    LPVOID baseSrcAligned;
    LPVOID baseDstAligned;
    SIZE_T selectBlockStart; 
    SIZE_T selectBlockStop;
    SIZE_T selectBlockDelta;
} MPE_TASK_CONTROL;

// This data used as output after benchmark task executed
typedef struct {
	int countMbps;
	double* arrayMbps;
} MPE_TASK_STATUS;

// This data for report to user
typedef struct {
	double minMbps;
	double maxMbps;
	double averageMbps;
	double medianMbps;
	double measuredThreshold;
	double medianHitMbps;
	double medianMissMbps;
} MPE_USER_OUTPUT;

// Routines definition: prepare input parameters, analyse input parameters execute benchmarks, analyse output parameters
void taskAccept( int argc, char** argv );
void taskInput(  );
void taskExecute(  );
void taskOutput(  );
// Memory benchmark task
DWORD taskRoot( int argc, char** argv, CHAR* returnString );
// Helpers
void arrayCaller( CHAR* functionsNames[], DWORD ( *pFunctions[] )( CHAR* returnText ) );
void optionCheck( DWORD currentSetting, DWORD defaultSetting, CHAR *optionName );
BOOL methodCheck( DWORD bitSelect, DWORD64 bitMap );
// Memory allocation services
void allocateBuffer( SIZE_T bufferSize, SIZE_T bufferAlignment, SIZE_T blockMax, 
                     LPVOID *bufferBase, LPVOID *bufferAlignedSrc, LPVOID *bufferAlignedDst );
void releaseBuffer( LPVOID bufferBase );


