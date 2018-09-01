/*
 *   Operational sequence for call benchmark patterns in the multi-thread mode.
 */

void mtStepPerformance ( MT_DATA* mtd,
                         LIST_DLL_FUNCTIONS* xf, MPE_PLATFORM_INPUT* xp,
                         MPE_INPUT_PARAMETERS_BLOCK* ipb, MPE_OUTPUT_PARAMETERS_BLOCK* opb,
                         MPE_CALIBRATION* xc,  LIST_RELEASE_RESOURCES* xr )
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
    
    threadsRestart( mtd, xf, &deltaTSC );
    threadsRestart( mtd, xf, &deltaTSC );
    threadsRestart( mtd, xf, &deltaTSC );

    // measurement cycle with table strings output
    while ( blockStart <= blockEnd )
    {
        instructionsCount = blockStart / bytesPerInstruction[ rwMethodSelect ];
        THREAD_CONTROL_ENTRY* ePointer = mtd->threadsControl;
        int i = 0;
        for( i=0; i<(mtd->threadsCount); i++ )
        {
            ePointer->sizeBytes = blockStart;
            ePointer->sizeInstructions = instructionsCount;
            ePointer->measurementRepeats = repeatsCount;
            ePointer++; 
        }
        
        threadsRestart( mtd, xf, &deltaTSC );

        cpi = deltaTSC;
        
        DWORD64 ic = instructionsCount;
        DWORD64 rc = repeatsCount;
        DWORD64 bs = blockStart;
        
        cpi /= ( ic * rc );
        nspi = cpi * ( xp->platformTimings.nanosecondsTsc );
        
        mbps = bs * rc * mtd->threadsCount;
        
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


