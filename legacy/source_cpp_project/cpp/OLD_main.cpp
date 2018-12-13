/* 

UNDER CONSTRUCTION.
Memory Performance Engine.
Debug sample, without technologies support pre-check.
TODO:
 1) + Crush when Nthreads > 1. Reenterancy ?
 2) + Slow when Nthreads > Ncpu, because waiting events clear, use separate events for set-clear.
 3) Optimize SIZE, SSIZE overlows prevent.
 4) When integration: check TSC supported.
 5) When integration: check CPU selected instruction set supported.
 6) Add and debug class: NUMA affinitization.
 7) Add and debug class: Large pages.

*/

#include <cstdio>
#include <windows.h>
#include "GlobalDeclarations.h"
#include "SystemLibrary.h"
#include "Performer.h"

#define ASM_METHOD CPU_FEATURE_READ_SSE128     // CPU_FEATURE_READ_AVX256 // CPU_FEATURE_READ_SSE128
#define THREADS_COUNT 1                        // 4 // 1
#define MEASUREMENT_REPEATS 2000000
#define TOTAL_SIZE_BYTES 65536

#define START_SIZE_INSTRUCTIONS 4096
#define END_SIZE_INSTRUCTIONS 65536
#define DELTA_SIZE_INSTRUCTIONS 4096           // 1024 // 4096
#define BYTES_PER_INSTRUCTION 16               // 32 // 16

// Title and service strings include copyright
const char* stringTitle1 = "Memory Performance Engine.";
const char* stringTitle2 = BUILD_STRING;
const char* stringTitle3 = "(C)2018 IC Book Labs.";
const char* stringAnyKey = "Press any key...";

SystemLibrary* pSystemLibrary = NULL;
Performer* pPerformer = NULL;
SYSTEM_FUNCTIONS_LIST* pf = NULL;
BENCHMARK_SCENARIO scenario;
BOOL status;
DWORD64 deltaTsc;
double frequencyHz;
double periodSeconds;

double megabytes;
double seconds;
double mbps;
int blockMax;
int blockCount;
int blockSize;
int blockDelta;
double cpi;
double nspi;

int print64( char* s, size_t n, DWORD64 x );
int printBaseAndSize( char* s, size_t n, DWORD64 base, DWORD64 size );
void printLine( char* s, size_t n, int m );
void calculateStatistics( int length , double results[] , 
                          double* min , double* max , double* average , double* median );

int print64( char* s, size_t n, DWORD64 x )
{
	DWORD32 xLow = x;
	DWORD32 xHigh = ( x >> 16 ) >> 16;
	int m = snprintf( s, n, "%08X%08Xh", xHigh, xLow );
	return m;
}

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


