/*
 *    Interpreting results of target operation: benchmark.
 */

void stepInterpretingOpb( MPE_OUTPUT_PARAMETERS_BLOCK* opb, MPE_USER_OUTPUT* uout,
                          LIST_RELEASE_RESOURCES* xr )
{
    // Assign local variables values
    int mbpsCount = xr->statisticsCount;
    double* mbpsStatistics = xr->bufferStatistics;
    double minMbps = 0;
    double maxMbps = 0;
    double averageMbps = 0;
    double medianMbps = 0;
    
    // Analysing benchmark results, output parameters = f ( benchmark results )
   	calculateStatistics( mbpsCount-1 , mbpsStatistics , 
                         &minMbps,
                         &maxMbps,
                         &averageMbps,
                         &medianMbps );
    // Output test result parameters summary
	printf( "\nMBPS max=%.2f , min=%.2f , average=%.2f , median=%.2f",
                         maxMbps,
                         minMbps,
                         averageMbps,
                         medianMbps );
                         
    // Build user output block ( redundant or required for hit/miss additional statistics ? )
    uout->maxMBps = maxMbps;
    uout->minMBps = minMbps;
    uout->averageMBps = averageMbps;
    uout->medianMBps = medianMbps;
}

