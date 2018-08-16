/*
 *   Memory Performance Engine (MPE) main task routines.
 *   This routine builds platformData, userInput.
 */

void taskAccept( int argc, char** argv )
{
    // Set default parameters
	userInput.optionRwMethod = DEFAULT_RW_METHOD;
    userInput.optionRwTarget = DEFAULT_RW_TARGET;
    userInput.optionNonTemporal = DEFAULT_RW_ACCESS;
    userInput.optionThreadsCount = DEFAULT_THREADS_COUNT;
    userInput.optionHyperThreading = DEFAULT_HYPER_THREADING;
    userInput.optionPageSize = DEFAULT_PAGE_SIZE;
    userInput.optionNuma = DEFAULT_NUMA_MODE;
    userInput.optionPrecision = DEFAULT_PRECISION;
    userInput.optionMachineReadable = DEFAULT_MACHINEREADABLE;
    userInput.optionBlockStart = DEFAULT_MIN_BLOCK;
    userInput.optionBlockStop = DEFAULT_MAX_BLOCK;
    userInput.optionBlockDelta = DEFAULT_STEP_BLOCK;
    // Get and interpreting command line parameters, override defaults
    CSTR cstrCmd[] = {	{ BOLD_COLOR , "\n\nGet command line parameters..." } , { 0, NULL } };
    colorPrint ( cstrCmd );
	regularInput ( argc, argv, command_line_options_list );
	printf( "\n" );
    // Options settings check
    optionCheck ( userInput.optionRwMethod , DEFAULT_RW_METHOD, command_line_options_list[0].name );
	optionCheck ( userInput.optionRwTarget , DEFAULT_RW_TARGET, command_line_options_list[1].name );
	optionCheck ( userInput.optionNonTemporal , DEFAULT_RW_ACCESS, command_line_options_list[2].name );
	optionCheck ( userInput.optionThreadsCount , DEFAULT_THREADS_COUNT, command_line_options_list[3].name );
	optionCheck ( userInput.optionHyperThreading , DEFAULT_HYPER_THREADING, command_line_options_list[4].name );
	optionCheck ( userInput.optionPageSize , DEFAULT_PAGE_SIZE, command_line_options_list[5].name );
	optionCheck ( userInput.optionNuma , DEFAULT_NUMA_MODE, command_line_options_list[6].name );
	optionCheck ( userInput.optionPrecision , DEFAULT_PRECISION, command_line_options_list[7].name );
	optionCheck ( userInput.optionMachineReadable , DEFAULT_MACHINEREADABLE, command_line_options_list[8].name );
	// Initializing application context, include load DLL and functions
    CSTR cstrInit[] = {	{ BOLD_COLOR , "\nInitializing application context..." } , { 0, NULL } };
    colorPrint ( cstrInit );
    arrayCaller( functionsNames, pInitFunctions );
    // Show native support DLL details
    CSTR cstrDll[] = {	{ BOLD_COLOR , "\n\nAccess native DLL..." } , { 0, NULL } };
    colorPrint ( cstrDll );
    showDll ( NULL );
    // Detect platform configuration: native CPU features and available read/write methods
    CSTR cstrCpuid[] = { { BOLD_COLOR , "\n\nGet CPUID parameters and measure TSC clock..." } , { 0, NULL } };
    colorPrint ( cstrCpuid );
    bitmapCpu( NULL, &platformData.platformFeatures );
    measureCpu( NULL, &platformData.platformTimings );
    // Print available read/write methods
    detectMethods( &userInput.optionRwMethod, &andedBitmap, 
                   platformData.platformFeatures.bitmapCpu, platformData.platformFeatures.bitmapOs );
    printMethods( userInput.optionRwMethod, andedBitmap, 
                  platformData.platformFeatures.bitmapCpu, platformData.platformFeatures.bitmapOs,
                  rwMethodsDetails );
    // Detect platform configuration: SMP topology and Caches List by WinAPI
    
    
    // Detect platform configuration: Non-temporal options
    
    
    // Detect platform configuration: Threads management
    
    
    // Detect platform configuration: Hyper-Threading
    
    
    // Detect platform configuration: Paging options
    
    
    // Detect platform configuration: NUMA options
    
    
    
    
   
}

