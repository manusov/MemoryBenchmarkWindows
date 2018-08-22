/*
 *    Build input parameters block = f ( user input, platform data ).
 */

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
    // CPU memory read/write method
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
    
    // DEBUG ASM METHODS
    // ipb->selectRwMethod = 8; // DEBUG
    // DEBUG ASM METHODS

    // ... oother options under construction ...

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

