/*
 *    Execute target operation: benchmark, output results table.
 */

void stepPerformance( LIST_DLL_FUNCTIONS* xf,
                      MPE_PLATFORM_INPUT* xp, 
                      MPE_INPUT_PARAMETERS_BLOCK* ipb, MPE_OUTPUT_PARAMETERS_BLOCK* opb,
                      MPE_CALIBRATION* xc, 
                      LIST_RELEASE_RESOURCES* xr )
{
    SIZE_T blockStart = ipb->selectBlockStart;   // note required 64-bit arithmetic, even in the 32-bit mode
    SIZE_T blockEnd = ipb->selectBlockStop;
    SIZE_T blockStep = ipb->selectBlockDelta;
    SIZE_T blockMax = blockStart;
    if ( blockMax < blockEnd )
    {
        blockMax = blockEnd;
    }
    DWORD rwMethodSelect = ipb->selectRwMethod;
    LPVOID bufferAlignedSrc = ipb->baseSrcAligned;
    LPVOID bufferAlignedDst = ipb->baseDstAligned;

    BOOL status = FALSE;
    DWORDLONG deltaTSC = 0;
    SIZE_T repeatsCount = xc->measurementRepeats;
    SIZE_T instructionsCount = 0;
    int count = 1;
    double cpi = 0.0;
    double nspi = 0.0;
    double megabytes = 0.0;
    double seconds = 1.0 / ( xp->platformTimings.clkTsc );
    double mbps = 0.0;
    double* mbpsStatistics = xr->bufferStatistics;

    CSTR cstrRun[] = {	{ BOLD_COLOR , "\n   #        size   CPI     nsPI    MBPS\n" } , { 0, NULL } };
    colorPrint ( cstrRun );
    lineOfTable( 78 );
    printf( "\n" );

    // pre-heat
    instructionsCount = blockMax / bytesPerInstruction[ rwMethodSelect ];
    
    status = ( xf->DLL_PerformanceGate )
             ( rwMethodSelect ,  bufferAlignedSrc , bufferAlignedDst ,
               instructionsCount , repeatsCount * 3 , &deltaTSC );

    if ( status == 0 )
    {
        exitWithInternalError( "memory target operation failed at pre-heat phase" );
    }

    // measurement cycle with table strings output
    while ( blockStart <= blockEnd )
    {
        instructionsCount = blockStart / bytesPerInstruction[ rwMethodSelect ];
        
        status = ( xf->DLL_PerformanceGate )
                 ( rwMethodSelect ,  bufferAlignedSrc , bufferAlignedDst ,
                   instructionsCount , repeatsCount , &deltaTSC );
        
        if ( status == 0 )
        {
            exitWithInternalError( "memory target operation failed at measurement phase" );
        }
        cpi = deltaTSC;
        
        DWORD64 ic = instructionsCount;
        DWORD64 rc = repeatsCount;
        DWORD64 bs = blockStart;
        
        cpi /= ( ic * rc );
        nspi = cpi * ( xp->platformTimings.nanosecondsTsc );
        mbps = bs * rc;
        mbps /= ( deltaTSC * seconds );
        mbps /= 1000000.0;
        mbpsStatistics[count-1] = mbps;
        printf ( " %3d  %10d   %5.3f   %5.3f   %-10.3f\n" , count , blockStart , cpi , nspi , mbps );
        count++;
        blockStart += blockStep;
    }
    // end of measurement cycle
    
    lineOfTable( 78 );
    printf( "\n" );
}

