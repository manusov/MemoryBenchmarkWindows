/*
 *   Data constants.
 */

// Global text strings
TCHAR sName[]      = "Memory benchmark engineering sample #1.";
TCHAR sVersion[]   = "v0.00.00. With extended debug messages.";
TCHAR sCopyright[] = "(C)2018 IC Book Labs.";

// Enumeration of assembler read-write subroutines, memory read-write method select
typedef enum {
	READ_X64, WRITE_X64, COPY_X64, MODIFY_X64, 
	WRITE_STRINGS_X64, COPY_STRINGS_X64,
	READ_SSE128, WRITE_SSE128, COPY_SSE128,
	READ_AVX256, WRITE_AVX256, COPY_AVX256,
	READ_AVX512, WRITE_AVX512, COPY_AVX512,
	DOT_FMA256, DOT_FMA512
} RW_METHOD;

// Text strings for option recognition and visual
static char* rwMethods[] = { 
	"readmov64", "writemov64", "copymov64", "modifynot64",
	"writestring64", "copystring64",
	"readsse128", "writesse128", "copysse128",
	"readavx256", "writeavx256", "copyavx256",
	"readavx512", "writeavx512", "copyavx512",
	"dotfma256", "dotfma512",
	NULL
};

// Enumeration of target tested objects, argument for select block size and number of threads
typedef enum {
	L1_CACHE, L2_CACHE, L3_CACHE, L4_CACHE, DRAM
} RW_TARGET;

// Text strings for option recognition and visual
static char* rwTargets[] = {
	"l1cache", "l2cache", "l3cache", "l4cache", "dram",
	NULL
};

// Enumeration of speculative (cacheable) memory access modes
// temporal means cacheable, non-temporal means non cacheable
typedef enum {
	TEMPORAL, NON_TEMPORAL, DEFAULT_CACHING
} RW_ACCESS;

// Text strings for option recognition and visual
static char* rwAccess[] = {
	"temporal", "nontemporal", "default",
	NULL
};

// Enumeration of hyper-threading support options
typedef enum {
	HTOFF, HTON
} HYPER_THREADING;

// Text strings for option recognition and visual
static char* hyperThreading[] = {
	"off", "on"
};

// Enumeration of paging options
typedef enum {
	DEFAULT_PAGES, LARGE_PAGES
} PAGING_MODE;

// Text strings for option recognition and visual
static char* pagingMode[] = {
	"default", "large",
	NULL
};

// Enumeration of NUMA topology support options
typedef enum {
	NON_AWARE, FORCE_OPTIMAL, FORCE_NON_OPTIMAL
} NUMA_MODE;

// Text strings for option recognition and visual
static char* numaMode[] = {
	"nonaware", "forceoptimal", "forcenonoptimal",
	NULL
};

// Enumeration of benchmark precision modes,
// can select FAST or SLOW (better precision, but slow)
typedef enum {
	SLOW, FAST
} PRECISION_MODE;

// Text strings for option recognition and visual
static char* precision[] = {
	"slow", "fast",
	NULL
};

// Enumeration of machine readable output modes
typedef enum {
	MROFF, MRON
} MACHINEREADABLE_MODE;

// Text strings for option recognition and visual
static char* machineReadable[] = {
	"off", "on",
	NULL
};

// See associated definitions at constants.h
static char* rwMethodsDetails[] = {
	"Read x86-64 (MOV)",
	"Write x86-64 (MOV)",
	"Copy x86-64 (MOV)",
	"Modify x86-64 (NOT)",
	"Write x86-64 strings (REP STOSQ)",
	"Copy x86-64 strings (REP MOVSQ)",
	"Read SSE-128 (MOVAPS)",
	"Write SSE-128 (MOVAPS)",
	"Copy SSE-128 (MOVAPS)",
	"Read AVX-256 (VMOVAPD)",
	"Write AVX-256 (VMOVAPD)",
	"Copy AVX-256 (VMOVAPD)",
	"Read AVX-512 (VMOVAPD)",
	"Write AVX-512 (VMOVAPD)",
	"Copy AVX-512 (VMOVAPD)",
	"Dot product FMA-256 (VFMADD231)",
	"Dot product FMA-512 (VFMADD231)",
	// Additions for non-temporal Write
	"Non-temporal write SSE-128 (MOVNTPS)",
	"Non-temporal copy SSE-128 (MOVAPS+MOVNTPS)",
	"Non-temporal write AVX-256 (VMOVNTPD)",
	"Non-temporal copy AVX-256 (VMOVAPD+VMOVNTPD)",
	"Non-temporal write AVX-512 (VMOVNTPD)",
	"Non-temporal copy AVX-512 (VMOVAPD+VMOVNTPD)",
	// Additions for non-temporal Read
	"Non-temporal read SSE-128 (MOVNTDQA)",
	"Non-temporal copy SSE-128 (MOVNTDQA+MOVNTPD)",
	"Non-temporal read AVX-256 (VMOVNTDQA)",
	"Non-temporal copy AVX-256 (VMOVNTDQA+VMOVNTPD)",
	"Non-temporal read AVX-512 (VMOVNTDQA)",
	"Non-temporal copy AVX-512 (VMOVNTDQA+VMOVNTPD)",
	// Additions for non-temporal Read if CPU not supports (V)MOVNTDQA
	// also if (V)MOVNTDQA is supported, but non-optimal
	"Non-temporal read SSE-128 (PREFETCHNTA+MOVAPS)",
	"Non-temporal copy SSE-128 (PREFETCHNTA+MOVAPS+MOVNTPS)",
	"Non-temporal read AVX-256 (PREFETCHNTA+VMOVAPD)"
	"Non-temporal copy AVX-256 (PREFETCHNTA+VMOVAPD)"
	"Non-temporal read AVX-512 (PREFETCHNTA+VMOVAPD)"
	"Non-temporal copy AVX-512 (PREFETCHNTA+VMOVAPD)"
	
};

