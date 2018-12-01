/* 
UNDER CONSTRUCTION.

TODO:

1) + Support Local and Remote modes for NUMA, different affinity masks
2) Start, End, Step set manually with memory=object (non default). But can without memory=object size only.
3) Big files (>2GB overflow) and mass storage. Stride select. MBPS and IOPS.
4) Some small reoredring, members swap between classes, functional regularity.
5) More debug information, make framework.

---
+ Required sequental KAFFINITY, Group Number, yet interleaved at THREAD_CONTROL_ENTRY struct.
+ Required memory allocation for NUMA nodes list.
+ Required functions control set, NUMA_CONTROL_SET struct.
+ Required memory free after allocation: for NUMA and non-NUMA branches.
+ NUMA branch must be conditional, by user option.
+ Required Large Pages support at NUMA mode. Make enum at GlobalDefinitions.h.
+ Required regular alignment support for NUMA and non-NUMA.
---
+ Modify Performer/NumaTopology: Affinity masks for NUMA.
+ Modify Performer/NumaTopology: override memory allocation by NUMA-allocation result: 
+ for this allocation option: Performer = disable allocation , NumaTopology = enable allocation.
+ Modify Performer/NumaTopology: Affinity masks for HT.
+ Summary, see above, required HT and NUMA support, affinity masks = f (user settings, platform configuration).
---
Group-Aware HT control supported only if NUMA option = NUMA_LOCAL or NUMA_REMOTE.
Some functions, used in this control list, previously used ! Refactor for single loader of functions or remove redundant.
---
Memory Performance Engine.
Debug sample, without technologies support pre-check.
TODO features:
 1)  + Crush when Nthreads > 1. Reenterancy ?
 2)  + Slow when Nthreads > Ncpu, because waiting events clear, use separate events for set-clear.
 3)  + Refactoring for optimize SIZE, SSIZE overlows prevent, minimize number of transit variables.
 4)  + Set number of measurement iterations as direct parameter, no re-encodings.
 5)  + When integration: check TSC supported.
 6)  + When integration: check CPU selected instruction set supported by CPU instruction set and OS context management.
 7)  + Auto set max method.
 8)  + Add and debug class: DetectorMemory (SystemMemory), for system information.
 9)  + Add and debug class: DetectorTopology (SystemTopology), for Cache and RAM, target objects.
 	 + count number of CPU cores at this function, print before HT.
     + option memory=value, include block size, number of threads and temporal mode
     + option threads=value, auto for multithread by topology info (if -1) ?
 10) + Auto select repeats, use pre-defined constants and calibration.
     + repeats override logic.
 11) + Add and debug class: DetectorPaging (PagingOptions), for Large pages. Add option.
     + required size alignment by 2/4 MB.
     + show paging option state when start.
 12) + Improve command line errors reporting: required report both option name and value.
 13) + Detail error reporting by OS API. Debug error handling branches.
     + add decode subroutine.
     + replace variable "status" to "errorCode", replace interpreting.
 ---
 14) + No divide to LATENCY_DIVISOR if direct set repeats.
 15) Negative memory size if selected >2GB (arithmetic overflow), example "end=2G". blockSize / bpi overflow. Yet 32-bit limit.
 ---
 16) ( ? ) Output coloring.
 17) Show all options values and wait user press key (y/n).
 ---
 18) Add and debug class: DetectorNuma (NumaOptions), for NUMA affinitization. Add option.
     - required platform for verify this
 ---
 19) Show user help.
 20) Scripting.
 21) Fast read-write-copy option or "read", "write", "copy", "ntread", "ntwrite", "ntcopy" for existed option.
---  
 TODO bugs fix:
 1) BUG Threads count manual set with memory=dram.
 2) Correct sequence.
 3) Correct data types, example size_t use.
 4) Check for overflows when measurement.
 5) Check for BOOL return error, but GetLastError( ) returns 0
 6) Random offsets JA or JBE, wrong limit -1. For latency measure, both for x64 and ia32 DLLs.
 ---
 TODO optimization.
 1) Subroutine alignByFactor replicated.
 2) Redesign per-thread entry, make pointer to part, common for all threads, routines pointers for example.
 3) 2) Redesign per-thread entry, reduce width of some parameters.
 4) Common reordering required.
 ---
 TODO strategy:
 1) + Measure latency.
 2) Scripting data reports model.
    - auto bandwidth=f(size), for all memory types.
    - auto latency=f(size), for all memory types.
 3) Refactoring by defined scripting data model. 

REMEMBER IMPORTANT DEBUG DUMP AT THREAD ROUTINE.

*/


#include <cstdio>
#include <windows.h>
#include "GlobalDeclarations.h"
#include "CommandLine.h"
#include "SystemLibrary.h"
#include "DecoderCpuid.h"
#include "SystemMemory.h"
#include "SystemTopology.h"
#include "NumaTopology.h"
#include "PagingOptions.h"
#include "Performer.h"

