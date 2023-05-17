/*

MEMORY PERFORMANCE ENGINE (MPE) FRAMEWORK.
-------------------------------------------
System informationand performance analysing application,
include console version of NCRB(NUMA CPUand RAM Benchmark).
See also memory benchmark projects :
https://github.com/manusov/NCRBv2
https://github.com/manusov/MemoryBenchmarkWindows
Experimental samples :
https://github.com/manusov/Prototyping
Special thanks for C++ lessons :
https://ravesli.com/uroki-cpp/

Class realization for memory and cache benchmark scenario.
TODO.

*/

#include "Benchmark.h"

int Benchmark::enumOp;
SYSTEM_CONTROL_SET* Benchmark::controlSet;
COMMAND_LINE_PARMS* Benchmark::pCmd;

Benchmark::Benchmark(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp)
{
	enumOp = op;
	controlSet = cs;
	pCmd = pp;
}
Benchmark::~Benchmark()
{
	// ... destructor functionality yet reserved ...
}
void Benchmark::execute()
{
	char msg[APPCONST::MAX_TEXT_STRING];

	// Setup variables not changed when executed benchmark scenario.
	// Mode options.
	int opAsm = pCmd->optionAsm;
	int opMemory = pCmd->optionMemory;
	int opThreads = pCmd->optionThreads;
	int opHt = pCmd->optionHt + 1;
	int opLp = pCmd->optionPageSize + 1;
	int opNuma = pCmd->optionNuma + 1;

	// Block size options
	DWORD64 opBlockStart = pCmd->optionBlockStart;
	DWORD64 opBlockStop = pCmd->optionBlockStop;
	DWORD64 opBlockDelta = pCmd->optionBlockDelta;

	// Measurement repeats options
	int opRepeats1 = pCmd->optionRepeats;
	int opRepeats2 = opRepeats1;                 // opRepeats2 = OPTION_NOT_SET if no user override.
	DWORD64 opRepeats3 = pCmd->optionAdaptive;   // opRepeats3 = Adaptive repeat mode, OPTION_NOT_SET if not selected.
	if (opRepeats1 == APPCONST::OPTION_NOT_SET)
	{
		opRepeats1 = APPCONST::APPROXIMATION_REPEATS;      // opRepeats1 = always valid counter
	}

	// Select optimal read/write method = f( options, features ).
	SYSTEM_PROCESSOR_DATA* pCpu = controlSet->pProcessorDetector->getProcessorList();

	// Automatically detect read-write method if not set.
	if ((opAsm == APPCONST::OPTION_NOT_SET) && (opMemory != DRAM))
	{
		opAsm = controlSet->pProcessorDetector->findMaxMethodTemporal(pCpu->cpuBitmap, pCpu->osBitmap);
	}
	else if (opAsm == APPCONST::OPTION_NOT_SET)
	{
		opAsm = controlSet->pProcessorDetector->findMaxMethodNonTemporal(pCpu->cpuBitmap, pCpu->osBitmap);
	}

	// Check read-write method selection
	if ((opAsm > APPCONST::MAXIMUM_ASM_METHOD) || (opAsm < 0))
	{
		snprintf(msg, APPCONST::MAX_TEXT_STRING,
			"\r\nINTERNAL ERROR: wrong read-write method selector (%d).\r\n\r\n", opAsm);
		AppLib::writeColor(msg, APPCONST::ERROR_COLOR);
		return;
	}

	// Check selected read-write method supported by CPUID features.
	DWORD a = opAsm;
	DWORD64 mask = 1;
	if (a > 31)
	{
		mask = (mask << 16) << 16;
		a -= 32;
	}
	mask = mask << a;
	if ((pCpu->cpuBitmap & mask) == 0)
	{
		snprintf(msg, APPCONST::MAX_TEXT_STRING,
			"\r\nERROR: selected read-write method (%d) not supported by CPU instruction set.\r\n\r\n", opAsm);
		AppLib::writeColor(msg, APPCONST::ERROR_COLOR);
		return;
	}

	// Check selected read-write method supported by OS context management features.
	if ((pCpu->cpuBitmap & mask) == 0)
	{
		snprintf(msg, APPCONST::MAX_TEXT_STRING,
			"\r\nERROR: selected read-write method (%d) not supported by OS context management.\r\n\r\n", opAsm);
		AppLib::writeColor(msg, APPCONST::ERROR_COLOR);
		return;
	}
	size_t bytesPerInstruction = controlSet->pProcessorDetector->getBytesPerInstruction(opAsm);
	int bpi = (int)bytesPerInstruction;
	snprintf(msg, APPCONST::MAX_TEXT_STRING, "Asm method ID=%d, bytes per instruction BPI=%d.\r\n", opAsm, bpi);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	double frequencyHz = (double)(pCpu->deltaTsc);
	double periodSeconds = 1.0 / pCpu->deltaTsc;

	// Show frequency in MHz, and period in nanoseconds.
	double frequencyMHz = frequencyHz / 1000000.0;
	double periodNs = periodSeconds * 1000000000.0;
	snprintf(msg, APPCONST::MAX_TEXT_STRING,
		"TSC frequency F=%.3f MHz, period T=%.3f ns.\r\n", frequencyMHz, periodNs);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);

	// Get cache memory configuration
	SYSTEM_TOPOLOGY_DATA* pTopology = controlSet->pTopologyDetector->getTopologyList();
	int kl1 = (int)(pTopology->pointL1 / 1024);
	int kl2 = (int)(pTopology->pointL2 / 1024);
	int kl3 = (int)(pTopology->pointL3 / 1024);
	int kl4 = (int)(pTopology->pointL4 / 1024);
	int kcc = pTopology->coresCount;
	int kht = pTopology->hyperThreadingFlag;
	snprintf(msg, APPCONST::MAX_TEXT_STRING,
		"Cache points for data read/write:\r\nL1=%d KB, L2=%d KB, L3=%d KB, L4=%d KB.\r\nCPU core count=%d, HyperThreading=%d.\r\n",
		kl1, kl2, kl3, kl4, kcc, kht);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);

	// Set memory regions and threads count by selected memory object.
	DWORD64 mSize = 0;
	switch (opMemory)
	{
	case CACHE_L1:
		mSize = pTopology->pointL1;
		if (opRepeats2 == APPCONST::OPTION_NOT_SET) opRepeats1 = APPCONST::APPROXIMATION_REPEATS_L1;
		break;
	case CACHE_L2:
		mSize = pTopology->pointL2;
		if (opRepeats2 == APPCONST::OPTION_NOT_SET) opRepeats1 = APPCONST::APPROXIMATION_REPEATS_L2;
		break;
	case CACHE_L3:
		mSize = pTopology->pointL3;
		if (opRepeats2 == APPCONST::OPTION_NOT_SET) opRepeats1 = APPCONST::APPROXIMATION_REPEATS_L3;
		break;
	case CACHE_L4:
		mSize = pTopology->pointL4;
		if (opRepeats2 == APPCONST::OPTION_NOT_SET) opRepeats1 = APPCONST::APPROXIMATION_REPEATS_L4;
		break;
	case DRAM:
		mSize = APPCONST::CONST_DRAM_BLOCK;
		if (opRepeats2 == APPCONST::OPTION_NOT_SET) opRepeats1 = APPCONST::APPROXIMATION_REPEATS_DRAM;
		break;
	case APPCONST::OPTION_NOT_SET:
	default:
		break;
	}

	// Select reduced repeats mode for measure latency.
	if ((opRepeats2 == APPCONST::OPTION_NOT_SET) &&
		((opAsm == PERF_LATENCY_BUILD_LCM) || (opAsm == PERF_LATENCY_BUILD_LCM_32X2) ||
			(opAsm == PERF_LATENCY_BUILD_RDRAND) || (opAsm == PERF_LATENCY_BUILD_RDRAND_32X2)))
	{
		opRepeats1 /= APPCONST::REPEATS_DIVISOR_LATENCY;
	}

	// Check for automatically set number of threads, if command line parameter not set.
	if (opThreads == APPCONST::OPTION_NOT_SET)
	{
		if ((opMemory == DRAM) &&
			(opAsm != PERF_LATENCY_BUILD_LCM) && (opAsm != PERF_LATENCY_BUILD_LCM_32X2) &&
			(opAsm != PERF_LATENCY_BUILD_RDRAND) && (opAsm != PERF_LATENCY_BUILD_RDRAND_32X2))
		{   // Object=dram, not a latency measurement, means bandwidth measurement.
			opThreads = pTopology->coresCount;
			if ((pTopology->hyperThreadingFlag) && (opHt == HT_USED))
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

	// Check Cache or DRAM detection status.
	if ((mSize == 0) && (opMemory != APPCONST::OPTION_NOT_SET))
	{
		AppLib::writeColor("\r\nERROR: selected memory object not detected, use settings start/end/step.\r\n\r\n", APPCONST::ERROR_COLOR);
		return;
	}
	else if (opMemory != APPCONST::OPTION_NOT_SET)
	{
		opBlockStart = mSize / 8;
		opBlockStop = mSize * 2;
		opBlockDelta = mSize / 8;
	}

	// Check selected paging option available.
	SYSTEM_PAGING_DATA* pPaging = controlSet->pPagingDetector->getPagingList();
	if ((opLp == LP_USED) && (pPaging->largePage == 0))
	{
		AppLib::writeColor("\r\nERROR: large pages not supported by platform.\r\n\r\n", APPCONST::ERROR_COLOR);
		return;
	}
	if ((opLp == LP_USED) && (pPaging->pagingRights == 0))
	{
		AppLib::writeColor("\r\nERROR: no privileges for large pages.\r\n\r\n", APPCONST::ERROR_COLOR);
		return;
	}

	// Start build benchmark scenario.
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
	iv.terminateThread = 0;

	OUTPUT_VARIABLES ov;
	ov.resultDeltaTsc = 0;
	ov.resultInstructions = 0;

	// Benchmark input constants, not changed during measurement iterations.
	ic.startSize = opBlockStart;
	ic.endSize = opBlockStop;
	ic.deltaSize = opBlockDelta;
	ic.maxSizeBytes = opBlockStart;
	if (ic.maxSizeBytes < opBlockStop)
	{
		ic.maxSizeBytes = opBlockStop;
	}
	ic.pageSize = pPaging->defaultPage;
	if (opLp == LP_USED)
	{
		ic.pageSize = pPaging->largePage;
	}
	ic.pagingMode = opLp;
	ic.htMode = opHt;
	ic.numaMode = opNuma;
	ic.threadCount = opThreads;

	// Benchmark input variables, changed during measurement iterations.
	iv.currentSizeInstructions = ic.startSize / bytesPerInstruction;
	iv.currentMeasurementRepeats = opRepeats1;
	iv.currentMethodId = opAsm;
	iv.terminateThread = 0;

	// Build threads list, print allocation,
	// opThreads = threads count, mt = threads list size in bytes, pt = pointer to threads list.
	if ((opThreads > APPCONST::MAXIMUM_THREADS) || (opThreads <= 0))
	{
		snprintf(msg, APPCONST::MAX_TEXT_STRING,
			"\r\nERROR: wrong number of threads (%d), can be 1-%d\r\n\r\n",
			opThreads, APPCONST::MAXIMUM_THREADS);
		AppLib::writeColor(msg, APPCONST::ERROR_COLOR);
		;
	}

	// Start memory allocation.
	AppLib::writeColor("\r\nMemory allocation...\r\n", APPCONST::GROUP_COLOR);

	// Allocation control data.
	SYSTEM_NUMA_DATA* pNuma;
	DWORD64 allThreadsSrcDstMemory = ic.maxSizeBytes * opThreads * 2;
	BOOL opStatus = FALSE;
	BOOL swapFlag = (opNuma == NUMA_REMOTE);

	// Text control data.
	// Select branch for NUMA aware and unaware.
	if ((opNuma == NUMA_LOCAL) || (opNuma == NUMA_REMOTE))
	{
		opStatus = controlSet->pDomainsBuilder->allocateNodesList
		((size_t)allThreadsSrcDstMemory, ic.pagingMode, ic.pageSize, swapFlag);
		pNuma = controlSet->pDomainsBuilder->getNumaList();
		controlSet->pDomainsBuilder->writeReportNuma();
	}
	else
	{
		opStatus = controlSet->pDomainsBuilder->allocateSimpleList
		((size_t)allThreadsSrcDstMemory, ic.pagingMode, ic.pageSize);
		pNuma = controlSet->pDomainsBuilder->getSimpleList();
		controlSet->pDomainsBuilder->writeReportSimple();
	}
	if (!opStatus)
	{
		AppLib::writeColor("\r\nMemory allocation error.\r\n\r\n", APPCONST::ERROR_COLOR);
		return;
	}

	// Start threads allocation.
	AppLib::writeColor("\r\nThreads allocation...\r\n", APPCONST::GROUP_COLOR);

	// Text control data.
	opStatus = controlSet->pThreadsBuilder->buildThreadsList(&ic, &iv, pNuma);
	controlSet->pThreadsBuilder->writeReport();
	if (!opStatus)
	{
		AppLib::writeColor("\r\nThreads allocation error.\r\n\r\n", APPCONST::ERROR_COLOR);
		return;
	}

	// Benchmark scenario, run threads.
	AppLib::writeColor("\r\nRunning threads...", APPCONST::GROUP_COLOR);
	opStatus = controlSet->pThreadsBuilder->runThreads(&ov);
	snprintf(msg, APPCONST::MAX_TEXT_STRING, "\r\nRunned with delta TSC = ");
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, ov.resultDeltaTsc, TRUE);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	AppLib::writeColor(".\r\n", APPCONST::VALUE_COLOR);
	if (!opStatus)
	{
		AppLib::writeColor("Run threads error.\r\n\r\n", APPCONST::ERROR_COLOR);
		return;
	}

	// Benchmark process and calculation variables.
	AppLib::writeColor("Allocate statistics arrays...", APPCONST::VALUE_COLOR);
	double megabytes = 0.0;
	double seconds = 0.0;
	double mbps = 0.0;
	double cpi = 0.0;
	double nspi = 0.0;
	int blockMax = 0;
	int blockCount = 0;
	DWORD64 blockSize = 0;
	DWORD64 blockDelta = 0;

	// Statistic variables for measured memory speed values.
	double resultMin = 0.0;
	double resultMax = 0.0;
	double resultAverage = 0.0;
	double resultMedian = 0.0;

	// Detect measurement cycle length.
	if (ic.startSize <= ic.endSize)
	{
		blockMax = (int)((ic.endSize - ic.startSize) / ic.deltaSize);
	}
	else
	{
		blockMax = (int)((ic.startSize - ic.endSize) / ic.deltaSize);
	}
	blockSize = ic.startSize;
	blockDelta = ic.deltaSize;

	// Build statistics list, print allocation.
	// blockMax = elements count, ma = element size in bytes,
	// pMbps = pointer to statistics array, bandwidth, megabytes per second
	// pNs   = pointer to statistics array, latency, nanoseconds.
	int ma = sizeof(double) * (blockMax + 1);
	double* pMbps = (double*)malloc(ma);
	double* pNs = (double*)malloc(ma);

	// Verify and show statistics arrays allocation.
	if ((pMbps == nullptr) || (pNs == nullptr))
	{
		AppLib::writeColor("FAILED\r\nError at memory allocation for statistics arrays.\r\n\r\n", APPCONST::ERROR_COLOR);
		return;
	}
	else
	{
		// Prepare for text write
		AppLib::writeColor("done.\r\nBandwidth statistics, ", APPCONST::VALUE_COLOR);
		AppLib::storeBaseAndSize(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)pMbps, ma);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		AppLib::writeColor(".\r\nLatency statistics,   ", APPCONST::VALUE_COLOR);
		AppLib::storeBaseAndSize(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)pNs, ma);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		AppLib::writeColor(".", APPCONST::VALUE_COLOR);
	}

	// Blank both statistics arrays.
	double* pMbps1 = pMbps;
	double* pNs1 = pNs;
	for (int i = 0; i < (blockMax + 1); i++)
	{
		*pMbps1 = 0.0;
		*pNs1 = 0.0;
		pMbps1++;
		pNs1++;
	}

	// Prepare for print input conditions.
	if (opRepeats3 == APPCONST::OPTION_NOT_SET)
	{	// Note use 64-bit parameters for printf can crush in the 32-bit mode.
		snprintf(msg, APPCONST::MAX_TEXT_STRING,
			"\r\nRun: method=%d, threads=%d, repeats=%d\r\n     page mode=%d, ",
			opAsm, opThreads, opRepeats1, ic.pagingMode);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	}
	else
	{
		snprintf(msg, APPCONST::MAX_TEXT_STRING, 
			"\r\nRun: method=%d, threads=%d, repeats=ADAPTIVE\r\n     page mode=%d, ",
			opAsm, opThreads, ic.pagingMode);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	}
	AppLib::writeColor("page size=", APPCONST::VALUE_COLOR);
	AppLib::storeCellMemorySize(msg, APPCONST::MAX_TEXT_STRING, ic.pageSize, 1);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);

	// Write start, end, delta, bpi.
	AppLib::writeColor("     start=", APPCONST::VALUE_COLOR);
	AppLib::storeCellMemorySize(msg, APPCONST::MAX_TEXT_STRING, ic.startSize, 1);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	AppLib::writeColor(", end=", APPCONST::VALUE_COLOR);
	AppLib::storeCellMemorySize(msg, APPCONST::MAX_TEXT_STRING, ic.endSize, 1);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	AppLib::writeColor(", delta=", APPCONST::VALUE_COLOR);
	AppLib::storeCellMemorySize(msg, APPCONST::MAX_TEXT_STRING, ic.deltaSize, 1);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	AppLib::writeColor(", bpi=", APPCONST::VALUE_COLOR);
	AppLib::storeCellMemorySize(msg, APPCONST::MAX_TEXT_STRING, bytesPerInstruction, 1);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	AppLib::writeColor(".\r\n", APPCONST::VALUE_COLOR);

	// Calibration
	if ((opRepeats2 == APPCONST::OPTION_NOT_SET) && (opRepeats3 == APPCONST::OPTION_NOT_SET))
	{
		AppLib::writeColor("Calibration...", APPCONST::VALUE_COLOR);
		iv.currentSizeInstructions = ic.maxSizeBytes / bytesPerInstruction;
		controlSet->pThreadsBuilder->updateThreadsList(&iv);
		opStatus = controlSet->pThreadsBuilder->restartThreads(&ov);
		if ((opStatus) &&
			((iv.currentMethodId == PERF_LATENCY_BUILD_LCM) || (iv.currentMethodId == PERF_LATENCY_BUILD_LCM_32X2) ||
				(iv.currentMethodId == PERF_LATENCY_BUILD_RDRAND) || (iv.currentMethodId == PERF_LATENCY_BUILD_RDRAND_32X2)))
		{
			int save = iv.currentMethodId;
			if ((iv.currentMethodId == PERF_LATENCY_BUILD_LCM) || (iv.currentMethodId == PERF_LATENCY_BUILD_RDRAND))
			{
				iv.currentMethodId = PERF_LATENCY_WALK;
			}
			else
			{
				iv.currentMethodId = PERF_LATENCY_WALK_32X2;
			}
			controlSet->pThreadsBuilder->updateThreadsList(&iv);
			controlSet->pThreadsBuilder->restartThreads(&ov);
			iv.currentMethodId = save;
			// controlSet->pThreadsBuilder->restartThreads( &ov );
		}
		if (!opStatus)
		{
			AppLib::writeColor("FAILED.\r\nthread build error.\r\n\r\n", APPCONST::ERROR_COLOR);
			return;
		}
		double cTime = ov.resultDeltaTsc * periodSeconds;
		double cTarget = APPCONST::CALIBRATION_TARGET_TIME;
		double cRepeats = opRepeats1 * (cTarget / cTime);
		opRepeats1 = (int)cRepeats;
		snprintf(msg, APPCONST::MAX_TEXT_STRING,
			"done ( delay=%.3f seconds, update repeats=%d ).\r\n", cTime, opRepeats1);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		iv.currentMeasurementRepeats = opRepeats1;
		controlSet->pThreadsBuilder->updateThreadsList(&iv);
	}

	// Decode read-write method name.
	const char* iname = controlSet->pProcessorDetector->getInstructionString(opAsm);
	snprintf(msg, APPCONST::MAX_TEXT_STRING, "[ %s ]\r\n", iname);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);

	// Print parameters names and table up line
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	AppLib::writeColor(" #     size        CPI        nsPI      MBPS\r\n", APPCONST::TABLE_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);

	// Select units for print measured memory block size.
	int printSizeUnits = PRINT_SIZE_AUTO;
	DWORD64 umx = max(ic.startSize, ic.endSize);
	DWORD64 umn = min(ic.startSize, ic.endSize);
	DWORD64 udt = ic.deltaSize;
	if ((umx < APPCONST::KILO) || (umn % APPCONST::KILO) || (umx % APPCONST::KILO) || (udt % APPCONST::KILO))
	{
		printSizeUnits = PRINT_SIZE_BYTES;
	}
	else if ((umx < APPCONST::MEGA) || (umn % APPCONST::MEGA) || (umx % APPCONST::MEGA) || (udt % APPCONST::MEGA))
	{
		printSizeUnits = PRINT_SIZE_KB;
	}
	else if ((umx < APPCONST::GIGA) || (umn % APPCONST::GIGA) || (umx % APPCONST::GIGA) || (udt % APPCONST::GIGA))
	{
		printSizeUnits = PRINT_SIZE_MB;
	}
	else
	{
		printSizeUnits = PRINT_SIZE_GB;
	}

	// Start measurements cycle for different block sizes, show table: speed = f(size).
	pMbps1 = pMbps;
	pNs1 = pNs;
	for (blockCount = 0; blockCount <= blockMax; blockCount++)
	{
		DWORD64 r3min = 0, r3max = 0, r3value = 0;
		if (opRepeats3 != APPCONST::OPTION_NOT_SET)
		{
			r3min = 1;
			r3max = 0xFFFFFFFFL;
			r3value = opRepeats3 / (blockSize / bytesPerInstruction);  // Approximate constant number of instructions = measurementRepeats * blockSize.
			if (r3value < r3min) r3value = r3min;
			if (r3value > r3max) r3value = r3max;
			iv.currentMeasurementRepeats = r3value;
			controlSet->pThreadsBuilder->updateThreadsList(&iv);
		}

		iv.currentSizeInstructions = blockSize / bytesPerInstruction;
		controlSet->pThreadsBuilder->updateThreadsList(&iv);
		opStatus = controlSet->pThreadsBuilder->restartThreads(&ov);

		if ((opStatus) &&
			((iv.currentMethodId == PERF_LATENCY_BUILD_LCM) || (iv.currentMethodId == PERF_LATENCY_BUILD_LCM_32X2) ||
				(iv.currentMethodId == PERF_LATENCY_BUILD_RDRAND) || (iv.currentMethodId == PERF_LATENCY_BUILD_RDRAND_32X2)))
		{
			int save = iv.currentMethodId;
			if ((iv.currentMethodId == PERF_LATENCY_BUILD_LCM) || (iv.currentMethodId == PERF_LATENCY_BUILD_RDRAND))
			{
				iv.currentMethodId = PERF_LATENCY_WALK;
			}
			else
			{
				iv.currentMethodId = PERF_LATENCY_WALK_32X2;
			}
			controlSet->pThreadsBuilder->updateThreadsList(&iv);
			opStatus = controlSet->pThreadsBuilder->restartThreads(&ov);
			iv.currentMethodId = save;
			controlSet->pThreadsBuilder->updateThreadsList(&iv);
		}

		if (!opStatus)
		{
			AppLib::writeColor("\r\nBenchmark error.\r\n\r\n", APPCONST::ERROR_COLOR);
			return;
		}

		DWORD64 x1 = blockSize;                  // block size, bytes
		DWORD64 x2 = opRepeats1;                 // number of measurement repeats
		if (r3value > 0) x2 = r3value;           // this for adaptive repeats mode
		DWORD64 x3 = opThreads;                  // number of threads
		double x4 = (double)ov.resultDeltaTsc;   // time interval in TSC clocks

		// Clocks per instruction, nanoseconds per instruction, here x3 = number of threads, NOT used for calculation,
		// cpi = x4 / (x1 * x2 * x3 / bytesPerInstruction);  // old variant with x3 used.
		cpi = x4 / (x1 * x2 / bytesPerInstruction);
		// Patched cpi calc.
		nspi = cpi * periodSeconds * 1000000000.0;

		// Megabytes per second, here x3 = number of threads, used for calculation.
		megabytes = x1 * x2 * x3 / 1000000.0;
		seconds = ov.resultDeltaTsc * periodSeconds;
		mbps = megabytes / seconds;

		// Write string with one benchmark iteration results and send parameters to log.
/*
		snprintf(msg, APPCONST::MAX_TEXT_STRING,
			"%4d%12d%8.3f%8.3f   %-10.3f\r\n", blockCount + 1, blockSize, cpi, nspi, mbps);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
*/
		snprintf(msg, APPCONST::MAX_TEXT_STRING, " %-4d  ", blockCount + 1);
		AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
		AppLib::storeCellMemorySizeInt(msg, APPCONST::MAX_TEXT_STRING, blockSize, 12, printSizeUnits);
		AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
		snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-11.3f%-10.3f%-11.3f\r\n", cpi, nspi, mbps);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);

		*pMbps1++ = mbps;
		*pNs1++ = nspi;
		blockSize += blockDelta;
	}

	// Results table down line.
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);

	// Statistics calculate and show, select branches for bandwidth or latency.
	const char* statisticsName = nullptr;
	double* statisticsPointer = nullptr;
	if ((iv.currentMethodId != PERF_LATENCY_BUILD_LCM) && (iv.currentMethodId != PERF_LATENCY_BUILD_LCM_32X2) &&
		(iv.currentMethodId != PERF_LATENCY_BUILD_RDRAND) && (iv.currentMethodId != PERF_LATENCY_BUILD_RDRAND_32X2))
	{
		statisticsName = "Bandwidth statistics ( megabytes per second ):";
		statisticsPointer = pMbps;
	}
	else
	{
		statisticsName = "Latency statistics ( nanoseconds ):";
		statisticsPointer = pNs;
	}

	// Statistics calculate and show, f(selected method).
	std::vector<double> v;
	for (int i = 0; i <= blockMax; i++)
	{
		v.push_back(*statisticsPointer);
		statisticsPointer++;
	}
	AppLib::calculateStatistics(v, resultMin, resultMax, resultAverage, resultMedian);
	
	snprintf(msg, APPCONST::MAX_TEXT_STRING, "%s\r\n", statisticsName);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
	snprintf(msg, APPCONST::MAX_TEXT_STRING,
		"max=%.3f,  min=%.3f,  average=%.3f,  median=%.3f.\r\n",
		resultMax, resultMin, resultAverage, resultMedian);
	AppLib::writeColor(msg, APPCONST::VALUE_COLOR);

	// Delete created objects and termination.
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	AppLib::writeColor("Delete local objects...", APPCONST::TABLE_COLOR);
	free(pMbps);
	free(pNs);

	iv.terminateThread = 1;
	controlSet->pThreadsBuilder->updateThreadsList(&iv);
	controlSet->pThreadsBuilder->restartThreads(&ov, FALSE);
	controlSet->pThreadsBuilder->releaseThreadsList();
	if ((opNuma == NUMA_LOCAL) || (opNuma == NUMA_REMOTE))
	{
		controlSet->pDomainsBuilder->freeNodesList();
	}
	else
	{
		controlSet->pDomainsBuilder->freeSimpleList();
	}

	AppLib::writeColor("done.\r\n", APPCONST::TABLE_COLOR);
}
