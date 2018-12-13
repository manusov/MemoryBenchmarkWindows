/*
 *    Measure and visual CPU TSC frequency.
 */

// Local (not declared at header) helpers

int measureTsc( LIST_DLL_FUNCTIONS* xf, 
                DWORDLONG *tscClk, double *tscHz, double *tscNs )
{
	DWORD errorType = 1;  // This error code if TSC not supported or locked
	BOOL status = 0;
	status = getCpuidFeature( xf, 0x00000001, 0x00000000, EDX, 0x00000010 );
	if ( status != 0 )
	{
		errorType = 2;  // This error code if measurement error
		status = ( xf->DLL_MeasureTsc )( tscClk );
		if ( status != 0 )
		{
			errorType = 0;    // This error code if no errors
			*tscHz = *tscClk;
			*tscNs = *tscHz;
			*tscNs = 1000000000.0 / *tscNs;
		}
	}
	return errorType;
}

void printTsc( double tscHz, double tscNs )
{
	double tscMHz = tscHz / 1000000.0;
	printf( "\nTSC frequency = %.2f MHz, period = %.3f ns", tscMHz, tscNs );
}

// Step action routine

void stepMeasureCpu( LIST_DLL_FUNCTIONS* xf, MPE_CPU_MEASURE* xm, LIST_RELEASE_RESOURCES* xr )
{
    printf( "\nMeasure TSC clock..." );
    int N = 100;
    CHAR p[N+1];
    int status = 0;
    status = measureTsc( xf, &xm->clkTsc, &xm->hzTsc, &xm->nanosecondsTsc );
    if ( status == 1 )
    {
        helperRelease( xr );
        snprintf( p, N, "TSC not supported or locked" );
        exitWithInternalError( p );
    }
    else if ( status == 2 )
    {
        helperRelease( xr );
        snprintf( p, N, "TSC clock measurement failed" );
        exitWithInternalError( p );
    }
    else if ( status != 0 )
    {
        helperRelease( xr );
        snprintf( p, N, "TSC access unknown error" );
        exitWithInternalError( p );
    }
    else
    {
        printTsc(  xm->hzTsc, xm->nanosecondsTsc );
    }
}

