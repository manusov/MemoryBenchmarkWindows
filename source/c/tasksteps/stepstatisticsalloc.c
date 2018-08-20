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
    xr->bufferStatistics = (double*)malloc(arraySize);   // set pointer to array
    xr->statisticsCount = stepsCount;                    // set number of elements
}

