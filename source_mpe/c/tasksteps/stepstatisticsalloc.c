/*
 *    Memory allocation for benchmark log (statistics) array.
 */

void stepStatisticsAlloc( MPE_INPUT_PARAMETERS_BLOCK* ipb, LIST_RELEASE_RESOURCES* xr )
{
    DWORD64 blockMin = ipb->selectBlockStart;
    DWORD64 blockMax = ipb->selectBlockStop;
    DWORD64 blockStep = ipb->selectBlockDelta;
    if ( blockMin > blockMax )
    {
        blockMin = ipb->selectBlockStop;
        blockMax = ipb->selectBlockStart;
    }
    int stepsCount = ( blockMax - blockMin ) / blockStep + 1;
    int arraySize = stepsCount * sizeof(double);
    xr->bufferStatistics = (double*)malloc(arraySize);   // set pointer to array, arraySize in BYTES
    xr->statisticsCount = stepsCount;                    // set number of elements
    // check memory allocation error
    if ( xr->bufferStatistics == NULL )
    {
        helperRelease ( xr );
        exitWithSystemError( "allocate memory for statistics" );
    }
    // Blank array
    int i = 0;
    double* p = xr->bufferStatistics;
    for( i=0; i<stepsCount; i++ )
    {
        *p = 0.0;
        p++;
    }
}

