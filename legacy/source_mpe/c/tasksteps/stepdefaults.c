/*
 *   Assign default settings to user input data, before command line options detect,
 *   also blank allocated resources list.
 */

void stepDefaults( MPE_USER_INPUT* xi, MPE_CALIBRATION* xc, LIST_RELEASE_RESOURCES* xr )
{
    // default values for options
    xi->optionRwMethod = DEFAULT_RW_METHOD;
    xi->optionRwTarget = DEFAULT_RW_TARGET;
    xi->optionNonTemporal = DEFAULT_RW_ACCESS;
    xi->optionThreadsCount = DEFAULT_THREADS_COUNT;
    xi->optionHyperThreading = DEFAULT_HYPER_THREADING;
    xi->optionPageSize = DEFAULT_PAGE_SIZE;
    xi->optionNuma = DEFAULT_NUMA_MODE;
    xi->optionPrecision = DEFAULT_PRECISION;
    xi->optionMachineReadable = DEFAULT_MACHINEREADABLE;
    xi->optionBlockStart = DEFAULT_MIN_BLOCK;
    xi->optionBlockStop = DEFAULT_MAX_BLOCK;
    xi->optionBlockDelta = DEFAULT_STEP_BLOCK;
    
    // default values for benchmarks measurements: adaptive control
    xc->measurementTime = DEFAULT_MEASUREMENT_TIME;
    xc->measurementApproximation = DEFAULT_MEASUREMENT_APPROXIMATION;
    xc->measurementRepeats = DEFAULT_MEASUREMENT_APPROXIMATION;
    
    // default values for benchmarks measurements: constants
    xc->fastL1 = DEFAULT_FAST_L1;
    xc->fastL2 = DEFAULT_FAST_L2;
    xc->fastL3 = DEFAULT_FAST_L3;
    xc->fastDRAM = DEFAULT_FAST_DRAM;
    xc->fastCustom = DEFAULT_FAST_CUSTOM;
    xc->slowL1 = DEFAULT_SLOW_L1;
    xc->slowL2 = DEFAULT_SLOW_L2;
    xc->slowL3 = DEFAULT_SLOW_L3;
    xc->slowDRAM = DEFAULT_SLOW_DRAM;
    xc->slowCustom = DEFAULT_SLOW_CUSTOM;
    
    // blank values for allocated resources
    xr->bufferTarget = NULL;
    xr->bufferStatistics = NULL;
    xr->nativeLibrary = NULL;
    xr->dllName = NULL;
}

