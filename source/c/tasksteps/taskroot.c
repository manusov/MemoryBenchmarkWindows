/*
 *    Memory benchmark task root sequencer routine and global (for all steps) data.
 *    Note output string yet not used, reserved.
 */

// Benchmark sequence data
MPE_USER_INPUT userInput;
MPE_USER_OUTPUT userOutput;
MPE_PLATFORM_INPUT platformInput;
MPE_INPUT_PARAMETERS_BLOCK targetIpb;
MPE_OUTPUT_PARAMETERS_BLOCK targetOpb;

// Data for communication between steps routines
LIST_DLL_FUNCTIONS listDll;
LIST_RELEASE_RESOURCES listRelease;

// Command line parameters parse control
// Strings for command line options detect
// Methods for read-write memory by CPU instruction set
char* rwMethods[] = { 
#if NATIVE_WIDTH == 32
    "readmov32", "writemov32", "copymov32", "modifynot32",
    "writestring32", "copystring32",
#endif
#if NATIVE_WIDTH == 64
    "readmov64", "writemov64", "copymov64", "modifynot64",
    "writestring64", "copystring64",
#endif
    "readsse128", "writesse128", "copysse128",
    "readavx256", "writeavx256", "copyavx256",
    "readavx512", "writeavx512", "copyavx512",
    "dotfma256", "dotfma512",
    NULL
};
// Target objects for benchmarks
char* rwTargets[] = {
    "l1cache", "l2cache", "l3cache", "l4cache", "dram", "userdefined",
    NULL
};
// Access methods by temporal/nontemporal classification
char* rwAccess[] = {
    "temporal", "nontemporal", "default",
    NULL
};
// Hyper-threading control
char* hyperThreading[] = {
    "off", "on"
};
// Page size control
char* pageSize[] = {
    "default", "large",
    NULL
};
// NUMA control
char* numaMode[] = {
    "nonaware", "forceoptimal", "forcenonoptimal",
    NULL
};
// Precision control
char* precision[] = {
    "slow", "fast",
    NULL
};
// Machine readable option control
char* machineReadable[] = {
    "off", "on",
    NULL
};

// Methods for read-write memory by CPU instruction set
char* rwMethodsDetails[] = {
#if NATIVE_WIDTH == 32
	"Read ia32 (MOV)",
	"Write ia32 (MOV)",
	"Copy ia32 (MOV)",
	"Modify ia32 (NOT)",
	"Write ia32 strings (REP STOSD)",
	"Copy ia32 strings (REP MOVSD)",
#endif
#if NATIVE_WIDTH == 64
	"Read x86-64 (MOV)",
	"Write x86-64 (MOV)",
	"Copy x86-64 (MOV)",
	"Modify x86-64 (NOT)",
	"Write x86-64 strings (REP STOSQ)",
	"Copy x86-64 strings (REP MOVSQ)",
#endif
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
	"Non-temporal copy SSE-128 (MOVAPS+MOVNTPS)",    // this duplicated 1
	"Non-temporal write AVX-256 (VMOVNTPD)",
	"Non-temporal copy AVX-256 (VMOVAPD+VMOVNTPD)",    // this duplicated 2, wrong
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
	// based on PREFETCHNTA hint
	"Non-temporal read SSE-128 (PREFETCHNTA+MOVAPS)",
	"Non-temporal copy SSE-128 (PREFETCHNTA+MOVAPS+MOVNTPS)",  // this duplicated 1
	"Non-temporal read AVX-256 (PREFETCHNTA+VMOVAPD)",
	// Reserved for extensions
	// "Non-temporal copy AVX-256 (PREFETCHNTA+VMOVAPD)"
	// "Non-temporal read AVX-512 (PREFETCHNTA+VMOVAPD)"
	// "Non-temporal copy AVX-512 (PREFETCHNTA+VMOVAPD)"
	// Reserved for FMA with non-temporal store
	NULL
};
// Target objects for benchmarks
char* rwTargetsDetails[] = {
	"Cache L1", "Cache L2", "Cache L3", "Cache L4", "System DRAM", "User-defined memory object",
	NULL
};
// Access methods by temporal/nontemporal classification
char* rwAccessDetails[] = {
	"Temporal data", "Non-temporal data", "Default caching mode",
	NULL
};
// Hyper-threading control
char* hyperThreadingDetails[] = {
	"Not supported by platform", "Not used but supported by platform", "Enabled and used"
};
// Page size control
char* pageSizeDetails[] = {
	"Default 4K", "Large 4M (ia32)", "Large 2M (x64)",
	NULL
};
// NUMA control
char* numaModeDetails[] = {
	"Non aware", "Force optimal", "Force non-optimal",
	NULL
};
// Precision control
char* precisionDetails[] = {
	"Slow and accurate", "Fast and approximation",
	NULL
};
// Machine readable option control
char* machineReadableDetails[] = {
	"Off", "On",
	NULL
};

