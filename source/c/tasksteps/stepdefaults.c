/*
 *   Assign default settings to user input data, before command line options detect,
 *   also blank allocated resources list.
 */

void stepDefaults( MPE_USER_INPUT* xi, LIST_RELEASE_RESOURCES* xr )
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
    // blank values for allocated resources
    xr->bufferTarget = NULL;
    xr->bufferStatistics = NULL;
    xr->nativeLibrary = NULL;
    xr->dllName = NULL;
}

