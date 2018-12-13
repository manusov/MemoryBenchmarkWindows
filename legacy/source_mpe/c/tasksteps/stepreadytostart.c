/*
 *  Show message about ready to start and parameters list
 */

void stepReadyToStart( PRINT_ENTRY parmList[], LIST_RELEASE_RESOURCES* xr )
{
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