// Option control list, used for command line parameters parsing (regular input)
OPTION_ENTRY commandLineOptions[] = {
    { "operation"       , rwMethods       , &userInput.optionRwMethod        , SELPARM } ,
    { "target"          , rwTargets       , &userInput.optionRwTarget        , SELPARM } ,
    { "access"          , rwAccess        , &userInput.optionNonTemporal     , SELPARM } ,
    { "threads"         , NULL            , &userInput.optionThreadsCount    , INTPARM } ,
    { "hyperthreading"  , hyperThreading  , &userInput.optionHyperThreading  , SELPARM } ,
    { "pagesize"        , pageSize        , &userInput.optionPageSize        , SELPARM } ,
    { "numa"            , numaMode        , &userInput.optionNuma            , SELPARM } ,
    { "precision"       , precision       , &userInput.optionPrecision       , SELPARM } ,
    { "machinereadable" , machineReadable , &userInput.optionMachineReadable , SELPARM } ,
    { "start"           , NULL            , &userInput.optionBlockStart      , MEMPARM } ,
    { "end"             , NULL            , &userInput.optionBlockStop       , MEMPARM } ,
    { "step"            , NULL            , &userInput.optionBlockDelta      , MEMPARM } ,
    { NULL              , NULL            , NULL                             , 0       }
};

// System configuration print list, used for system configuration visual (regular output)
PRINT_ENTRY targetParameters[] = {
    { "CPU operation"       , rwMethodsDetails       , &targetIpb.selectRwMethod        , SELECTOR } ,
/*  THIS OPTIONS SUPPORT IS UNDER CONSTRUCTION     
    { "Target object"       , rwTargetsDetails       , &targetIpb.selectRwTarget        , SELECTOR } ,
    { "Cacheability mode"   , rwAccessDetails        , &targetIpb.selectNonTemporal     , SELECTOR } ,
    { "Threads count"       , NULL                   , &targetIpb.selectThreadsCount    , INTEGER  } ,
    { "Hyper-threading"     , hyperThreadingDetails  , &targetIpb.selectHyperThreading  , SELECTOR } ,
    { "Paging mode"         , pageSizeDetails        , &targetIpb.selectPageSize        , SELECTOR } ,
    { "NUMA topology"       , numaModeDetails        , &targetIpb.selectNuma            , SELECTOR } ,
    { "Precision mode"      , precisionDetails       , &targetIpb.selectPrecision       , SELECTOR } ,
    { "Machine readable"    , machineReadableDetails , &targetIpb.selectMachineReadable , SELECTOR } ,
UNDER CONSTRUCTION */
    { "Allocated buffer"    , NULL                   , &targetIpb.baseOriginal          , POINTER  } ,
    { "Aligned source"      , NULL                   , &targetIpb.baseSrcAligned        , POINTER  } ,
    { "Aligned destination" , NULL                   , &targetIpb.baseDstAligned        , POINTER  } ,
    { "Start block size"    , NULL                   , &targetIpb.selectBlockStart      , MEMSIZE  } ,
    { "End block size"      , NULL                   , &targetIpb.selectBlockStop       , MEMSIZE  } ,
    { "Block size step"     , NULL                   , &targetIpb.selectBlockDelta      , MEMSIZE  } ,
    { NULL                  , NULL                   , NULL                             , 0        }
};