int main(int argc, char** argv) 
{
	// Show title
	printf( "\n%s %s %s\n", stringTitle1, stringTitle2, stringTitle3 );
	// Buffer strings
	const int NS = 80;
	char s[NS+1];
	char* statusString;
	// Load library, check validity
	printf( "load library..." );
	pSystemLibrary = new SystemLibrary( );
	status = pSystemLibrary->loadSystemLibrary( );
	if ( !status )
	{
		printf( "FAILED.\n" );
		statusString = pSystemLibrary->getStatusString( );
        printf( "Error at %s\n", statusString );
		return 1;
	}
	printf( "OK.\n" );
	pf = pSystemLibrary->getSystemFunctionsList( );
	char *dllProduct, *dllVersion, *dllVendor;
	pf->DLL_GetDllStrings( &dllProduct, &dllVersion, &dllVendor );
	printf( "%s %s %s\n", dllProduct, dllVersion, dllVendor );
	
	// Setup and print variables not changes when executed benchmark scenario
	int a = ASM_METHOD;
	int n = THREADS_COUNT;
	int r = MEASUREMENT_REPEATS;
	int b = TOTAL_SIZE_BYTES;
	printf( "method=%d, threads=%d, repeats=%d, buffer=%d\n", a, n, r, b );
	
	// TSC clock frequency measurement, update variables
	printf( "measure TSC clock..." );
	status = ( pf->DLL_MeasureTsc )( &deltaTsc );
	if ( !status )
	{
		printf( "FAILED.\nTSC clock measurement error.\n" );
		return 2;
	}
	if ( deltaTsc == 0 )
	{
		printf( "FAILED.\nTSC clock zero frequency returned.\n" );
		return 3;
	}
	frequencyHz = deltaTsc;
	periodSeconds = 1.0 / deltaTsc;
	double frequencyMHz = frequencyHz / 1000000.0;
	double periodNs = periodSeconds * 1000000000.0;
	printf( "OK.\nTSC frequency=%.3f MHz, period=%.3f ns\n", frequencyMHz, periodNs );
	
	// Setup and print variables, changed under benchmark scenario
	size_t bs = START_SIZE_INSTRUCTIONS;
	size_t be = END_SIZE_INSTRUCTIONS;
	size_t bd = DELTA_SIZE_INSTRUCTIONS;
	size_t bpi = BYTES_PER_INSTRUCTION;
	scenario.startSize = bs;
	scenario.endSize = be;
	scenario.deltaSize = bd;
	scenario.maxSizeBytes = be;
	scenario.currentSizeInstructions = bs / bpi;
	scenario.measurementRepeats = r;
	scenario.methodId = a;
	printf( "start=%d, end=%d, delta=%d, bpi=%d\n", bs, be, bd, bpi );
	
	// Build threads list, print allocation
	// n = threads count, mt = threads list size in bytes, pt = pointer to threads list
	int mt = sizeof( THREAD_CONTROL_ENTRY ) * n;
	THREAD_CONTROL_ENTRY* pt = ( THREAD_CONTROL_ENTRY* )malloc( mt );
	scenario.nThreadsList = n;
	scenario.pThreadsList = pt;
	if ( pt == NULL )
	{
		printf( "\nError at memory allocation for threads list.\n" );
		return 4;
	}
    /* DEBUG. DWORD64 x1 = 0x0123456789ABCDEF;
	DWORD64 x2 = 0xF;
	printBaseAndSize( s, NS, x1, x2 );
	printf( "\n\n%s\n\n", s ); */
	printBaseAndSize( s, NS, ( DWORD64 )pt, mt );
	printf( "threads list allocated: %s.\n", s );
	
	// Build handles list, print allocation
	// n = threads count, ms = signals list size in bytes, ps = pointer to signals list
	int ms = sizeof( HANDLE ) * n;
	HANDLE* ps = ( HANDLE* )malloc( ms );
	scenario.pSignalsList = ps;
	if ( ps == NULL )
	{
		printf( "\nError at memory allocation for signals handles list.\n" );
		return 5;
	}
	printBaseAndSize( s, NS, ( DWORD64 )ps, ms );
	printf( "handles list allocated: %s.\n", s );

	// Create Performer class
	pPerformer = new Performer( pf );
	
	// Build threads context
	status = pPerformer->buildThreadsList( &scenario );
	if ( !status )
	{
		statusString = pPerformer->getStatusString( );
        printf( "\nError at %s\n", statusString );
		return 6;
	}

	// Benchmark scenario
	
	// Run threads
	printf( "running threads...\n" );
	status = pPerformer->threadsRun( &scenario, deltaTsc );
	if ( !status )
	{
		statusString = pPerformer->getStatusString( );
        printf( "\nError at %s\n", statusString );
		return 7;
	}
	// DEBUG: Sleep( 100 );
	print64( s, NS, deltaTsc );
	printf( "run OK, dTSC=%s\n", s );

	/*	
	//scenario.currentSizeInstructions = be / bpi;
	// pPerformer->threadsUpdate( &scenario, be / bpi * 2 );
	int i;
	for ( i=0; i<3; i++ )
	{
		// Restart threads
		printf( "continue threads...\n" );
		status = pPerformer->threadsRestart( &scenario, deltaTsc );
		if ( !status )
		{
			statusString = pPerformer->getStatusString( );
        	printf( "\nError at %s\n", statusString );
			return 8;
		}
		// DEBUG: Sleep( 100 );
		print64( s, NS, deltaTsc );
		printf( "restart OK, dTSC=%s\n", s );
		// Calculate and print megabytes per second (mbps)
		megabytes = scenario.currentSizeInstructions * bpi * scenario.measurementRepeats * scenario.nThreadsList / 1000000.0;
		seconds = periodSeconds * deltaTsc;
		mbps = megabytes / seconds;
		printf( "megabytes=%.3f, seconds=%.3f, mbps=%.3f\n", megabytes, seconds, mbps );
	}
	*/



/*	
	int i;
	int j;
	int k = NS;
	char* pstr = s;
	for( i=0; i<45; i++ )
	{
		j = snprintf( pstr, k, "-" );
		pstr += j;
		k -= j;
	}
	printf( "\n Index    Block, bytes       MBPS" );
	printf( "\n%s\n", s );
	
	int drawIndex;
	int drawCount;
	if ( bs <= be )
	{
		drawCount = ( be - bs ) / bd;
	}
	else
	{
		drawCount = ( bs - be ) / bd;
	}
	
	for( drawIndex=0; drawIndex<=drawCount; drawIndex++ )
	{
		int x1 = bs + bd * drawIndex;
		int x2 = x1 / bpi;
		pPerformer->threadsUpdate( &scenario, x2 );
			status = pPerformer->threadsRestart( &scenario, deltaTsc );
			if ( !status )
			{
				statusString = pPerformer->getStatusString( );
        		printf( "\nError at %s\n", statusString );
				return 8;
			}

		// DWORD64 y1 = scenario.currentSizeInstructions;
		// DWORD64 y2 = bpi;
		// DWORD64 y3 = scenario.measurementRepeats;
		// DWORD64 y4 = scenario.nThreadsList;
		// megabytes = y1 * y2 * y3 * y4 / 1000000.0;
		
		megabytes = 
		scenario.currentSizeInstructions *
		bpi *
		scenario.measurementRepeats *
		scenario.nThreadsList /
		1000000.0;
		
		seconds = periodSeconds * deltaTsc;
		mbps = megabytes / seconds;

		printf("  %-9d%-18d%-10.3f\n", drawIndex, x1, mbps );
	}
	
	printf( "%s\n", s );

	// Delete threads context
	status = pPerformer->releaseThreadsList( &scenario );
	if( !status )
	{
		statusString = pPerformer->getStatusString( );
       	printf( "\nError at %s\n", statusString );
		return 9;
	}
*/

	printf ( "\n   #        size   CPI     nsPI    MBPS\n" );
	printLine( s, NS, 55 );
	printf( "%s\n", s );
	
	if ( bs <= be )
	{
		blockMax = ( be - bs ) / bd;
	}
	else
	{
		blockMax = ( bs - be ) / bd;
	}
	blockSize = bs;
	blockDelta = bd;
	
	for( blockCount=0; blockCount<=blockMax; blockCount++ )
	{
		pPerformer->threadsUpdate( &scenario, blockSize / bpi );
		status = pPerformer->threadsRestart( &scenario, deltaTsc );
		if ( !status )
		{
			statusString = pPerformer->getStatusString( );
        	printf( "\nError at %s\n", statusString );
			return 8;
		}
		DWORD64 x1 = blockSize;
		DWORD64 x2 = r;
		double x3 = deltaTsc;
		cpi = x3 / ( x1 * x2 / bpi );
		nspi = cpi * periodSeconds * 1000000000.0;
		megabytes = x1 * x2 / 1000000.0;
		seconds = deltaTsc * periodSeconds;
		mbps = megabytes / seconds;
		printf ( " %3d  %10d   %5.3f   %5.3f   %-10.3f\n", blockCount+1, blockSize, cpi, nspi, mbps );
		blockSize += blockDelta;
	}
	
	printLine( s, NS, 55 );
	printf( "%s\n", s );
	
	// Delete threads context
	status = pPerformer->releaseThreadsList( &scenario );
	if( !status )
	{
		statusString = pPerformer->getStatusString( );
       	printf( "\nError at %s\n", statusString );
		return 9;
	}

	// Delete created objects and termination
	delete pPerformer;
	free( scenario.pThreadsList );
	delete pSystemLibrary;
	return 0;
}