// Title and service strings include copyright
const char* stringTitle1 = "Memory Performance Engine.";
const char* stringTitle2 = BUILD_STRING;
const char* stringTitle3 = "(C)2018 IC Book Labs.";
const char* stringAnyKey = "Press any key...";

// Objects (pointers to classes), used for benchmarks, control and status
CommandLine*    pCommandLine    = NULL;
SystemLibrary*  pSystemLibrary  = NULL;
DecoderCpuid*   pDecoderCpuid   = NULL;
SystemMemory*   pSystemMemory   = NULL;
SystemTopology* pSystemTopology = NULL;
NumaTopology*   pNumaTopology   = NULL;
PagingOptions*  pPagingOptions  = NULL;
Performer*      pPerformer      = NULL;

// Variables and structures
COMMAND_LINE_PARMS* pp = NULL;
SYSTEM_FUNCTIONS_LIST* psfl = NULL;
NUMA_CONTROL_SET* pncs = NULL;
BENCHMARK_SCENARIO scenario;

// Status variables
BOOL opStatus;
DWORD osErrorCode;

// CPUID/XGETBW/RDTSC variables
DWORD rEax;
DWORD rEbx;
DWORD rEcx;
DWORD rEdx;
DWORD64 cpuBitMap;
DWORD64 osBitMap;
DWORD64 deltaTsc;
double frequencyHz;
double periodSeconds;

// Memory configuration variables
SYSTEM_MEMORY_DATA sysMemory;

// System Topology and cache levels (by WinAPI) configuration variables
SYSTEM_TOPOLOGY_DATA sysTopology;

// Paging information data
PAGING_OPTIONS_DATA pagingOptions;

// Benchmark process and calculation variables
double megabytes;
double seconds;
double mbps;
double cpi;
double nspi;
int blockMax;
int blockCount;
int blockSize;
int blockDelta;

// Statistic variables for measured memory speed values
double min;
double max;
double average;
double median;

// Values Bytes Per Instruction for native methods
// This table associated with native DLL procedures
// Arrays length must be constant = MAXIMUM_ASM_METHOD
#if NATIVE_WIDTH == 32
BYTE bytesPerInstruction[] = 
{
     4,  4,  4,  4,  4,  4, 16, 16, 16, 32, 32, 32, 64, 64, 64, 32, 64,
    16, 16, 32, 32, 64, 64, 16, 16, 32, 32, 64, 64, 16, 16, 32,
     8,  8
};
#endif
#if NATIVE_WIDTH == 64
BYTE bytesPerInstruction[] = 
{
     8,  8,  8,  8,  8,  8, 16, 16, 16, 32, 32, 32, 64, 64, 64, 32, 64,
    16, 16, 32, 32, 64, 64, 16, 16, 32, 32, 64, 64, 16, 16, 32,
     8,  8
};
#endif

// Helper methods declarations
int print64( char* s, size_t n, DWORD64 x );
int printBaseAndSize( char* s, size_t n, DWORD64 base, DWORD64 size );
int printMemorySize( char* s, size_t n, DWORD64 x );
void printLine( char* s, size_t n, int m );
void calculateStatistics( int length , double results[] , 
                          double* min , double* max , double* average , double* median );
void printSystemError( DWORD osErrorCode );

// Print hex value = x to output string = s with size limit = n
int print64( char* s, size_t n, DWORD64 x )
{
	DWORD32 xLow = x;
	DWORD32 xHigh = ( x >> 16 ) >> 16;
	int m = snprintf( s, n, "%08X%08Xh", xHigh, xLow );
	return m;
}

// Print base and size hex values to output string = s with size limit = n
int printBaseAndSize( char* s, size_t n, DWORD64 base, DWORD64 size )
{
	int m = 0;
	int msum = 0;
	m = snprintf( s, n, "base=" );
	s += m;
	n -= m;
	msum += m;
	m = print64( s, n, base );
	s += m;
	n -= m;
	msum += m;
	m = snprintf( s, n, ", size=" );
	s += m;
	n -= m;
	msum += m;
	m = print64( s, n, size );
	s += m;
	n -= m;
	msum += m;
	return msum;
}

// Print memory size: bytes/KB/MB/GB, overloaded
#define KILO 1024
#define MEGA 1024*1024
#define GIGA 1024*1024*1024
int printMemorySize( char* s, size_t n, DWORD64 memsize )
{
    double xd = memsize;
    int nchars = 0;
    if ( memsize < KILO )
    {
        int xi = memsize;
        nchars = snprintf( s, n, "%d bytes", xi );
    }
    else if ( memsize < MEGA )
    {
        xd /= KILO;
        nchars = snprintf( s, n, "%.2lfKB", xd );
    }
    else if ( memsize < GIGA )
    {
        xd /= MEGA;
        nchars = snprintf( s, n, "%.2lfMB", xd );
    }
    else
    {
        xd /= GIGA;
        nchars = snprintf( s, n, "%.2lfGB", xd );
    }
    return nchars;
}

