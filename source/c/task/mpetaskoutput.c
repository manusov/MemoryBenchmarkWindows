/*
 *   Memory Performance Engine (MPE) main task routines.
 *   This routine interpreting and show benchmark output parameters, results.
 */

void taskOutput( )
{
        // Analysing benchmark results, output parameters = f ( benchmark results )
   	calculateStatistics( mbpsCount-1 , mbpsStatistics , 
                         &userOutput.minMbps,
                         &userOutput.maxMbps,
                         &userOutput.averageMbps,
                         &userOutput.medianMbps );
    // Output test result parameters summary
	printf( "\nMBPS max=%.2f , min=%.2f , average=%.2f , median=%.2f\n",
                         userOutput.maxMbps,
                         userOutput.minMbps,
                         userOutput.averageMbps,
                         userOutput.medianMbps );
    // De-initializing application context, system restore, resources release
    CSTR cstrDeinit[] = {	{ BOLD_COLOR , "\nDe-initializing application context..." } , { 0, NULL } };
    colorPrint ( cstrDeinit );
    // Memory buffer de-allocation
    releaseBuffer ( taskControl.baseOriginal );
    arrayCaller( functionsNames, pDeinitFunctions );
    // Release memory, log array
    free( mbpsStatistics );
}