// Benchmark sequence routine
void taskRoot( int argc, char** argv )
{
    // Set options defaults
    stepDefaults( &userInput, &listRelease );
    // Override options by command line
    stepCommandLine( argc, argv, commandLineOptions );
    // Check options values, must be DEFAULT if support UNDER CONSTRUCTION
    stepOptionCheck( userInput.optionRwMethod        , DEFAULT_RW_METHOD       , commandLineOptions[0].name );
    stepOptionCheck( userInput.optionRwTarget        , DEFAULT_RW_TARGET       , commandLineOptions[1].name );
    stepOptionCheck( userInput.optionNonTemporal     , DEFAULT_RW_ACCESS       , commandLineOptions[2].name );
    stepOptionCheck( userInput.optionThreadsCount    , DEFAULT_THREADS_COUNT   , commandLineOptions[3].name );
    stepOptionCheck( userInput.optionHyperThreading  , DEFAULT_HYPER_THREADING , commandLineOptions[4].name );
    stepOptionCheck( userInput.optionPageSize        , DEFAULT_PAGE_SIZE       , commandLineOptions[5].name );
    stepOptionCheck( userInput.optionNuma            , DEFAULT_NUMA_MODE       , commandLineOptions[6].name );
    stepOptionCheck( userInput.optionPrecision       , DEFAULT_PRECISION       , commandLineOptions[7].name );
    stepOptionCheck( userInput.optionMachineReadable , DEFAULT_MACHINEREADABLE , commandLineOptions[8].name );
    // Load native DLL, show strings
    stepLoadLibrary( &listDll, &listRelease );
    // Check library function pointers
    stepFunctionCheck( listDll.DLL_GetDllStrings   , listDll.name1 , listRelease.dllName );
    stepFunctionCheck( listDll.DLL_CheckCpuid      , listDll.name2 , listRelease.dllName );
    stepFunctionCheck( listDll.DLL_ExecuteCpuid    , listDll.name3 , listRelease.dllName );
    stepFunctionCheck( listDll.DLL_ExecuteXgetbv   , listDll.name4 , listRelease.dllName );
    stepFunctionCheck( listDll.DLL_MeasureTsc      , listDll.name5 , listRelease.dllName );
    stepFunctionCheck( listDll.DLL_PerformanceGate , listDll.name6 , listRelease.dllName );
    // Check CPUID, show CPU vendor and model strings, get functionality bitmaps, show this bitmaps
    stepDetectCpu( &listDll, &platformInput.platformFeatures, &listRelease );
    // Check TSC support, measure TSC clock, show
    stepMeasureCpu( &listDll, &platformInput.platformTimings, &listRelease );
    // Allocate memory for target benchmark read/write buffer
    stepMemoryAlloc( &userInput, &targetIpb, &listRelease );
    // Allocate memory for benchmark statistics buffer
    stepStatisticsAlloc( &targetIpb, &listRelease );
    // Build Input Parameters Block (IPB) for benchmark routine, show IPB fields
    stepBuildIpb( &userInput, &platformInput, &targetIpb, targetParameters, &listRelease );
    // Execute benchmark
    stepPerformance( &listDll, &platformInput, &targetIpb, &targetOpb, &listRelease );
    // Show benchmark results
    stepInterpretingOpb( &targetOpb, &userOutput, &listRelease );
    // Release allocated resources
    stepRelease( &listRelease );
}


