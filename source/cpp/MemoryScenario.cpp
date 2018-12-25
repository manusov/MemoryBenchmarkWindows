/*
                     MEMORY PERFORMANCE ENGINE FRAMEWORK.
                       Memory Benchmark Scenario class.
    This class make sequence of different initialization end measurement routines.
*/

#include "MemoryScenario.h"

char* MemoryScenario::saveDst;
size_t MemoryScenario::saveMax;
int MemoryScenario::enumOp;
SYSTEM_CONTROL_SET* MemoryScenario::controlSet;
COMMAND_LINE_PARMS* MemoryScenario::pCmd;

MemoryScenario::MemoryScenario( char* pointer, size_t limit, int op, 
								SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp )
{
	saveDst = pointer;
	saveMax = limit;
	enumOp = op;
	controlSet = cs;
	pCmd = pp;
}

MemoryScenario::~MemoryScenario( )
{
	// ... destructor functionality yet reserved ...
}

void MemoryScenario::execute( )
{
	char* dst = NULL;
	size_t max = 0;
	// Setup variables not changed when executed benchmark scenario
	// Mode options
	int opAsm     = pCmd->optionAsm;
	int opMemory  = pCmd->optionMemory;
	int opThreads = pCmd->optionThreads;
	int opHt      = pCmd->optionHt + 1;
	int opLp      = pCmd->optionPageSize + 1;
	int opNuma    = pCmd->optionNuma + 1;
	// Block size options
	DWORD64 opBlockStart = pCmd->optionBlockStart;
	DWORD64 opBlockStop  = pCmd->optionBlockStop;
	DWORD64 opBlockDelta = pCmd->optionBlockDelta;
	// Measurement repeats options
	int opRepeats1 = pCmd->optionRepeats;
	int opRepeats2 = opRepeats1;                 // opRepeats2 = OPTION_NOT_SET if no user override
	DWORD64 opRepeats3 = pCmd->optionAdaptive;   // opRepeats3 = Adaptive repeat mode, OPTION_NOT_SET if not selected
	if ( opRepeats1 == OPTION_NOT_SET )
	{
		opRepeats1 = APPROXIMATION_REPEATS;      // opRepeats1 = always valid counter
	}
	// Select optimal read/write method = f( options, features )
	SYSTEM_PROCESSOR_DATA* pCpu = controlSet->pProcessorDetector->getProcessorList( );
	// Automatically detect read-write method if not set
	if ( ( opAsm == OPTION_NOT_SET ) && ( opMemory != DRAM ) )
	{
		opAsm = controlSet->pProcessorDetector->findMaxMethodTemporal( pCpu->cpuBitmap, pCpu->osBitmap );
	}
	else if ( opAsm == OPTION_NOT_SET )
	{
		opAsm = controlSet->pProcessorDetector->findMaxMethodNonTemporal( pCpu->cpuBitmap, pCpu->osBitmap );
	}
	// Check read-write method selection
	if ( ( opAsm > MAXIMUM_ASM_METHOD ) || ( opAsm < 0 ) )
	{
		snprintf( saveDst, saveMax, "\nINTERNAL ERROR: wrong read-write method selector (%d).\n\n", opAsm );
		AppConsole::transmit( saveDst );
		;
	}
	// Check selected read-write method supported by CPUID features
	DWORD a = opAsm;
	DWORD64 mask = 1;
	if ( a > 31 )
	{
		mask = ( mask << 16 ) << 16;
		a -= 32;
	}
	mask = mask << a;
	if ( ( pCpu->cpuBitmap & mask ) == 0 )
	{
		snprintf( saveDst, saveMax, "\nERROR: selected read-write method (%d) not supported by CPU instruction set.\n\n", opAsm );
		AppConsole::transmit( saveDst );
		;
	}
	// Check selected read-write method supported by OS context management features
	if ( ( pCpu->cpuBitmap & mask ) == 0 )
	{
		snprintf( saveDst, saveMax, "\nERROR: selected read-write method (%d) not supported by OS context management.\n\n", opAsm );
		AppConsole::transmit( saveDst );
		;
	}
	size_t bytesPerInstruction = controlSet->pProcessorDetector->getBytesPerInstruction( opAsm );
	int bpi = bytesPerInstruction;
	snprintf( saveDst, saveMax, "asm method ID=%d, bytes per instruction BPI=%d\n", opAsm, bpi );
	AppConsole::transmit( saveDst );

	double frequencyHz = pCpu->deltaTsc;
	double periodSeconds = 1.0 / pCpu->deltaTsc;
	// Show frequency in MHz, and period in nanoseconds
	double frequencyMHz = frequencyHz / 1000000.0;
	double periodNs = periodSeconds * 1000000000.0;

	snprintf( saveDst, saveMax, "TSC frequency F=%.3f MHz, period T=%.3f ns\n", frequencyMHz, periodNs );
	AppConsole::transmit( saveDst );
	// Get cache memory configuration
	SYSTEM_TOPOLOGY_DATA* pTopology = controlSet->pTopologyDetector->getTopologyList( );
	int kl1 = pTopology->pointL1 / 1024;
	int kl2 = pTopology->pointL2 / 1024;
	int kl3 = pTopology->pointL3 / 1024;
	int kl4 = pTopology->pointL4 / 1024;
	int kcc = pTopology->coresCount;
	int kht = pTopology->hyperThreadingFlag;
    snprintf( saveDst, saveMax, "cache points for data read/write:\nL1=%d KB, L2=%d KB, L3=%d KB, L4=%d KB\nCPU core count=%d, HyperThreading=%d\n",
    		kl1, kl2, kl3, kl4, kcc, kht );
    AppConsole::transmit( saveDst );
	
	// Set memory regions and threads count by selected memory object
	DWORD64 mSize = 0;
	switch ( opMemory )
	{
		case CACHE_L1:
			mSize = pTopology->pointL1;
			if ( opRepeats2 == OPTION_NOT_SET ) opRepeats1 = APPROXIMATION_REPEATS_L1;
			break;
		case CACHE_L2:
			mSize = pTopology->pointL2;
			if ( opRepeats2 == OPTION_NOT_SET ) opRepeats1 = APPROXIMATION_REPEATS_L2;
			break;
		case CACHE_L3:
			mSize = pTopology->pointL3;
			if ( opRepeats2 == OPTION_NOT_SET ) opRepeats1 = APPROXIMATION_REPEATS_L3;
			break;
		case CACHE_L4:
			mSize = pTopology->pointL4;
			if ( opRepeats2 == OPTION_NOT_SET ) opRepeats1 = APPROXIMATION_REPEATS_L4;
			break;
		case DRAM:
			mSize = CONST_DRAM_BLOCK;
			if ( opRepeats2 == OPTION_NOT_SET ) opRepeats1 = APPROXIMATION_REPEATS_DRAM;
			break;
		case OPTION_NOT_SET:
		default:
		break;
	}
 	// Select reduced repeats mode for measure latency
	if ( ( opRepeats2 == OPTION_NOT_SET ) && 
	     ( ( opAsm == CPU_FEATURE_LATENCY_LCM ) || ( opAsm == CPU_FEATURE_LATENCY_RDRAND ) ) )
	{
		opRepeats1 /= REPEATS_DIVISOR_LATENCY;
	}
   	// Check for automatically set number of threads, if command line parameter not set
	if ( opThreads == OPTION_NOT_SET )
	{
		if ( ( opMemory == DRAM ) &&
		     ( opAsm != CPU_FEATURE_LATENCY_LCM ) &&
		     ( opAsm != CPU_FEATURE_LATENCY_RDRAND ) )
		{   // object=dram, not a latency measurement, means bandwidth measurement
			opThreads = pTopology->coresCount;
			if ( ( pTopology->hyperThreadingFlag ) && ( opHt == HT_USED ) )
			{
				opThreads *= 2;
			}
			// pp->optionThreads = n;   // multi-thread by platform topology
		}
		else
		{   // other situations
			// pp->optionThreads = 1;   // single-thread for selected scenario
			opThreads = 1;
		}
	}
	// Check Cache or DRAM detection status
	if ( ( mSize == 0 ) && ( opMemory != OPTION_NOT_SET ) )
	{
		AppConsole::transmit( "\nERROR: selected memory object not detected, use settings start/end/step.\n\n" );
		return;
	}
	else if ( opMemory != OPTION_NOT_SET )
	{
		opBlockStart = mSize / 8;
		opBlockStop  = mSize * 2;
		opBlockDelta = mSize / 8;
	}
	// Check selected paging option available
    SYSTEM_PAGING_DATA* pPaging = controlSet->pPagingDetector->getPagingList( );
	if ( ( opLp == LP_USED ) && ( pPaging->largePage == 0 ) )
    {
    	AppConsole::transmit( "\nERROR: large pages not supported by platform.\n\n" );
    	return;
	}
	if ( ( opLp == LP_USED ) && ( pPaging->pagingRights == 0 ) )
	{
		AppConsole::transmit( "\nERROR: no privileges for large pages.\n\n" );
    	return;
	}
	// start build benchmark scenario
	INPUT_CONSTANTS ic;
	ic.startSize = 0;
	ic.endSize = 0;
	ic.deltaSize = 0;
	ic.maxSizeBytes = 0;  // this for memory allocation
	ic.pageSize = 0;
	ic.pagingMode = 0;
	ic.htMode = 0;
	ic.numaMode = 0;
	ic.threadCount = 0;
	
	INPUT_VARIABLES iv;
	iv.currentMethodId = 0;
	iv.currentSizeInstructions = 0;
	iv.currentMeasurementRepeats = 0;
	
	OUTPUT_VARIABLES ov;
	ov.resultDeltaTsc = 0;
	ov.resultInstructions = 0;
	// benchmark input constants, not changed during measurement iterations
	ic.startSize = opBlockStart;
	ic.endSize = opBlockStop;
	ic.deltaSize = opBlockDelta;
	ic.maxSizeBytes = opBlockStart;
	if ( ic.maxSizeBytes < opBlockStop )
	{
		ic.maxSizeBytes = opBlockStop;
	}
	ic.pageSize = pPaging->defaultPage;
	if ( opLp == LP_USED )
	{
		ic.pageSize = pPaging->largePage;
	}
	ic.pagingMode = opLp;
	ic.htMode = opHt;
	ic.numaMode = opNuma;
	ic.threadCount = opThreads;
	// benchmark input variables, changed during measurement iterations
	iv.currentSizeInstructions = ic.startSize / bytesPerInstruction;
	iv.currentMeasurementRepeats = opRepeats1;
	iv.currentMethodId = opAsm;
	// Build threads list, print allocation
	// opThreads = threads count, mt = threads list size in bytes, pt = pointer to threads list
	if ( ( opThreads > MAXIMUM_THREADS ) || ( opThreads <= 0 ) )
	{
		snprintf( saveDst, saveMax, "\nERROR: wrong number of threads (%d), can be 1-%d\n\n", opThreads, MAXIMUM_THREADS );
		AppConsole::transmit( saveDst );
		;
	}
	// start memory allocation
	AppConsole::transmit( "memory allocation..." );
	// allocation control data
	SYSTEM_NUMA_DATA* pNuma;
	DWORD64 allThreadsSrcDstMemory = ic.maxSizeBytes * opThreads * 2;
	BOOL opStatus = FALSE;
	BOOL swapFlag = ( opNuma == NUMA_REMOTE );
	// text control data
	dst = saveDst;
	max = saveMax;
	// select branch for NUMA aware and unaware
	if ( ( opNuma == NUMA_LOCAL ) || ( opNuma == NUMA_REMOTE ) )
	{
		opStatus = controlSet->pDomainsBuilder->allocateNodesList
			( allThreadsSrcDstMemory, ic.pagingMode, ic.pageSize, swapFlag );
		pNuma = controlSet->pDomainsBuilder->getNumaList( );
		controlSet->pDomainsBuilder->getNumaText( dst, max );
	}
	else
	{
		opStatus = controlSet->pDomainsBuilder->allocateSimpleList
			( allThreadsSrcDstMemory, ic.pagingMode, ic.pageSize );
		pNuma = controlSet->pDomainsBuilder->getSimpleList( );
		controlSet->pDomainsBuilder->getSimpleText( dst, max );
	}
	*dst = 0;  // terminator must be char = 0
	AppConsole::transmit( "done\n" );
	AppConsole::transmit( saveDst );
	if ( !opStatus )
	{
		AppConsole::transmit( "\nmemory allocation error.\n\n" );
		return;
	}
	// start threads allocation
	AppConsole::transmit( "\nthreads allocation..." );
	// text control data
	dst = saveDst;
	max = saveMax;
	opStatus = controlSet->pThreadsBuilder->buildThreadsList( &ic, &iv, pNuma );
	controlSet->pThreadsBuilder->getThreadsText( dst, max );
	AppConsole::transmit( "done\n" );
	AppConsole::transmit( saveDst );
	if ( !opStatus )
	{
		AppConsole::transmit( "\nthreads allocation error.\n\n" );
		;
	}
	// Benchmark scenario, run threads
	AppConsole::transmit( "\nrunning threads..." );
	opStatus = controlSet->pThreadsBuilder->runThreads( &ov );
	dst = saveDst;
	max = saveMax;
	int cnt = 0;
	cnt = snprintf( dst, max, "done ( dTSC=" );
	dst += cnt;
	max -=cnt;
	AppLib::print64( dst, max, ov.resultDeltaTsc, TRUE );
	cnt = snprintf( dst, max, " )\n" );
	dst += cnt;
	*dst = 0;
	AppConsole::transmit( saveDst );
	if ( !opStatus )
	{
		AppConsole::transmit( "\nrun threads error.\n\n" );
		return;
	}
	// Benchmark process and calculation variables
	AppConsole::transmit( "allocate statistics arrays..." );
	double megabytes = 0.0;
	double seconds = 0.0;
	double mbps = 0.0;
	double cpi = 0.0;
	double nspi = 0.0;
	int blockMax = 0;
	int blockCount = 0;
	int blockSize = 0;
	int blockDelta = 0;
	// Statistic variables for measured memory speed values
	double resultMin = 0.0;
	double resultMax = 0.0;
	double resultAverage = 0.0;
	double resultMedian = 0.0;

	// Detect measurement cycle length
	if ( ic.startSize <= ic.endSize )
	{
		blockMax = ( ic.endSize - ic.startSize ) / ic.deltaSize;
	}
	else
	{
		blockMax = ( ic.startSize - ic.endSize ) / ic.deltaSize;
	}
	blockSize = ic.startSize;
	blockDelta = ic.deltaSize;
	// Build statistics list, print allocation
	// blockMax = elements count, ma = element size in bytes,
	// pMbps = pointer to statistics array, bandwidth, megabytes per second
	// pNs   = pointer to statistics array, latency, nanoseconds
	int ma = sizeof( double ) * ( blockMax + 1 );
	double* pMbps = ( double* )malloc( ma );
	double* pNs =   ( double* )malloc( ma );

	// Verify and show statistics arrays allocation	
	if ( ( pMbps == NULL ) || ( pNs == NULL ) )
	{
		AppConsole::transmit( "FAILED\nError at memory allocation for statistics arrays.\n\n" );
		return;
	}
	else
	{
	// Prepare for text write
	dst = saveDst;
	max = saveMax;
	AppLib::printString( dst, max, "done\nbandwidth statistics, " );
	AppLib::printBaseAndSize( dst, max, ( DWORD64 )pMbps, ma );
	AppLib::printString( dst, max, "\nlatency statistics,   " );
	AppLib::printBaseAndSize( dst, max, ( DWORD64 )pNs, ma );
	*dst = 0;
	AppConsole::transmit( saveDst );
	}
	// Blank both statistics arrays
	double* pMbps1 = pMbps;
	double* pNs1 = pNs;
	for( int i=0; i<(blockMax+1); i++ )
	{
		*pMbps1 = 0.0;
		*pNs1 = 0.0;
		pMbps1++; 
		pNs1++;
	}
	
	// Prepare for print input conditions
	dst = saveDst;
	max = saveMax;

	if ( opRepeats3 == OPTION_NOT_SET )
	{	// note use 64-bit parameters for printf can crush in the 32-bit mode
		cnt = snprintf( dst, max, "\nRUN: method=%d, threads=%d, repeats=%d\n     page mode=%d, ", 
	    				opAsm, opThreads, opRepeats1, ic.pagingMode );
	}
	else
	{
		cnt = snprintf( dst, max, "\nRUN: method=%d, threads=%d, repeats=ADAPTIVE\n     page mode=%d, ", 
	        			opAsm, opThreads, ic.pagingMode );
	}
	dst += cnt;
	max -= cnt;
	
	cnt = snprintf( dst, max, "page size=" );
	dst += cnt;
	max -= cnt;
	AppLib::printCellMemorySize( dst, max, ic.pageSize, 1 );
	cnt = snprintf( dst, max, "\n" );
	dst += cnt;
	*dst = 0;
	AppConsole::transmit( saveDst );
	
	// Write start, end, delta, bpi
	dst = saveDst;
	max = saveMax;
	AppLib::printString( dst, max, "     start=" );
	AppLib::printCellMemorySize( dst, max, ic.startSize, 1 );
	AppLib::printString( dst, max, ", end=" );
	AppLib::printCellMemorySize( dst, max, ic.endSize, 1 );
	AppLib::printString( dst, max, ", delta=" );
	AppLib::printCellMemorySize( dst, max, ic.deltaSize, 1 );
	AppLib::printString( dst, max, ", bpi=" );
	AppLib::printCellMemorySize( dst, max, bytesPerInstruction, 1 );
	*dst=0;
	AppConsole::transmit( saveDst );
	AppConsole::transmit( "\n" );
	
	// Calibration
	if ( ( opRepeats2 == OPTION_NOT_SET ) && ( opRepeats3 == OPTION_NOT_SET ) )
	{
		AppConsole::transmit( "calibration..." );
		iv.currentSizeInstructions = ic.maxSizeBytes / bytesPerInstruction;
		controlSet->pThreadsBuilder->updateThreadsList( &iv );
		opStatus = controlSet->pThreadsBuilder->restartThreads( &ov );
		if ( ( opStatus ) && 
		     ( ( iv.currentMethodId == CPU_FEATURE_LATENCY_LCM    ) || 
		       ( iv.currentMethodId == CPU_FEATURE_LATENCY_RDRAND )  )  )
		{
			int a = iv.currentMethodId;
			iv.currentMethodId = CPU_FEATURE_LATENCY_WALK;
			controlSet->pThreadsBuilder->updateThreadsList( &iv );
			controlSet->pThreadsBuilder->restartThreads( &ov );
			iv.currentMethodId = a;
			// controlSet->pThreadsBuilder->restartThreads( &ov );
		}
		if ( !opStatus )
		{
			AppConsole::transmit( "FAILED.\nthread build error.\n\n" );
			return;
		}
		double cTime = ov.resultDeltaTsc * periodSeconds;
		double cTarget = CALIBRATION_TARGET_TIME;
		double cRepeats = opRepeats1 * ( cTarget / cTime );
		opRepeats1 = cRepeats;
		snprintf( saveDst, saveMax, "done ( delay=%.3f seconds, update repeats=%d )\n", cTime, opRepeats1 );
		AppConsole::transmit( saveDst );
		iv.currentMeasurementRepeats = opRepeats1;
		controlSet->pThreadsBuilder->updateThreadsList( &iv );
	}
	// decode read-write method name
	const char* iname = controlSet->pProcessorDetector->getInstructionString( opAsm );
	snprintf( saveDst, saveMax, "\n[ %s ]\n", iname );
	AppConsole::transmit( saveDst );

#define BENCHMARK_TABLE_WIDTH 55
#define STATISTICS_TABLE_WIDTH 78

	// Print parameters names and table up line
	AppConsole::transmit( "\n   #        size   CPI     nsPI    MBPS\n" );
	dst = saveDst;
	max = saveMax;
	AppLib::printLine( dst, max, BENCHMARK_TABLE_WIDTH );
	*dst = 0;
	AppConsole::transmit( saveDst );
	        
	// Start measurements cycle for different block sizes, show table: speed = f(size)
	pMbps1 = pMbps;
	pNs1 = pNs;
	for( blockCount=0; blockCount<=blockMax; blockCount++ )
	{

		DWORD64 r3min=0, r3max=0, r3value=0;
		if ( opRepeats3 != OPTION_NOT_SET )
		{
			r3min = 1;
			r3max = 0xFFFFFFFFL;
			r3value = opRepeats3 / ( blockSize / bytesPerInstruction );       // approximate constant number of instructions = measurementRepeats * blockSize
			if ( r3value < r3min ) r3value = r3min;
			if ( r3value > r3max ) r3value = r3max;
			iv.currentMeasurementRepeats = r3value;
			controlSet->pThreadsBuilder->updateThreadsList( &iv );
		}

		iv.currentSizeInstructions = blockSize / bytesPerInstruction;
		controlSet->pThreadsBuilder->updateThreadsList( &iv );
		opStatus = controlSet->pThreadsBuilder->restartThreads( &ov );
		
		if ( ( opStatus ) && 
		     ( ( iv.currentMethodId == CPU_FEATURE_LATENCY_LCM    ) || 
		       ( iv.currentMethodId == CPU_FEATURE_LATENCY_RDRAND )  )  )
		{
			int a = iv.currentMethodId;
			iv.currentMethodId = CPU_FEATURE_LATENCY_WALK;
			controlSet->pThreadsBuilder->updateThreadsList( &iv );  // ADD 1
			opStatus = controlSet->pThreadsBuilder->restartThreads( &ov );
			iv.currentMethodId = a;
			controlSet->pThreadsBuilder->updateThreadsList( &iv );  // ADD 2
		}
		
		if ( !opStatus )
		{
        	AppConsole::transmit( "\nbenchmark error\n\n" );
			return;
		}
		
		DWORD64 x1 = blockSize;             // block size, bytes
		DWORD64 x2 = opRepeats1;            // number of measurement repeats
		if ( r3value > 0 ) x2 = r3value;    // this for adaptive repeats mode
		DWORD64 x3 = opThreads;             // number of threads
		double x4 = ov.resultDeltaTsc;      // time interval in TSC clocks
		cpi = x4 / ( x1 * x2 * x3 / bytesPerInstruction );
		nspi = cpi * periodSeconds * 1000000000.0;
		
		megabytes = x1 * x2 * x3 / 1000000.0;
		seconds = ov.resultDeltaTsc * periodSeconds;
		mbps = megabytes / seconds;
		
		snprintf ( saveDst, saveMax, "%4d%12d%8.3f%8.3f   %-10.3f\n", blockCount+1, blockSize, cpi, nspi, mbps );
		AppConsole::transmit( saveDst );
		
		*pMbps1++ = mbps;
		*pNs1++ = nspi;
		blockSize += blockDelta;
	}

	// Results table down line
	dst = saveDst;
	max = saveMax;
	AppLib::printLine( dst, max, BENCHMARK_TABLE_WIDTH );
	*dst = 0;
	AppConsole::transmit( saveDst );
	
	// Statistics calculate and show, select branches for bandwidth or latency
	const char* statisticsName = NULL;
	double* statisticsPointer = NULL;
	if ( ( iv.currentMethodId != CPU_FEATURE_LATENCY_LCM    ) &&
	   ( iv.currentMethodId != CPU_FEATURE_LATENCY_RDRAND )  )
	{
		statisticsName = "Bandwidth ( megabytes per second )";
		statisticsPointer = pMbps;
	}
	else
	{
		statisticsName = "Latency ( nanoseconds )";
		statisticsPointer = pNs;
	}

	// Statistics calculate and show, f(selected method)
	AppLib::calculateStatistics( blockMax+1, statisticsPointer, 
								 &resultMin, &resultMax, &resultAverage, &resultMedian );
	dst = saveDst;
	max = saveMax;
	int nchars = 0;
	nchars = snprintf ( dst, max, "\n %s\n", statisticsName );
	dst += nchars;
	max -= nchars;
	AppLib::printLine( dst, max, STATISTICS_TABLE_WIDTH );
	nchars = snprintf( dst, max, " | max=%-10.3f | min=%-10.3f | average=%-10.3f | median=%-10.3f |\n", 
			resultMax, resultMin, resultAverage, resultMedian );
	dst += nchars;
	max -= nchars;
	AppLib::printLine( dst, max, STATISTICS_TABLE_WIDTH );
	*dst = 0;
	AppConsole::transmit( saveDst );
	
	// Delete created objects and termination
	AppConsole::transmit( "\ndelete local objects..." );
	free ( pMbps );
	free ( pNs );
	controlSet->pThreadsBuilder->releaseThreadsList( );
	if ( ( opNuma == NUMA_LOCAL ) || ( opNuma == NUMA_REMOTE ) )
	{
		controlSet->pDomainsBuilder->freeNodesList( );
	}
	else
	{
		controlSet->pDomainsBuilder->freeSimpleList( );
	}
	AppConsole::transmit( "done\n" );

}

