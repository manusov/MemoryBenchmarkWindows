/*
 *   Memory Performance Engine (MPE) main task routines.
 *   This routine builds Task Control = f ( platformData, userInput ),
 *   Benchmark scenario parameters depends from system configuration and command line options,
 *   then show benchmark options when ready to start test.
 */

void taskInput( )
{
    // CPU memory read/write method
    DWORD cpuMethod = userInput.optionRwMethod;
    if ( ! methodCheck( cpuMethod, platformData.platformFeatures.bitmapCpu ) )
    {
        exitWithInternalError( "selected read/write method not supported by CPU" );
    }
    if ( ! methodCheck( cpuMethod, platformData.platformFeatures.bitmapOs ) )
    {
        exitWithInternalError( "selected read/write method not supported by OS" );
    }
    taskControl.selectRwMethod = cpuMethod;
    
    // Reserved for other options
    
    // Memory block size (start, stop, delta) options, include buffer allocation
    SIZE_T blockStart = userInput.optionBlockStart;
    SIZE_T blockStop = userInput.optionBlockStop;
    SIZE_T blockDelta = userInput.optionBlockDelta;
    // ... reserved for verify this parameters ...
    // Memory buffer allocation
    SIZE_T blockMax = userInput.optionBlockStart;
    SIZE_T bufferAlignment = 4096;  // BUG: replace constant 4096 to variable
    if ( blockMax < userInput.optionBlockStop )
    {
        blockMax = userInput.optionBlockStop;
    }
    SIZE_T bufferSize = blockMax;
    bufferSize *= 2;                 // because copy block required source and destination
    bufferSize += bufferAlignment;   // because alignment
    allocateBuffer( bufferSize, bufferAlignment, blockMax, 
                    &taskControl.baseOriginal, 
                    &taskControl.baseSrcAligned, 
                    &taskControl.baseDstAligned );
    // Setup benchmark parameters
    taskControl.selectBlockStart = blockStart;
    taskControl.selectBlockStop = blockStop;
    taskControl.selectBlockDelta = blockDelta;
    
    
     
    
    // Show benchmark parameters = f(options), ready to start test, wait user input
    CSTR cstrSparm[] = { { BOLD_COLOR , "\n\nReady to start with parameters:" } , { 0, NULL } };
    colorPrint ( cstrSparm );
    regularOutput( test_parameters_print_list );
    // Wait user input
   	char c = inputChar( "\nStart (y/n) ? ", inputBuffer, 1 );
   	// Handling user input
	printf( "\n" );
	c = tolower( c );
	if ( c != 'y' )
	{
        // Memory buffer de-allocation and exit
        releaseBuffer ( taskControl.baseOriginal );
		ExitProcess( 3 );
	} 

}

// Helpers

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
    return ( bitMap & mask != 0 );
}



