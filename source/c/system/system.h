/*
 *   System information and benchmarks data and routines definitions
 */

// CPU support and native DLL management data
typedef struct {
    DWORD64 bitmapCpu;
    DWORD64 bitmapOs;
} MPE_CPU_BITMAP;

typedef struct {
    double frequencyTsc;
    double periodTsc;
    DWORD64 dtsc;
} MPE_CPU_MEASURE;

typedef struct {
    double mbps;
    double cpi;
    double bpi;
} MPE_CPU_PERFORMANCE;

// Structures for CPU optional supported features detect

typedef enum {
	EAX, EBX, ECX, EDX
} CPUID_OUTPUT;

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

// CPU support and native DLL management functions
DWORD initCpu( CHAR* returnText );
DWORD deinitCpu( CHAR* returnText );
DWORD showDll( CHAR* returnText );
DWORD bitmapCpu ( CHAR* returnText, MPE_CPU_BITMAP* returnBinary );
DWORD measureCpu( CHAR* returnText, MPE_CPU_MEASURE* returnBinary );
DWORDLONG buildCpuidBitmap( CPUID_CONDITION x[] );
DWORDLONG buildXgetbvBitmap( XGETBV_CONDITION x[] );
DWORD performanceCpu( CHAR* returnText, MPE_CPU_PERFORMANCE* returnBinary );
// CPU support and native DLL management: helpers functions
void dllFunctionCheck( void *functionPointer, CHAR *functionName, CHAR *dllName );
// CPUID/RDTSC/XGETBV helpers functions
BOOL getCpuidFeature( DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask );
BOOL getXgetbvFeature( DWORD bitmask );
BOOL detectCpu( CHAR *cpuVendorString, CHAR *cpuModelString );
int measureTsc( DWORDLONG *tscClk, double *tscHz, double *tscNs );
void printCpu( CHAR *cpuVendorString, CHAR *cpuModelString );
void printTsc( double tscHz, double tscNs );

// SMP topology and caches list (by WinAPI) data
typedef struct {      // Benchmark useable points include DATA or UNIFIED caches
    DWORD64 pointL1;
    DWORD64 pointL2;
    DWORD64 pointL3;
    DWORD32 cpuNumaDomains;
    DWORD32 cpuPackages;
    DWORD32 cpuCores;
    DWORD32 cpuLogical;
    DWORD32 cpuHt;
} MPE_TOPOLOGY_DATA;
// SMP topology and caches list (by WinAPI) functions
DWORD initTopology( CHAR* returnText );
DWORD deinitTopology( CHAR* returnText );
DWORD detectTopology( CHAR* returnText, MPE_TOPOLOGY_DATA* returnBinary );

// System memory information data
typedef struct {
    DWORD64 pointPhysicalMemory;
    DWORD64 pointFreeMemory;
} MPE_MEMORY_DATA;
// System memory information functions
DWORD initMemory( CHAR* returnText );
DWORD deinitMemory( CHAR* returnText );
DWORD detectMemory( CHAR* returnText, MPE_MEMORY_DATA* returnBinary );

// SMP and Multi-Threading information data
typedef struct {
    DWORD32 nativeWidth;
    DWORD32 logicalCpuCount;
    DWORD64 logicalCpuBitmap;
} MPE_OS_DATA;
// SMP and Multi-Threading information functions
DWORD initOs( CHAR* returnText );
DWORD deinitOs( CHAR* returnText );
DWORD detectOs( CHAR* returnText, MPE_OS_DATA* returnBinary );

// ACPI information data
typedef struct {
    DWORD32 acpiCpuCount;
    DWORD32 acpiNumaNodesCount;
} MPE_ACPI_DATA;
// ACPI information functions
DWORD initAcpi( CHAR* returnText );
DWORD deinitAcpi( CHAR* returnText );
DWORD detectAcpi( CHAR* returnText, MPE_ACPI_DATA* returnBinary );

// NUMA information data
typedef struct {
    DWORD32 numaDomains;
} MPE_NUMA_COUNT;

typedef struct {
	DWORD32 numaId;
	KAFFINITY numaAffinity;
} MPE_NUMA_DOMAIN;
// NUMA information functions
DWORD initNuma( CHAR* returnText );
DWORD deinitNuma( CHAR* returnText );
DWORD detectNuma( CHAR* returnText, MPE_NUMA_COUNT* returnBinary );
DWORD listNuma( CHAR* returnText, MPE_NUMA_DOMAIN* returnBinary, int nodeSelect );

// Paging information data
typedef struct {
    DWORD32 defaultPage;
    DWORD32 largePage;
    DWORD32 pagingRights;
} MPE_PAGING_DATA;
// Paging information functions
DWORD initPaging( CHAR* returnText );
DWORD deinitPaging( CHAR* returnText );
DWORD detectPaging( CHAR* returnText, MPE_PAGING_DATA* returnBinary );






