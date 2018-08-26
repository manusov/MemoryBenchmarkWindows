/*
 *    Benchmarks calibration, adjust measurement repeats count.
 */

void stepCalibration( MPE_CALIBRATION* xc,
                      LIST_DLL_FUNCTIONS* xf,  
                      MPE_PLATFORM_INPUT* xp, MPE_INPUT_PARAMETERS_BLOCK* ipb, 
                      LIST_RELEASE_RESOURCES* xr )
{
    // Show begin message
    DWORD32 xSlow = xc->measurementApproximation;
    DWORD32 xFast = xc->measurementApproximation;
    switch ( ipb->selectRwTarget )
    {
        case L1_CACHE:
            xSlow = xc->slowL1;
            xFast = xc->fastL1;
            break;
        case L2_CACHE:
            xSlow = xc->slowL2;
            xFast = xc->fastL2;
            break;
        case L3_CACHE:
            xSlow = xc->slowL3;
            xFast = xc->fastL3;
            break;
        case L4_CACHE:
        case DRAM:
            xSlow = xc->slowDRAM;
            xFast = xc->fastDRAM;
            break;
        case USER_DEFINED_TARGET:
            xSlow = xc->slowCustom;
            xFast = xc->fastCustom;
            break;
        default:
            break;
    }
    xc->measurementApproximation = xFast;
    
    if ( ipb->selectPrecision == AUTO_CALIBRATE )
    {
        printf( "\nTarget time=%.1f seconds, repeats approximation=%d iterations\nAuto calibration mode selected...", 
                xc->measurementTime, xc->measurementApproximation );
        double seconds = 1.0 / ( xp->platformTimings.clkTsc );           // seconds per one TSC clock
        double tryingSeconds = 0.0;
        // Local variables
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
        SIZE_T instructionsCount = blockMax / bytesPerInstruction[ rwMethodSelect ];
        SIZE_T repeatsCount = xc->measurementApproximation;

        // Measurement for calibration
        status = ( xf->DLL_PerformanceGate )
                 ( rwMethodSelect ,  bufferAlignedSrc , bufferAlignedDst ,
                   instructionsCount , repeatsCount , &deltaTSC );

        // Analysing calibration results
        if ( status == 0 )
        {
            helperRelease ( xr );
            exitWithInternalError( "memory target operation failed at calibration phase" );
        }
        // Calculate optimal measurement repeats count
        tryingSeconds = deltaTSC * seconds;
        double k = ( xc->measurementTime ) / tryingSeconds;
        double updatedDt = ( xc->measurementApproximation ) * k;
        xc->measurementRepeats = updatedDt;
        // Show results message
        printf( "\nMeasured time=%.4f seconds, repeats updated=%d iterations\n",
               tryingSeconds, xc->measurementRepeats );
    }
    else
    {
        if ( ipb->selectPrecision == SLOW )
        {
            xc->measurementRepeats = xSlow;
        }
        else
        {
            xc->measurementRepeats = xFast;
        }
        
        printf( "\nRepeats updated to default constant=%d\n", xc->measurementRepeats );
    }
}

