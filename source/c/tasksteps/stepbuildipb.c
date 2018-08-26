/*
 *    Build input parameters block = f ( user input, platform data ).
 */

// Default definitions for cache and DRAM benchmarks

#define AUTO_L1 16*1024
#define AUTO_L2 128*1024
#define AUTO_L3 4096*1024
#define AUTO_DRAM 512*1024*1024
#define AUTO_CUSTOM 1024*1024

#define OPTIMAL_COUNT_L1 32
#define OPTIMAL_COUNT_L2 32
#define OPTIMAL_COUNT_L3 16
#define OPTIMAL_COUNT_DRAM 8
#define OPTIMAL_COUNT_CUSTOM 16

#define DEFAULT_SET_MAX 4096
#define DEFAULT_SET_MIN 65536
#define DEFAULT_SET_STEP 1024

/*

Temporal vs Nontemporal methods replacement

ID    Temporal        Nontemporal        Replaced ?
------------------------------------------------------
0     Read_MOV64      Read_MOV64
1     Write_MOV64     Write_MOV64
2     Copy_MOV64      Copy_MOV64
3     Modify_NOT64    Modify_NOT64
4     Write_STOSQ     Write_STOSQ
5     Copy_MOVSQ      Copy_MOVSQ
6     Read_SSE128     NtpRead_SSE128     +
7     Write_SSE128    NtWrite_SSE128     +
8     Copy_SSE128     NtCopy_SSE128      +
9     Read_AVX256     NtpRead_AVX256     +
10    Write_AVX256    NtWrite_AVX256     +
11    Copy_AVX256     NtCopy_AVX256      +
12    Read_AVX512     Read_AVX512 
13    Write_AVX512    NtWrite_AVX512     +
14    Copy_AVX512     NtCopy_AVX512      +
15    Dot_FMA256      Dot_FMA256
16    Dot_FMA512      Dot_FMA512
------------------------------------------------------

C replace argument                      C replace result                       Replaced ?
-------------------------------------------------------------------------------------------
CPU_FEATURE_READ_IA32_X64               CPU_FEATURE_READ_IA32_X64
CPU_FEATURE_WRITE_IA32_X64              CPU_FEATURE_WRITE_IA32_X64
CPU_FEATURE_COPY_IA32_X64               CPU_FEATURE_COPY_IA32_X64
CPU_FEATURE_MODIFY_IA32_X64             CPU_FEATURE_MODIFY_IA32_X64
CPU_FEATURE_WRITE_STRINGS_IA32_X64      CPU_FEATURE_WRITE_STRINGS_IA32_X64
CPU_FEATURE_COPY_STRINGS_IA32_X64       CPU_FEATURE_COPY_STRINGS_IA32_X64
CPU_FEATURE_READ_SSE128                 CPU_FEATURE_NTPRW_READ_SSE128          +
CPU_FEATURE_WRITE_SSE128                CPU_FEATURE_NTW_WRITE_SSE128           +
CPU_FEATURE_COPY_SSE128                 CPU_FEATURE_NTW_COPY_SSE128            +
CPU_FEATURE_READ_AVX256                 CPU_FEATURE_NTPRW_READ_AVX256          +
CPU_FEATURE_WRITE_AVX256                CPU_FEATURE_NTW_WRITE_AVX256           +
CPU_FEATURE_COPY_AVX256                 CPU_FEATURE_NTRW_COPY_AVX256           +
CPU_FEATURE_READ_AVX512                 CPU_FEATURE_READ_AVX512
CPU_FEATURE_WRITE_AVX512                CPU_FEATURE_NTW_WRITE_AVX512           +
CPU_FEATURE_COPY_AVX512                 CPU_FEATURE_NTW_COPY_AVX512            +
CPU_FEATURE_DOT_FMA256                  CPU_FEATURE_DOT_FMA256
CPU_FEATURE_DOT_FMA512                  CPU_FEATURE_DOT_FMA256

Replacements array
------------------------------------------------------
CPU_FEATURE_READ_IA32_X64,
CPU_FEATURE_WRITE_IA32_X64,
CPU_FEATURE_COPY_IA32_X64,
CPU_FEATURE_MODIFY_IA32_X64,
CPU_FEATURE_WRITE_STRINGS_IA32_X64,
CPU_FEATURE_COPY_STRINGS_IA32_X64,
CPU_FEATURE_NTPRW_READ_SSE128,
CPU_FEATURE_NTW_WRITE_SSE128,
CPU_FEATURE_NTW_COPY_SSE128,
CPU_FEATURE_NTPRW_READ_AVX256,
CPU_FEATURE_NTW_WRITE_AVX256,
CPU_FEATURE_NTRW_COPY_AVX256,
CPU_FEATURE_READ_AVX512,
CPU_FEATURE_NTW_WRITE_AVX512,
CPU_FEATURE_NTW_COPY_AVX512,
CPU_FEATURE_DOT_FMA256,
CPU_FEATURE_DOT_FMA256,

*/

#define NT_COUNT 17
BYTE nontemporalMethods[] =
{
    CPU_FEATURE_READ_IA32_X64,
    CPU_FEATURE_WRITE_IA32_X64,
    CPU_FEATURE_COPY_IA32_X64,
    CPU_FEATURE_MODIFY_IA32_X64,
    CPU_FEATURE_WRITE_STRINGS_IA32_X64,
    CPU_FEATURE_COPY_STRINGS_IA32_X64,
    CPU_FEATURE_NTPRW_READ_SSE128,
    CPU_FEATURE_NTW_WRITE_SSE128,
    CPU_FEATURE_NTW_COPY_SSE128,
    CPU_FEATURE_NTPRW_READ_AVX256,
    CPU_FEATURE_NTW_WRITE_AVX256,
    CPU_FEATURE_NTRW_COPY_AVX256,
    CPU_FEATURE_READ_AVX512,
    CPU_FEATURE_NTW_WRITE_AVX512,
    CPU_FEATURE_NTW_COPY_AVX512,
    CPU_FEATURE_DOT_FMA256,
    CPU_FEATURE_DOT_FMA256,
};