static BYTE bytesPerInstruction[] = {
	8, 8, 8, 8, 8, 8, 16, 16, 16, 32, 32, 32, 64, 64, 64, 32, 64
};

static char* rwTargetsDetails[] = {
	"L1 cache", "L2 cache", "L3 cache", "L4 cache", "DRAM"
};

static char* rwAccessDetails[] = {
	"Temporal", "Non-temporal", "Default"
};

static char* hyperThreadingDetails[] = {
	"Supported but disabled", "Enabled", "Not supported"
};

static char* pagingModeDetails[] = {
	"Normal", "Large"
};

static char* numaModeDetails[] = {
	"Non-aware", "Force optimal", "Force non-optimal"
};

static char* precisionDetails[] = {
	"Slow and carefully", "Fast"
};

static char* machineReadableDetails[] = {
	"Off", "On"
};

// Control structure for detect optional supported CPU features

static CPUID_CONDITION cpuidControl[] = {
	// Unconditional x86-64 instructions
	{ ORMASK      , 0          , 0          , UNCOND , 0                       , CPU_FEATURES_UNCONDITIONAL },
	// SSE required
	{ ORMASK      , 0x00000001 , 0x00000000 , EDX    , 1<<25                   , CPU_FEATURES_SSE128        },
	// AVX256 include context management required
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , (1<<27)|(1<<28)         , CPU_FEATURES_AVX256        },
	// AVX512 include context management required
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1<<16                   , CPU_FEATURES_AVX512        },
    { ANDMASK     , 0x00000001 , 0x00000000 , ECX    , 1<<27                   , CPU_FEATURES_AVX512        },
    // FMA256 include context management required
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , (1<<27)|(1<<28)|(1<<12) , CPU_FEATURES_FMA256        },
	// FMA512 include context management required
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1<<16                   , CPU_FEATURES_FMA512        },
	{ ANDMASK     , 0x00000001 , 0x00000000 , ECX    , (1<<27)|(1<<12)         , CPU_FEATURES_FMA512        },
	// SSE non-temporal read by MOVNTDQA (SSE4.1)
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , 1<<19                   , CPU_FEATURES_MOVNTDQA128   },
	// AVX256 non-temporal read by VMOVNTDQA (AVX2) include context management required
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1<<5                    , CPU_FEATURES_MOVNTDQA256   },
	{ ANDMASKLAST , 0x00000001 , 0x00000000 , ECX    , 1<<27                   , CPU_FEATURES_AVX512        }
};

static XGETBV_CONDITION xgetbvControl[] ={
	{ UNCOND     , 0                           , CPU_FEATURES_UNCONDITIONAL },
	{ UNCOND     , 0                           , CPU_FEATURES_SSE128        },
	{ UNCOND     , 0                           , CPU_FEATURES_MOVNTDQA128   },
	{ ORMASK     , 1<<2                        , CPU_FEATURES_AVX256        },
	{ ORMASK     , (1<<2)|(1<<5)|(1<<6)|(1<<7) , CPU_FEATURES_AVX512        },
	{ ORMASK     , 1<<2                        , CPU_FEATURES_FMA256        },
	{ ORMASK     , (1<<2)|(1<<5)|(1<<6)|(1<<7) , CPU_FEATURES_FMA512        },
	{ ORMASKLAST , 1<<2                        , CPU_FEATURES_MOVNTDQA256   }
};