// Print horizontal line by m "-" chars to output string = s with size limit = n
void printLine( char* s, size_t n, int m )
{
	int i;
	int j;
	for( i=0; i<m; i++ )
	{
		j = snprintf( s, n, "-" );
		s += j;
		n -= j;
	}
}

// Calculate statistics: min, max, average, median for array results with elements count = length
void calculateStatistics( int length , double results[] , 
                          double* min , double* max , double* average , double* median )
{
    int i;
    BOOLEAN f;
    double temp;
    *min = results[0];
    *max = results[0];
    for ( i=0; i<length; i++ )
    {
        if ( results[i] < *min )
        {
            *min = results[i];
        }

        if ( results[i] > *max )
        {
            *max = results[i];
       }
    }

    for ( i=0; i<length; i++ )
    {
        *average += results[i];
    }
    *average /= length;

    i = 0;
    do {
    f = FALSE;
        for ( i=0; i<(length-1); i++ )
        {
            if ( results[i] > results[i+1] )
            {
            temp = results[i];
            results[i] = results[i+1];
            results[i+1] = temp;
            f = TRUE;
            }
        }
    } while (f);

    int j = length / 2;
    if ( (length %2) == 0 )
    {
        *median = ( results[j-1] + results[j] ) / 2.0;  // average of middle pair
    }
    else
    {
        *median = results[j];  // middle or single element
    }
}

// Helper for print decoded OS error code
void printSystemError( DWORD x )
{
	if ( ( x > 0 ) && ( x < 0x7FFFFFFF ) )  // reject 0 = no OS errors, -1 = error without OS API code
	{
		// Local variables
    	LPVOID lpvMessageBuffer;
    	DWORD status;
    	// Build message string = f (error code)
    	status = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
        	          		    FORMAT_MESSAGE_FROM_SYSTEM |
            	      		    FORMAT_MESSAGE_IGNORE_INSERTS,
                	  		    NULL, x,
                  			    MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                  			    ( LPTSTR )&lpvMessageBuffer, 0, NULL );
    	if ( status )
    	{
        	// this visualized if error string build OK
        	printf( "OS API reports error %d = %s\n", x, lpvMessageBuffer );
    	}
    	else
    	{
        	DWORD dwError = GetLastError( );
        	// this visualized if build error string FAILED
        	printf( "Decode OS error failed, format message error %d\n", dwError );
    	}
    	// Free the buffer allocated by the system API function
    	LocalFree( lpvMessageBuffer );
	}
}

