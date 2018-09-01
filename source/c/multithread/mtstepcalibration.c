/*
 *   Operational sequence for call benchmark patterns in the multi-thread mode.
 *   Benchmarks calibration before run.
 */

void mtStepCalibration( MT_DATA* mtd,
                        MPE_CALIBRATION* xc,
                        LIST_DLL_FUNCTIONS* xf, 
                        MPE_PLATFORM_INPUT* xp, MPE_INPUT_PARAMETERS_BLOCK* ipb,
                        LIST_RELEASE_RESOURCES* xr )
{
    // First, calibrate by single-thread and initializing parameters
    stepCalibration( xc, xf, xp, ipb, xr );
    
    // Second, calibrate by multi-thread
    DWORD rwMethodSelect = ipb->selectRwMethod;
    SIZE_T bytesCount = ipb->selectBlockStart;
    if ( bytesCount < ipb->selectBlockStop )
    {
        bytesCount = ipb->selectBlockStop;
    }
    SIZE_T instructionsCount = bytesCount / bytesPerInstruction[ rwMethodSelect ];
    SIZE_T repeatsCount = xc->measurementApproximation;
    // Initialize cycle for threads descriptors list build
    THREAD_CONTROL_ENTRY* ePointer = mtd->threadsControl;
    int n = mtd->threadsCount;
    int i = 0;
    for( i=0; i<n; i++ )
    {
        ePointer->sizeBytes = bytesCount;
        ePointer->sizeInstructions = instructionsCount;
        ePointer->measurementRepeats = xc->measurementApproximation;
        ePointer++;
    }

    if ( ipb->selectPrecision == AUTO_CALIBRATE )
    {
        printf( "Multi-thread auto calibration mode selected...", 
                xc->measurementTime, xc->measurementApproximation );
                
        DWORD64 dtsc = 0;
        threadsRun( mtd, xf, &dtsc );
        threadsRestart( mtd, xf, &dtsc );
        
        // Calculate optimal measurement repeats count
        double seconds = 1.0 / ( xp->platformTimings.clkTsc );           // seconds per one TSC clock
        double tryingSeconds = dtsc * seconds;
        double k = ( xc->measurementTime ) / tryingSeconds;
        double updatedDt = ( xc->measurementApproximation ) * k;
        xc->measurementRepeats = updatedDt;
        // Show results message
        printf( "\nMeasured time=%.4f seconds, repeats updated=%d iterations\n",
               tryingSeconds, xc->measurementRepeats );

    }
    
    for( i=0; i<n; i++ )
    {
        ePointer->measurementRepeats = xc->measurementRepeats;
        ePointer++;
    }
}


  



