/*
 *   Memory Performance Engine (MPE) main task routines.
 *   This routine executes benchmark.
 */

void taskExecute( )
{
    BYTE bytesPerInstruction[] = {
    8, 8, 8, 8, 8, 8, 16, 16, 16, 32, 32, 32, 64, 64, 64, 32, 64
    };

    SIZE_T blockStart = taskControl.selectBlockStart;
    SIZE_T blockEnd = taskControl.selectBlockStop;
    SIZE_T blockStep = taskControl.selectBlockDelta;
    SIZE_T blockMax = blockStart;
    if ( blockMax < blockEnd )
    {
        blockMax = blockEnd;
    }
    DWORD rwMethodSelect = taskControl.selectRwMethod;
    LPVOID bufferAlignedSrc = taskControl.baseSrcAligned;
    LPVOID bufferAlignedDst = taskControl.baseDstAligned;

    BOOL status = FALSE;
    DWORDLONG deltaTSC = 0;
    SIZE_T repeatsCount = 2000000;
    SIZE_T instructionsCount = 0;
    int count = 1;
    double cpi = 0.0;
    double nspi = 0.0;
    double megabytes = 0.0;
    double seconds = 1.0 / platformData.platformTimings.dtsc;
    double mbps = 0.0;

    int stepsCount = ( blockEnd - blockStart ) / blockStep + 1;
    mbpsCount = stepsCount;
    int arraySize = stepsCount * sizeof(double);
    mbpsStatistics = (double*)malloc(arraySize);

    printf( "\n" );
    CSTR cstrRun[] = {	{ BOLD_COLOR , "\n   #    size   CPI     nsPI    MBPS\n" } , { 0, NULL } };
    colorPrint ( cstrRun );
    lineOfTable( 78 );
    printf( "\n" );

    // pre-heat
    instructionsCount = blockMax / bytesPerInstruction[ rwMethodSelect ];
    status = DLL_PerformanceGate( rwMethodSelect ,  bufferAlignedSrc , bufferAlignedDst ,
                                  instructionsCount , repeatsCount * 3 , &deltaTSC );
    if ( status == 0 )
    {
        exitWithInternalError( "memory target operation failed at pre-heat phase" );
    }

    // measurement cycle
    while ( blockStart <= blockEnd )
    {
        instructionsCount = blockStart / bytesPerInstruction[ rwMethodSelect ];
        status = DLL_PerformanceGate( rwMethodSelect ,  bufferAlignedSrc , bufferAlignedDst ,
                                      instructionsCount , repeatsCount , &deltaTSC );
        if ( status == 0 )
        {
            exitWithInternalError( "memory target operation failed at measurement phase" );
        }
        cpi = deltaTSC;
        cpi /= ( instructionsCount * repeatsCount );
        nspi = cpi * tscNs;
        mbps = blockStart * repeatsCount;
        mbps /= deltaTSC * seconds;
        mbps /= 1000000.0;
        mbpsStatistics[count-1] = mbps;
        printf ( " %3d  %6d   %5.3f   %5.3f   %-10.3f\n" , count , blockStart , cpi , nspi , mbps );
        count++;
        blockStart += blockStep;
    }
   
    lineOfTable( 78 );
    printf( "\n" );
}