// Application entry point
int main(int argc, char** argv) 
{
	// Show title
	printf( "\n%s %s %s\n", stringTitle1, stringTitle2, stringTitle3 );
	
	// Buffer strings
	const int NS = 80;
	char s[NS+1];
	char* statusString;
	
	// Get command line parameters
	printf( "get command line parameters..." );
	pCommandLine = new CommandLine( );
	pCommandLine->resetBeforeParse( );
	osErrorCode = pCommandLine->parseCommandLine( argc, argv );
	if ( osErrorCode )
	{
		printf( "FAILED.\n" );
		statusString = pCommandLine->getStatusString( );
        printf( "ERROR: %s\n", statusString );
        printSystemError( osErrorCode );
		return 1;
	}
	printf( "OK.\n" );
	pCommandLine->correctAfterParse( );
	pp = pCommandLine->getCommandLineParms( );

	// Load library, check validity
	printf( "load library..." );
	pSystemLibrary = new SystemLibrary( );
	osErrorCode = pSystemLibrary->loadSystemLibrary( );
	if ( osErrorCode )
	{
		printf( "FAILED.\n" );
		statusString = pSystemLibrary->getStatusString( );
        printf( "Error at %s\n", statusString );
        printSystemError( osErrorCode );
		return 1;
	}
	printf( "OK.\n" );
	psfl = pSystemLibrary->getSystemFunctionsList( );
	
	// Get library info, show string
	char *dllProduct, *dllVersion, *dllVendor;
	psfl->DLL_GetDllStrings( &dllProduct, &dllVersion, &dllVendor );
	printf( "%s %s %s\n", dllProduct, dllVersion, dllVendor );
	
	// Setup variables not changed when executed benchmark scenario
	int a = pp->optionAsm;
	int m = pp->optionMemory;
	int n = pp->optionThreads;
	int h = pp->optionHt;
	int u = pp->optionNuma;
	
	int r1 = pp->optionRepeats;
	int r2 = r1;                        // r2 = OPTION_NOT_SET if no user override
	DWORD64 r3 = pp->optionAdaptive;    // r3 = Adaptive repeat mode, OPTION_NOT_SET if not selected
	if ( r1 == OPTION_NOT_SET )
	{
		r1 = APPROXIMATION_REPEATS;     // r1 = always valid counter
	}
	
	int b = pp->optionBlockStop;
	if ( b < pp->optionBlockStart )
	{
		b = pp->optionBlockStart;
	}
	
	// Create DecoderCpuid class
	pDecoderCpuid = new DecoderCpuid( psfl );
	// Create SystemMemory class
	pSystemMemory = new SystemMemory( );
	// Create SystemTopology class
	pSystemTopology = new SystemTopology( );
	// Create NumaTopology class
	pNumaTopology = new NumaTopology( );
	// Create PagingOptions class
	pPagingOptions = new PagingOptions( );
	
	// Check CPUID support
	printf( "check CPUID support..." );
	opStatus = ( psfl->DLL_CheckCpuid )( );
	if ( !opStatus )
	{
		printf( "FAILED.\nCPUID instruction not supported or locked by VMM.\n" );
		return 1;
	}
	printf( "OK.\n" );
	
	// Detect CPU vendor and model
	char sVendor[13], sModel[49];
	opStatus = pDecoderCpuid->getVendorAndModel( sVendor, sModel, 13, 49 );
	if ( !opStatus )
	{
		printf( "Get CPU vendor and model FAILED.\n" );
	}
	else
	{
		printf( "CPU vendor = %s\nCPU model  = %s\n", sVendor, sModel );
	}
	
	// Check TSC (Time Stamp Counter) support
	printf( "check RDTSC support..." );
	( psfl->DLL_ExecuteCpuid )( 0, 0, &rEax, &rEbx, &rEcx, &rEdx );  // CPUID function=0, subfunction=n/a
	rEdx = 0;
	if ( rEax >= 1 )
	{
		( psfl->DLL_ExecuteCpuid )( 1, 0, &rEax, &rEbx, &rEcx, &rEdx );  // CPUID function=1, subfunction=n/a
	}
	if ( ( rEdx & 0x00000010 ) == 0 )
	{
		printf( "FAILED.\nRDTSC instruction not supported or locked by VMM.\n" );
		return 1;
	}
	printf( "OK.\n" );
	
	// Get platform features bitmaps
	printf( "check platform features..." );
	cpuBitMap = 0;
	osBitMap = 0;
	pDecoderCpuid->buildCpuBitmap( cpuBitMap );
	pDecoderCpuid->buldOsBitmap( osBitMap );
	char sCpuBits[81], sOsBits[81];
	print64( sCpuBits, 80, cpuBitMap );
	print64( sOsBits, 80 , osBitMap );
	printf( "OK.\nCPU bitmap=%s , OS bitmap=%s\n", sCpuBits, sOsBits );
	
	// Automatically detect read-write method if not set
	if ( ( a == OPTION_NOT_SET ) && ( m != DRAM ) )
	{
		a = pDecoderCpuid->findMaxMethodTemporal( cpuBitMap, osBitMap );
	}
	else if ( a == OPTION_NOT_SET )
	{
		a = pDecoderCpuid->findMaxMethodNonTemporal( cpuBitMap, osBitMap );
	}

	// Check read-write method selection
	if ( ( a > MAXIMUM_ASM_METHOD ) || ( a < 0 ) )
	{
		printf( "\nInternal error: wrong read-write method selector (%d).\n", a );
		return 1;
	}

	// Check selected read-write method supported by CPUID features
	DWORD a1 = a;
	DWORD64 mask = 1;
	if ( a1 > 31 )
	{
		mask = ( mask << 16 ) << 16;
		a1 -= 32;
	}
	mask = mask << a1;
	if ( ( cpuBitMap & mask ) == 0 )
	{
		printf( "ERROR: selected read-write method not supported by CPU instruction set.\n" );
		return 1;
	}

	// Check selected read-write method supported by OS context management features
	if ( ( osBitMap & mask ) == 0 )
	{
		printf( "ERROR: selected read-write method not supported by OS context management.\n" );
		return 1;
	}
	
	// TSC clock frequency measurement, update variables
	printf( "measure TSC clock..." );
	opStatus = ( psfl->DLL_MeasureTsc )( &deltaTsc );
	if ( !opStatus )
	{
		printf( "FAILED.\nTSC clock measurement error.\n" );
		return 1;
	}
	if ( deltaTsc == 0 )
	{
		printf( "FAILED.\nTSC clock zero frequency returned.\n" );
		return 1;
	}
	frequencyHz = deltaTsc;
	periodSeconds = 1.0 / deltaTsc;
	
	// Show frequency in MHz, and period in nanoseconds
	double frequencyMHz = frequencyHz / 1000000.0;
	double periodNs = periodSeconds * 1000000000.0;
	printf( "OK.\nTSC frequency=%.3f MHz, period=%.3f ns\n", frequencyMHz, periodNs );
	
	// Detect and print system memory configuration info
	osErrorCode = pSystemMemory->detectMemory( &sysMemory );
    if( osErrorCode )
    {
        statusString = pSystemMemory->getStatusString( );
        printf( "\nError at %s\n", statusString );
        printSystemError( osErrorCode );
        return 1;
    }
    DWORD64 x1 = sysMemory.physicalMemory;
    DWORD64 x2 = sysMemory.freeMemory;
    DWORD64 x3 = 0;
    DWORD x4 = 0;
    DWORD x5 = 0;
    char sMem1[81];
    char sMem2[81];
    char sMem3[81];
    printMemorySize( sMem1, 80, x1 );
    printMemorySize( sMem2, 80, x2 );
    printf( "Memory: total physical=%s , free=%s\n", sMem1, sMem2 );
    
    // Detect and print cache (by WinAPI) and system topology configuration info
    osErrorCode = pSystemTopology->detectTopology( &sysTopology );
    if( osErrorCode )
    {
        statusString = pSystemTopology->getStatusString( );
        printf( "\nError at %s\n", statusString );
        printSystemError( osErrorCode );
        return 1;
    }
    x1 = sysTopology.pointL1;
    x2 = sysTopology.pointL2;
    x3 = sysTopology.pointL3;
    x4 = sysTopology.coreCount;
    x5 = sysTopology.hyperThreadingFlag;
	printMemorySize( sMem1, 80, x1 );
	printMemorySize( sMem2, 80, x2 );
	printMemorySize( sMem3, 80, x3 );
    printf( "Cache points for data read/write:\nL1=%s, L2=%s, L3=%s\nCPU core count=%d HyperThreading=%d\n", 
	        sMem1, sMem2, sMem3, x4, x5 );
	
	// Set memory regions, temporal mode and threads count by selected memory object
	DWORD64 mSize = 0;
	switch ( m )
	{
		case L1_CACHE:
			mSize = sysTopology.pointL1;
			if ( r2 == OPTION_NOT_SET ) r1 = APPROXIMATION_REPEATS_L1;
			break;
		case L2_CACHE:
			mSize = sysTopology.pointL2;
			if ( r2 == OPTION_NOT_SET ) r1 = APPROXIMATION_REPEATS_L2;
			break;
		case L3_CACHE:
			mSize = sysTopology.pointL3;
			if ( r2 == OPTION_NOT_SET ) r1 = APPROXIMATION_REPEATS_L3;
			break;
		case DRAM:
			mSize = CONST_DRAM_BLOCK;
			if ( r2 == OPTION_NOT_SET ) r1 = APPROXIMATION_REPEATS_DRAM;
			break;
		case OPTION_NOT_SET:
		default:
			break;
	}
	
	// Select reduced repeats mode for measure latency
	if ( ( r2 == OPTION_NOT_SET ) && 
	     ( ( a == CPU_FEATURE_LATENCY_LCM ) || ( a == CPU_FEATURE_LATENCY_RDRAND ) ) )
	{
		r1 /= REPEATS_DIVISOR_LATENCY;
	}

	// Check for automatically set number of threads, if command line parameter not set
	if ( n == OPTION_NOT_SET )
	{
		if ( ( m == DRAM ) &&
		     ( a != CPU_FEATURE_LATENCY_LCM ) &&
		     ( a != CPU_FEATURE_LATENCY_RDRAND ) )
		{   // object=dram, not a latency measurement, means bandwidth measurement
			n = sysTopology.coreCount;
			if ( ( sysTopology.hyperThreadingFlag ) && ( h != HT_OFF ) )
			{
				n *= 2;
			}
			// pp->optionThreads = n;   // multi-thread by platform topology
		}
		else
		{   // other situations
			// pp->optionThreads = 1;   // single-thread for selected scenario
			n = 1;
		}
	}

	// Check Cache or DRAM detection status
	if ( ( mSize == 0 ) && ( m != OPTION_NOT_SET ) )
	{
		printf( "ERROR: selected memory object not detected, use settings start/end/step.\n" );
		return 1;
	}
	else if ( m != OPTION_NOT_SET )
	{
		pp->optionBlockStart = mSize / 8;
		pp->optionBlockStop = mSize * 2;
		pp->optionBlockDelta = mSize / 8;
	}

	// Detect paging options
	opStatus = pPagingOptions->detectPaging( &pagingOptions );
	if ( !opStatus )
	{
		statusString = pPagingOptions->getStatusString( );
        printf( "\nError at %s\n", statusString );
		return 1;
	}
	char sPage1[81];
	char sPage2[81];
	DWORD y1 = pagingOptions.defaultPage;
	DWORD y2 = pagingOptions.largePage;
	DWORD y3 = pagingOptions.pagingRights;
    printMemorySize( sPage1, 80, y1 );
    printMemorySize( sPage2, 80, y2 );
    printf( "default page=%s, large page=%s, rights=%d\n", sPage1, sPage2, y3 );
    
    // Check selected paging option available
    if ( ( pp->optionPageSize != 0 ) && ( pagingOptions.largePage == 0 ) )
    {
    	printf( "Error: large pages not supported by platform.\n" );
    	return 1;
	}
	if ( ( pp->optionPageSize != 0 ) && ( pagingOptions.pagingRights == 0 ) )
	{
    	printf( "Error: no privileges for large pages.\n" );
    	return 1;
	}
	
	// Setup variables for benchmark scenario
	size_t bpi = bytesPerInstruction[a];
	scenario.startSize = pp->optionBlockStart;
	scenario.endSize = pp->optionBlockStop;
	scenario.deltaSize = pp->optionBlockDelta;
	scenario.maxSizeBytes = scenario.endSize;
	scenario.currentSizeInstructions = scenario.startSize / bpi;
	scenario.measurementRepeats = r1;
	scenario.methodId = a;
	scenario.numaMode = u;
	
	scenario.pagingMode = pp->optionPageSize;
	if ( scenario.pagingMode )
	{
		scenario.pageSize = pagingOptions.largePage;
	}
	else
	{
		scenario.pageSize = pagingOptions.defaultPage;
	}
	
	// Build threads list, print allocation
	// n = threads count, mt = threads list size in bytes, pt = pointer to threads list
	if ( ( n > MAXIMUM_THREADS ) || ( n <= 0 ) )
	{
		printf( "\nError: wrong number of threads (%d), can be 1-%d\n", n, MAXIMUM_THREADS );
		return 1;
	}
	int mt = sizeof( THREAD_CONTROL_ENTRY ) * n;
	THREAD_CONTROL_ENTRY* pt = ( THREAD_CONTROL_ENTRY* )malloc( mt );
	scenario.nThreadsList = n;
	scenario.pThreadsList = pt;
	if ( pt == NULL )
	{
		printf( "\nError at memory allocation for threads list.\n" );
		return 1;
	}
	printBaseAndSize( s, NS, ( DWORD64 )pt, mt );
	printf( "threads list allocated: %s\n", s );
	
	// Build handles list, print allocation
	// n = threads count, ms = signals list size in bytes, ps = pointer to signals list
	int ms = sizeof( HANDLE ) * n;
	HANDLE* ps = ( HANDLE* )malloc( ms );
	scenario.pSignalsList = ps;
	if ( ps == NULL )
	{
		printf( "\nError at memory allocation for signals handles list.\n" );
		return 1;
	}
	printBaseAndSize( s, NS, ( DWORD64 )ps, ms );
	printf( "handles list allocated: %s\n", s );

	// Build NUMA nodes list, print allocation
	// nn = NUMA nodes count, mnn = nodes list size in bytes, pnn = pointer to nodes list
	int nn = MAXIMUM_NODES;
	int mnn = sizeof( NUMA_NODE_ENTRY ) * nn;
	NUMA_NODE_ENTRY* pnn = ( NUMA_NODE_ENTRY* )malloc( mnn );
	scenario.nNodesList = nn;
	scenario.pNodesList = pnn;
	if ( pnn == NULL )
	{
		printf( "\nError at memory allocation for NUMA nodes list.\n" );
		return 1;
	}
	printBaseAndSize( s, NS, ( DWORD64 )pnn, mnn );
	printf( "nodes list allocated:   %s\n", s );
	
	// Detect Hyper-Threading masking mode, detect NUMA remote domains swap mode
	BOOL maskFlag = ( ( sysTopology.hyperThreadingFlag != 0 ) && ( h == HT_OFF ) );
	BOOL swapFlag = ( u == NUMA_REMOTE );
	
	// Detect NUMA topology and extended set of OS/NUMA API
	// Note this operations for all systems, include non-NUMA, results used on memory release
	printf( "detect NUMA...\n" );
	pNumaTopology->loadControlSet();
	pncs = pNumaTopology->getControlSet();
	int nndetected = pNumaTopology->buildNodesList( pnn );
	pNumaTopology->blankThreadsList( n, pt, maskFlag );


// DEBUG
/*
	GROUP_AFFINITY gaff1;
	gaff1.Mask = 1;
	gaff1.Group = 0;
	gaff1.Reserved[0] = 0, gaff1.Reserved[1] = 0, gaff1.Reserved[2] = 0;
	GROUP_AFFINITY gaff2;
	gaff2.Mask = 1;
	gaff2.Group = 0;
	HANDLE th = GetCurrentThread( );
	int dbgst1 = ( pncs->API_SetThreadGroupAffinity ) ( th, &gaff1, &gaff2 );
	int dbgst2 = GetLastError( );
	printf( " \n\n DEBUG STATUS = %d, %d \n\n ", dbgst1, dbgst2 );
	return 2;
*/
// DEBUG


	// Note this operations for NUMA-aware branch selected by user option
	if ( ( u == NUMA_LOCAL ) || ( u == NUMA_REMOTE ) )
	{
		printf( "selected NUMA-aware memory allocation...\n" );
		if ( nndetected > 0 )
		{
			DWORD64 sizePerNode = scenario.maxSizeBytes * n / nndetected;
			BOOL status = pNumaTopology->allocateNodesList
				( sizePerNode, nndetected, pnn, scenario.pagingMode, scenario.pageSize, swapFlag );
			if ( !status )
			{
				printf( "\nError at NUMA-aware at-nodes memory allocation.\n" );
				return 1;
			}
			status = pNumaTopology->allocateThreadsList( nndetected, n, pnn, pt );
			if ( !status )
			{
				printf( "\nError at NUMA-aware at-threads memory allocation.\n" );
				return 1;
			}
		}
		else
		{
			printf( "\nError: NUMA not supported.\n" );
			return 1;
		}
	}

	// Create Performer class
	pPerformer = new Performer( psfl, pncs );
	
	// Build threads context
	osErrorCode = pPerformer->buildThreadsList( &scenario );
	if ( osErrorCode )
	{
		statusString = pPerformer->getStatusString( );
        printf( "\nError at %s\n", statusString );
        printSystemError( osErrorCode );
		return 1;
	}

	// Benchmark scenario, run threads
	printf( "running threads...\n" );
	osErrorCode = pPerformer->threadsRun( &scenario, deltaTsc );
	if ( osErrorCode )
	{
		statusString = pPerformer->getStatusString( );
        printf( "\nError at %s\n", statusString );
        printSystemError( osErrorCode );
		return 1;
	}
	print64( s, NS, deltaTsc );
	printf( "run OK, dTSC=%s\n", s );
	
	// Detect measurement cycle length
	if ( scenario.startSize <= scenario.endSize )
	{
		blockMax = ( scenario.endSize - scenario.startSize ) / scenario.deltaSize;
	}
	else
	{
		blockMax = ( scenario.startSize - scenario.endSize ) / scenario.deltaSize;
	}
	blockSize = scenario.startSize;
	blockDelta = scenario.deltaSize;
	
	// Build statistics list, print allocation
	// blockMax = elements count, ma = element size in bytes, pa = pointer to statistics array
	int ma = sizeof( double ) * ( blockMax + 1 );
	double* pa = ( double* )malloc( ma );
	if ( pa == NULL )
	{
		printf( "\nError at memory allocation for statistics array.\n" );
		return 1;
	}
	printBaseAndSize( s, NS, ( DWORD64 )pa, ma );
	printf( "statistics array allocated: %s\n", s );
	
	// print benchmark conditions, before calibration.
	char sPage[81];
	printMemorySize( sPage, 80, scenario.pageSize );
	
	if ( r3 == OPTION_NOT_SET )
	{
		printf( "\nRUN: method=%d, threads=%d, repeats=%d\n     buffer=%d, large pages=%d, page size=%s\n", 
	        a, n, r1, b, scenario.pagingMode, sPage );
	}
	else
	{
		printf( "\nRUN: method=%d, threads=%d, repeats=ADAPTIVE\n     buffer=%d, large pages=%d, page size=%s\n", 
	        a, n, b, scenario.pagingMode, sPage );
	}
	        
	printf( "     start=%d, end=%d, delta=%d, bpi=%d\n", 
	        ( int )scenario.startSize, ( int )scenario.endSize, ( int )scenario.deltaSize, bpi );

	// Calibration
	if ( ( r2 == OPTION_NOT_SET ) && ( r3 == OPTION_NOT_SET ) )
	{
		printf( "     calibration..." );
		
		pPerformer->threadsUpdate( &scenario, scenario.endSize / bpi );
		osErrorCode = pPerformer->threadsRestart( &scenario, deltaTsc );

		if ( ( !osErrorCode ) && 
		     ( ( scenario.methodId == CPU_FEATURE_LATENCY_LCM    ) || 
		       ( scenario.methodId == CPU_FEATURE_LATENCY_RDRAND )  )  )
		{
			pPerformer->routineUpdate( &scenario, CPU_FEATURE_LATENCY_WALK );
			osErrorCode = pPerformer->threadsRestart( &scenario, deltaTsc );
			pPerformer->routineUpdate( &scenario, scenario.methodId );
		}

		if ( osErrorCode )
		{
			statusString = pPerformer->getStatusString( );
       		printf( "\nCalibration error at %s\n", statusString );
       		printSystemError( osErrorCode );
			return 1;
		}
		double cTime = deltaTsc * periodSeconds;
		double cTarget = CALIBRATION_TARGET_TIME;
		double cRepeats = r1 * ( cTarget / cTime );
		r1 = cRepeats;
		printf( "OK. delay=%.3f seconds, update repeats=%d. \n", cTime, r1 );
		pPerformer->repeatsUpdate( &scenario, r1 );
	}
	
	// Print parameters names and table up line
	printf ( "\n   #        size   CPI     nsPI    MBPS\n" );
	printLine( s, NS, 55 );
	printf( "%s\n", s );
	
	// Start measurements cycle for different block sizes, show table: speed = f(size)
	double* pa1 = pa;
	for( blockCount=0; blockCount<=blockMax; blockCount++ )
	{

		DWORD64 r3min=0, r3max=0, r3value=0;
		if ( r3 != OPTION_NOT_SET )
		{
			r3min = 1;
			r3max = 0xFFFFFFFFL;
			r3value = r3 / ( blockSize / bpi );       // approximate constant number of instructions = measurementRepeats * blockSize
			if ( r3value < r3min ) r3value = r3min;
			if ( r3value > r3max ) r3value = r3max;
			pPerformer->repeatsUpdate( &scenario, r3value );
		}

		pPerformer->threadsUpdate( &scenario, blockSize / bpi );
		osErrorCode = pPerformer->threadsRestart( &scenario, deltaTsc );
		
		if ( ( !osErrorCode ) && 
		     ( ( scenario.methodId == CPU_FEATURE_LATENCY_LCM    ) || 
		       ( scenario.methodId == CPU_FEATURE_LATENCY_RDRAND )  )  )
		{
			pPerformer->routineUpdate( &scenario, CPU_FEATURE_LATENCY_WALK );
			osErrorCode = pPerformer->threadsRestart( &scenario, deltaTsc );
			pPerformer->routineUpdate( &scenario, scenario.methodId );
		}
		
		if ( osErrorCode )
		{
			statusString = pPerformer->getStatusString( );
        	printf( "\nBenchmark error at %s\n", statusString );
        	printSystemError( osErrorCode );
			return 1;
		}
		
		DWORD64 x1 = blockSize;             // block size, bytes
		DWORD64 x2 = r1;                    // number of measurement repeats
		if ( r3value > 0 ) x2 = r3value;    // this for adaptive repeats mode
		DWORD64 x3 = n;                     // number of threads
		double x4 = deltaTsc;               // time interval in TSC clocks
		cpi = x4 / ( x1 * x2 * x3 / bpi );
		nspi = cpi * periodSeconds * 1000000000.0;
		
		megabytes = x1 * x2 * x3 / 1000000.0;
		seconds = deltaTsc * periodSeconds;
		mbps = megabytes / seconds;
		
		printf ( "%4d%12d%8.3f%8.3f   %-10.3f\n", blockCount+1, blockSize, cpi, nspi, mbps );
		
		*pa1++ = mbps;
		blockSize += blockDelta;
	}
	
	// Print table down line
	printLine( s, NS, 55 );
	printf( "%s\n", s );
	
	// Delete threads context
	osErrorCode = pPerformer->releaseThreadsList( &scenario );
	if( osErrorCode )
	{
		statusString = pPerformer->getStatusString( );
       	printf( "\nError at %s\n", statusString );
       	printSystemError( osErrorCode );
		return 1;
	}

	// Statistics calculate and show
	calculateStatistics( blockMax+1, pa, &min, &max, &average, &median );
	printf( "\n max=%-10.3f, min=%-10.3f, average=%-10.3f, median=%-10.3f\n", max, min, average, median );
	printLine( s, NS, 55 );
	printf( "\n%s\n", s );
	
	// Delete created objects and termination
	free( pa );
	pPerformer->freeThreadsList( &scenario );         // This memory release for NUMA-unaware allocation
	delete pPerformer;
	pNumaTopology->freeNodesList( nndetected, pnn );  // This memory release for NUMA-aware allocation
	free( scenario.pNodesList );
	free( scenario.pSignalsList );
	free( scenario.pThreadsList );
	delete pPagingOptions;
	delete pNumaTopology;
	delete pSystemTopology;
	delete pSystemMemory;
	delete pDecoderCpuid;
	delete pSystemLibrary;
	delete pCommandLine;
	return 0;
}

