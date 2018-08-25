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
    switch ( ipb->selectRwTarget )
    {
        case L1_CACHE:
            blkMax = xp->platformCache.pointL1;
            if ( blkMax == 0 ) blkMax = AUTO_L1;
            blkCount = OPTIMAL_COUNT_L1;
            break;
        case L2_CACHE:
            blkMax = xp->platformCache.pointL2;
            if ( blkMax == 0 ) blkMax = AUTO_L2;
            blkCount = OPTIMAL_COUNT_L2;
            break;
        case L3_CACHE:
            blkMax = xp->platformCache.pointL3;
            if ( blkMax == 0 ) blkMax = AUTO_L3;
            blkCount = OPTIMAL_COUNT_L3;
            break;
        case L4_CACHE:
        case DRAM:
            blkMax = AUTO_DRAM;
            blkCount = OPTIMAL_COUNT_DRAM;
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

    
    // ... other options under construction ...
    
    
    // Select measurement precision mode = f (application defaults, command line options )
    ipb->selectPrecision = xu->optionPrecision;
    
 
    // ... other options under construction ...
    

    // Print parameters before benchmarks run
    CSTR cstrSparm[] = { { BOLD_COLOR , "\n\nReady to start with parameters:" } , { 0, NULL } };
    colorPrint ( cstrSparm );
    regularOutput( parmList );
    // Wait user input
   	char c = inputChar( "\nStart (y/n) ? ", inputBuffer, 1 );
   	// Handling user input
	printf( "\n" );
	c = tolower( c );
	if ( c != 'y' )
	{
        // Memory buffer de-allocation and exit
        helperRelease( xr );
		ExitProcess( 3 );
	} 
}