// Local (not declared at header) helpers

BOOL methodCheck( DWORD bitSelect, DWORD64 bitMap )
{
    DWORD64 mask = 1;
    if ( bitSelect < 32 )
    {
        mask = mask << bitSelect;
    }
    else
    {
        mask = ( mask << 16 ) << 16;  // This required for 32-bit mode
        bitSelect -= 32;
        mask = mask << bitSelect;
    }
    return ( ( bitMap & mask ) != 0 );
}

// Step action routine

void stepBuildIpb( MPE_USER_INPUT* xu, MPE_PLATFORM_INPUT* xp, 
                   MPE_INPUT_PARAMETERS_BLOCK* ipb,
                   PRINT_ENTRY parmList[], 
                   LIST_RELEASE_RESOURCES* xr )
{
    // Select CPU memory read/write method = f ( application defaults, command line options, platform features)
    DWORD cpuMethod = xu->optionRwMethod;
    if ( cpuMethod == DEFAULT_RW_METHOD )
    {
        cpuMethod = xp->platformFeatures.maxMethod;
    }
    if ( cpuMethod == NOT_SET )
    {
        helperRelease( xr );
        exitWithInternalError( "cannot select read/write method by CPU features" );
    }
    if ( ! methodCheck( cpuMethod, xp->platformFeatures.bitmapCpu ) )
    {
        helperRelease( xr );
        printf( "\n\n%s = %s", parmList[0].name, parmList[0].values[cpuMethod] );
        exitWithInternalError( "selected read/write method not supported by CPU" );
                               
    }
    if ( ! methodCheck( cpuMethod, xp->platformFeatures.bitmapOs ) )
    {
        helperRelease( xr );
        printf( "\n\n%s = %s", parmList[0].name, parmList[0].values[cpuMethod] );
        exitWithInternalError( "selected read/write method not supported by OS" );
    }
    ipb->selectRwMethod = cpuMethod;
    
    // Select target object = f ( application defaults, command line options )
    ipb->selectRwTarget = xu->optionRwTarget;
    DWORD64 blkMin = 0;
    DWORD64 blkMax = 0;
    DWORD64 blkStep = 0;
    DWORD64 blkCount = 0;
    DWORD temporalByObject = DEFAULT_CACHING;
    switch ( ipb->selectRwTarget )
    {
        case L1_CACHE:
            blkMax = xp->platformCache.pointL1;
            if ( blkMax == 0 ) blkMax = AUTO_L1;
            blkCount = OPTIMAL_COUNT_L1;
            temporalByObject = TEMPORAL;
            break;
        case L2_CACHE:
            blkMax = xp->platformCache.pointL2;
            if ( blkMax == 0 ) blkMax = AUTO_L2;
            blkCount = OPTIMAL_COUNT_L2;
            temporalByObject = TEMPORAL;
            break;
        case L3_CACHE:
            blkMax = xp->platformCache.pointL3;
            if ( blkMax == 0 ) blkMax = AUTO_L3;
            blkCount = OPTIMAL_COUNT_L3;
            temporalByObject = TEMPORAL;
            break;
        case L4_CACHE:
        case DRAM:
            blkMax = AUTO_DRAM;
            blkCount = OPTIMAL_COUNT_DRAM;
            temporalByObject = NON_TEMPORAL;   // NON_TEMPORAL even for L4 cache
            break;
        case USER_DEFINED_TARGET:
            blkMax = AUTO_CUSTOM;
            blkCount = OPTIMAL_COUNT_CUSTOM;
            break;
        default:
            break;
    }
    blkMax *= 2;
    blkStep = blkMax / blkCount;
    blkMin = blkStep;
    if ( ( blkMin <= 0 )||( blkMax <= 0 )||( blkStep <= 0 )||( blkCount <= 0 ) )
    {
        helperRelease ( xr );
        exitWithInternalError( "failed buffer parameters assignment for target object" );
    }
    // assign block start/stop/delta sizes from command line parameters
    ipb->selectBlockStart = xu->optionBlockStart;
    ipb->selectBlockStop = xu->optionBlockStop;
    ipb->selectBlockDelta = xu->optionBlockDelta;
    // if no command line options, set automatically
    if ( ipb->selectBlockStart == NOT_SET ) ipb->selectBlockStart = blkMin;
    if ( ipb->selectBlockStop == NOT_SET ) ipb->selectBlockStop = blkMax;
    if ( ipb->selectBlockDelta == NOT_SET ) ipb->selectBlockDelta = blkStep;
    
    // Select temporal/nontemporal mode = f ( application defaults, command line options )
    
    if ( xu->optionNonTemporal != NOT_SET )
    {
        ipb->selectNonTemporal = xu->optionNonTemporal;
    }
    else
    {
        ipb->selectNonTemporal = temporalByObject;
    }
    
    if ( ipb->selectNonTemporal == NON_TEMPORAL )
    {
        if ( ipb->selectRwMethod > NT_COUNT )
        {
            exitWithInternalError( "failed non-temporal memory read/write method selection" );
        }
        else
        {
            ipb->selectRwMethod = nontemporalMethods[ ipb->selectRwMethod ];
        }
    }

    
    // ... other options under construction ...
    
    
    // Select measurement precision mode = f (application defaults, command line options )
    ipb->selectPrecision = xu->optionPrecision;
    
 
    // ... other options under construction ...
   

}

