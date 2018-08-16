/*
 *   Memory Performance Engine (MPE) main task routines.
 *   Main routine and helpers.
 */

// Application-specific constants and data structures
// Benchmark task data
MPE_USER_INPUT userInput;
MPE_USER_OUTPUT userOutput;
MPE_TASK_CONTROL taskControl;
MPE_PLATFORM_DATA platformData;
DWORD64 andedBitmap = 0;
double *mbpsStatistics;
int mbpsCount;
// Option control list, used for command line parameters parsing (regular input)
OPTION_ENTRY command_line_options_list[] = {
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
PRINT_ENTRY test_parameters_print_list[] = {
    { "CPU operation"       , rwMethodsDetails       , &taskControl.selectRwMethod        , SELECTOR } ,
/*  UNDER CONSTRUCTION
    { "Target object"       , rwTargetsDetails       , &taskControl.selectRwTarget        , SELECTOR } ,
    { "Cacheability mode"   , rwAccessDetails        , &taskControl.selectNonTemporal     , SELECTOR } ,
    { "Threads count"       , NULL                   , &taskControl.selectThreadsCount    , INTEGER  } ,
    { "Hyper-threading"     , hyperThreadingDetails  , &taskControl.selectHyperThreading  , SELECTOR } ,
    { "Paging mode"         , pageSizeDetails        , &taskControl.selectPageSize        , SELECTOR } ,
    { "NUMA topology"       , numaModeDetails        , &taskControl.selectNuma            , SELECTOR } ,
    { "Precision mode"      , precisionDetails       , &taskControl.selectPrecision       , SELECTOR } ,
    { "Machine readable"    , machineReadableDetails , &taskControl.selectMachineReadable , SELECTOR } ,
*/    
    { "Allocated buffer"    , NULL                   , &taskControl.baseOriginal          , POINTER  } ,
    { "Aligned source"      , NULL                   , &taskControl.baseSrcAligned        , POINTER  } ,
    { "Aligned destination" , NULL                   , &taskControl.baseDstAligned        , POINTER  } ,
    { "Start block size"    , NULL                   , &taskControl.selectBlockStart      , MEMSIZE  } ,
    { "End block size"      , NULL                   , &taskControl.selectBlockStop       , MEMSIZE  } ,
    { "Block size step"     , NULL                   , &taskControl.selectBlockDelta      , MEMSIZE  } ,
    { NULL                  , NULL                   , NULL                               , 0        }
};
// Benchmarks results print list, used for results visual
// UNDER CONSTRUCTION 
// Messages strings for initializing procedures,
// this table also limits number of functions (last NULL)
CHAR* functionsNames[] =
{
    "CPU native operations DLL",
    "SMP and Cache topology by WinAPI",
    "Memory information",
    "System information",
    "ACPI tables",
    "NUMA topology",
    "Paging options",
    NULL
};
// Array of pointers to initializing procedures
DWORD ( *pInitFunctions[] )( CHAR* returnText ) =
{
    &initCpu,
    &initTopology,
    &initMemory,
    &initOs,
    &initAcpi,
    &initNuma,
    &initPaging
};
// Array of pointers to de-initializing procedures
DWORD ( *pDeinitFunctions[] )( CHAR* returnText ) =
{
    &deinitCpu,
    &deinitTopology,
    &deinitMemory,
    &deinitOs,
    &deinitAcpi,
    &deinitNuma,
    &deinitPaging
};
// Note in this case init and deinit sequiences same.
// If required deinit as reverse init, lists must be redesinged.

// Memory benchmark task routine
DWORD taskRoot( int argc, char** argv, CHAR* returnString )
{
    // Setup and verify benchmark parameters: Task Control = f ( user input, platform configuration )
    taskAccept( argc, argv );
    // Memory allocation, show benchmark parameters just before run and wait user press key (Y/N)
    taskInput( );
    // Benchmarks execution
    taskExecute( );
    // Memory de-allocation, interpreting and show benchmark results
    taskOutput( );
    // Return to caller
    return STATUS_UNDER_CONSTRUCTION;
}

// Helpers

// Call sequence of initialization and de-initialization routines, with messages
void arrayCaller( CHAR* functionsNames[], DWORD ( *pFunctions[] )( CHAR* returnText ) )
{
    int i = 0;
    DWORD status = 0;
    CHAR tmpStr[40];
    CHAR* pTmpStr = tmpStr;
    while ( functionsNames[i] != NULL )
    {
        *pTmpStr = 0;                         // blank buffer for optionally returned string
		printf( "\n%s... ", functionsNames[i] );   // print target function name
        status = pFunctions[i] ( pTmpStr );   // call target function
        printf( "%s", tmpStr );               // print optionally returned string
        showStatus( status );                 // print operation status per strings
        i++;
    }
}

// verify current and default settings match, if mismatch, show error message and exit
void optionCheck( DWORD currentSetting, DWORD defaultSetting, CHAR *optionName )
{
	if ( currentSetting != defaultSetting )
	{
    	int N = 100;
		CHAR p[N+1];
    	snprintf( p, N, "Cannot change default \"%s\" option in the engineering release", optionName );
        exitWithInternalError( p );
	}
}


