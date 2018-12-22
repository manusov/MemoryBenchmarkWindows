/*
       MEMORY PERFORMANCE ENGINE FRAMEWORK.
    CPU features and frequency detector class.
*/

#include "ProcessorDetector.h"

// Pointer to global control set of functions 
FUNCTIONS_LIST* ProcessorDetector::f = NULL;
// System processor summary report data
SYSTEM_PROCESSOR_DATA ProcessorDetector::processorData;
// Status string
#define NS 81
char ProcessorDetector::s[NS];

// Values Bytes Per Instruction for native methods
// This table associated with native DLL procedures
// Arrays length must be constant = MAXIMUM_ASM_METHOD
#if NATIVE_WIDTH == 32
const BYTE ProcessorDetector::bytesPerInstruction[] = 
{
     4,  4,  4,  4,  4,  4,
	 8,  8,  8, 
	16, 16, 16, 32, 32, 32, 64, 64, 64, 32, 64,
    16, 16, 32, 32, 64, 64, 16, 16, 32, 32, 64, 64, 16, 16, 32,
     8,  8
};
#endif
#if NATIVE_WIDTH == 64
const BYTE ProcessorDetector::bytesPerInstruction[] = 
{
     8,  8,  8,  8,  8,  8,
	 8,  8,  8, 
	16, 16, 16, 32, 32, 32, 64, 64, 64, 32, 64,
    16, 16, 32, 32, 64, 64, 16, 16, 32, 32, 64, 64, 16, 16, 32,
     8,  8
};
#endif

// Data for build strings about processor features
const char* ProcessorDetector::instructionStrings[] =
{
#if NATIVE_WIDTH == 32
	"read 32-bit MOV",
	"write 32-bit MOV",
	"copy 32-bit MOV",
	"modify 32-bit NOT",
	"write 32-bit REP STOSD",
	"copy 32-bit REP MOVSD",
#endif
#if NATIVE_WIDTH == 64
	"read 64-bit MOV",
	"write 64-bit MOV",
	"copy 64-bit MOV",
	"modify 64-bit NOT",
	"write 64-bit REP STOSQ",
	"copy 64-bit REP MOVSQ",
#endif
// start of added at v0.60.03
	"read 64-bit MMX MOVQ",
	"write 64-bit MMX MOVQ",
	"copy 64-bit MMX MOVQ",
// end of added at v0.60.03
	"read 128-bit SSE MOVAPS",
	"write 128-bit SSE MOVAPS",
	"copy 128-bit SSE MOVAPS",
	"read 256-bit AVX VMOVAPD",
	"write 256-bit AVX VMOVAPD",
	"copy 256-bit AVX VMOVAPD",
	"read 512-bit AVX VMOVAPD",
	"write 512-bit AVX VMOVAPD",
	"copy 512-bit AVX VMOVAPD",
	"dot product 256-bit FMA VFMADD231PD",
	"dot product 512-bit FMA VFMADD231PD",
	"non-temporal write 128-bit SSE MOVNTPS",
	"non-temporal copy 128-bit SSE MOVAPS+MOVNTPS",
	"non-temporal write 256-bit AVX VMOVNTPD",
	"non-temporal copy 256-bit AVX VMOVAPD+VMOVNTPD",
	"non-temporal write 512-bit AVX VMOVNTPD",
	"non-temporal copy 512-bit AVX VMOVAPD+VMOVNTPD",
	"non-temporal read 128-bit SSE MOVNTDQA",
	"non-temporal copy 128-bit SSE MOVNTDQA+MOVNTPD",
	"non-temporal read 256-bit AVX VMOVNTDQA",
	"non-temporal copy 256-bit AVX VMOVNTDQA+VMOVNTPD",
	"non-temporal read 512-bit AVX VMOVNTDQA",
	"non-temporal copy 512-bit AVX VMOVNTDQA+VMOVNTPD",
	"non-temporal read 128-bit SSE PREFETCHNTA+MOVAPS",
	"reserved",
	"non-temporal read 256-bit AVX PREFETCHNTA+VMOVAPD",
	"prepare walk list for latency measure by LCM",
	"prepare walk list for latency measure by RDRAND",
	"latency measure by walk list",
	NULL	
};

// Control structure for detect optional supported CPU features, decoder for CPUID results
const CPUID_CONDITION ProcessorDetector::x[] = {
	// Unconditional x86-64 base instruction set
	{ ORMASK      , 0          , 0          , NOREG  , 0                       , CPU_FEATURES_UNCONDITIONAL } ,
// start of added at v0.60.03
	// MMX required
	{ ORMASK      , 0x00000001 , 0x00000000 , EDX    , 1<<23                   , CPU_FEATURES_MMX64         } ,
// end of added at v0.60.03
	// SSE required
	{ ORMASK      , 0x00000001 , 0x00000000 , EDX    , 1<<25                   , CPU_FEATURES_SSE128        } ,
	// AVX256 include context management required
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , (1<<27)|(1<<28)         , CPU_FEATURES_AVX256        } ,
	// AVX512 include context management required
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1<<16                   , CPU_FEATURES_AVX512        } ,
    { ANDMASK     , 0x00000001 , 0x00000000 , ECX    , 1<<27                   , CPU_FEATURES_AVX512        } ,
    // FMA256 include context management required
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , (1<<27)|(1<<28)|(1<<12) , CPU_FEATURES_FMA256        } ,
	// FMA512 include context management required
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1<<16                   , CPU_FEATURES_FMA512        } ,
	{ ANDMASK     , 0x00000001 , 0x00000000 , ECX    , (1<<27)|(1<<12)         , CPU_FEATURES_FMA512        } ,
	// SSE non-temporal read by MOVNTDQA (SSE4.1)
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , 1<<19                   , CPU_FEATURES_MOVNTDQA128   } ,
	// AVX256 non-temporal read by VMOVNTDQA (AVX2) include context management required
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1<<5                    , CPU_FEATURES_MOVNTDQA256   } ,
	{ ANDMASK     , 0x00000001 , 0x00000000 , ECX    , 1<<27                   , CPU_FEATURES_AVX512        } ,
	// RDRAND instruction required
	{ ORMASKLAST  , 0x00000001 , 0x00000000 , ECX    , 1<<30                   , CPU_FEATURES_RDRAND        }
};

// Control structure for detect optional supported OS context management features, decoder for XGETBV results
const XGETBV_CONDITION ProcessorDetector::y[] = {
	{ UNCOND     , 0                           , CPU_FEATURES_UNCONDITIONAL } ,
// start of added at v0.60.03
	{ UNCOND     , 0                           , CPU_FEATURES_MMX64         } ,
// end of added at v0.60.03
	{ UNCOND     , 0                           , CPU_FEATURES_SSE128        } ,
	{ UNCOND     , 0                           , CPU_FEATURES_MOVNTDQA128   } ,
	{ ORMASK     , 1<<2                        , CPU_FEATURES_AVX256        } ,
	{ ORMASK     , (1<<2)|(1<<5)|(1<<6)|(1<<7) , CPU_FEATURES_AVX512        } ,
	{ ORMASK     , 1<<2                        , CPU_FEATURES_FMA256        } ,
	{ ORMASK     , (1<<2)|(1<<5)|(1<<6)|(1<<7) , CPU_FEATURES_FMA512        } ,
	{ ORMASK     , 1<<2                        , CPU_FEATURES_MOVNTDQA256   } ,
	{ UNCONDLAST , 0                           , CPU_FEATURES_RDRAND        }
};

// Table up for CPU functionality list
const char* ProcessorDetector::tableUpFunctionality =
	" #    CPU  OS   cpu functionality and instructions\r\n";

// String results of CPUID
#define VENDOR_SIZE 12+1
#define MODEL_SIZE 48+1
char ProcessorDetector::cpuVendorString[VENDOR_SIZE];
char ProcessorDetector::cpuModelString[MODEL_SIZE];

// Class constructor, initialize pointers, get Processor data
ProcessorDetector::ProcessorDetector( FUNCTIONS_LIST* functions )
{
	// Blank status string
    snprintf( s, NS, "no data" );
	// global initialization and pre-blank output
	f = functions;
	processorData.deltaTsc = 0;
	processorData.cpuBitmap = 0;
	processorData.osBitmap = 0;
	
	BOOL status = ( f->DLL_CheckCpuid )( );
	if ( !status ) return;
	
	const char* s1 = "no data";
	snprintf( cpuVendorString, VENDOR_SIZE, s1 );
	snprintf( cpuModelString, MODEL_SIZE, s1 );
	// Get processor vendor and model strings
	getVendorAndModel( cpuVendorString, cpuModelString, VENDOR_SIZE, MODEL_SIZE );
	// Measure CPU TSC frequency by RDTSC instruction
	// ... This fragment moved from constructor for show string "Measure TSC clock..."	
	// Detect CPU instruction set features by CPUID instruction
	buildCpuBitmap( processorData.cpuBitmap );
	// Detect OS context management features by XGETBV instruction
	buildOsBitmap( processorData.osBitmap );
}

// Class destructor, functionality reserved
ProcessorDetector::~ProcessorDetector( )
{

}

// Return pointer to processor data
SYSTEM_PROCESSOR_DATA* ProcessorDetector::getProcessorList( )
{
	return &processorData;
}

// Get text report
#define INSTRUCTION_LINE 70
#define INSTRUCTION_CELL 50
#define SMALL_CELL 5

void ProcessorDetector::getProcessorText( LPSTR &dst, size_t &max )
{
	int count = 0;
	// Processor vendor and model
	count = snprintf( dst, max, "processor vendor = %s\r\nprocessor model  = %s\r\n", 
					  cpuVendorString, cpuModelString );
	dst += count;
	max -= count;
	// Processor frequency
	DWORD64 hz = processorData.deltaTsc;
	if ( hz == 0 )
	{
		count = snprintf( dst, max, "time stamp counter (TSC) frequency measurement error\r\n");
	}
	else
	{
		double mhz = hz / 1000000.0;
		double ns = 1000000000.0 / hz;
		count = snprintf( dst, max, 
						  "time stamp counter (TSC) frequency = %.1f MHz, period = %.3f ns\r\n",
						  mhz, ns );
	}
	dst += count;
	max -= count;
	// Processor and OS bitmaps as hex
	DWORD64 cpuBits = processorData.cpuBitmap;
	DWORD64 osBits = processorData.osBitmap;
	AppLib::printString( dst, max, "cpuid bitmap = " );
	AppLib::print64( dst, max, cpuBits, TRUE );
	AppLib::printString( dst, max, " , xgetbv bitmap = " );
	AppLib::print64( dst, max, osBits, TRUE );
	AppLib::printCrLf( dst, max );
	AppLib::printCrLf( dst, max );
	// Initializing variables for functionality tables
	DWORD64 mask = 1;
	int cpuFlag = 0;
	int osFlag = 0;
	int i = 0;
	// Build table of CPU and OS features
	AppLib::printString( dst, max, tableUpFunctionality );
	AppLib::printLine( dst, max, INSTRUCTION_LINE );
	while ( instructionStrings[i] != NULL )
	{
		if ( ( cpuBits & mask ) == 0 ) { cpuFlag = 0; } else { cpuFlag = 1; }
		if ( ( osBits & mask ) == 0  ) { osFlag = 0;  } else { osFlag = 1;  }
		AppLib::printString( dst, max, " " );
		AppLib::printCellNum( dst, max, i, SMALL_CELL );
		AppLib::printCellNum( dst, max, cpuFlag, SMALL_CELL );
		AppLib::printCellNum( dst, max, osFlag,  SMALL_CELL );
		AppLib::printCell( dst, instructionStrings[i], max, INSTRUCTION_CELL );
		AppLib::printCrLf( dst, max );
		mask = mask << 1;
		i++;
	}
	AppLib::printLine( dst, max, INSTRUCTION_LINE );
}

// Method returns status string, valid if error returned
char* ProcessorDetector::getStatusString( )
{
    return s;
}

// Public method for check TSC (Time Stamp Counter) support and measure TSC clock
// This fragment moved from constructor for show string "Measure TSC clock..."	
BOOL ProcessorDetector::measureTSC( )
{
	// Initializing variables
	DWORD rEax = 0;
	DWORD rEbx = 0;
	DWORD rEcx = 0;
	DWORD rEdx = 0;
	BOOL opStatus = FALSE;
	processorData.deltaTsc = 0;
	// Check RDTSC support
	( f->DLL_ExecuteCpuid )( 0, 0, &rEax, &rEbx, &rEcx, &rEdx );  // CPUID function=0, subfunction=n/a
	rEdx = 0;
	if ( rEax >= 1 )
	{
		( f->DLL_ExecuteCpuid )( 1, 0, &rEax, &rEbx, &rEcx, &rEdx );  // CPUID function=1, subfunction=n/a
	}
	if ( ( rEdx & 0x00000010 ) == 0 )
	{
		snprintf( s, NS, "FAILED.\nRDTSC instruction not supported or locked by VMM.\n" );
		return FALSE;
	}
	// TSC clock frequency measurement, update variables
	opStatus = ( f->DLL_MeasureTsc )( &processorData.deltaTsc );
	if ( !opStatus )
	{
		snprintf( s, NS, "FAILED.\nTSC clock measurement error.\n" );
		return FALSE;
	}
	if ( processorData.deltaTsc == 0 )
	{
		snprintf( s, NS, "FAILED.\nTSC clock zero frequency returned.\n" );
		return FALSE;
	}
	return TRUE;
}

// Public method for detect bytes per instruction depend on benchmark routine
BYTE ProcessorDetector::getBytesPerInstruction( int i )
{
	return bytesPerInstruction[i];
}


const char* ProcessorDetector::getInstructionString( int i )
{
	return instructionStrings[i];
}

// ---------- Public methods for CPUID and XGETBV support (ported from v0.5x) ----------

// This method used for auto select method, for temporal mode (cacheable)
DWORD ProcessorDetector::findMaxMethodTemporal( DWORD64 bitmapCpu, DWORD64 bitmapOs )
{
    DWORD64 bitmap = bitmapCpu & bitmapOs;
    int select = CPU_FEATURE_READ_AVX512;
    if ( mapCheck ( bitmap, select ) ) return select;
    select = CPU_FEATURE_READ_AVX256;
    if ( mapCheck ( bitmap, select ) ) return select;
    select = CPU_FEATURE_READ_SSE128;
    if ( mapCheck ( bitmap, select ) ) return select;
    select = CPU_FEATURE_READ_MMX64;
    if ( mapCheck ( bitmap, select ) ) return select;
    select = CPU_FEATURE_READ_IA32_X64;
    if ( mapCheck ( bitmap, select ) ) return select;
    return OPTION_NOT_SET;
}

// This method used for auto select method, for non-temporal mode (uncacheable)
DWORD ProcessorDetector::findMaxMethodNonTemporal( DWORD64 bitmapCpu, DWORD64 bitmapOs )
{
    DWORD64 bitmap = bitmapCpu & bitmapOs;
    int select = CPU_FEATURE_NTRW_READ_AVX512;
    if ( mapCheck ( bitmap, select ) ) return select;
    select = CPU_FEATURE_NTPRW_READ_AVX256;
    if ( mapCheck ( bitmap, select ) ) return select;
    select = CPU_FEATURE_NTPRW_READ_SSE128;
    if ( mapCheck ( bitmap, select ) ) return select;
    select = CPU_FEATURE_READ_MMX64;
    if ( mapCheck ( bitmap, select ) ) return select;
    select = CPU_FEATURE_READ_IA32_X64;
    if ( mapCheck ( bitmap, select ) ) return select;
    return OPTION_NOT_SET;

}

// Helper method for test 64-bit map, prevent overflow of 32-bit operations
BOOL ProcessorDetector::mapCheck( DWORD64 map, int index )
{
	int i1 = index;
	int i2 = 0;
	if ( index > 31 )
	{
		i1 = 31;
		i2 = index - 31;
	}
	map = map >> i1;
	map = map >> i2;
	return ( map & 1L );
}

// ---------- Private helpers methods (ported from v0.5x) ----------

// Get processor vendor and model strings, by CPUID instruction
BOOL ProcessorDetector::getVendorAndModel( char* cpuVendorString, char* cpuModelString, int nV, int nM )
{
    BOOL status = FALSE;
    if ( ( nV < VENDOR_SIZE ) || ( nM < MODEL_SIZE ) )  // verify strings size limits for output data
    {
    	return status;
	}
    
	DWORD eax, ebx, ecx, edx;
	status = ( f->DLL_CheckCpuid )( );
	if ( status != 0 )
	{
		// Get platform parameters: get CPU strings
		( f->DLL_ExecuteCpuid )( 0, 0, &eax, &ebx, &ecx, &edx );
		typedef union {
		CHAR s8[52]; DWORD s32[13];
		} CPUID_STRING;
		CPUID_STRING cpuidString;
		// Vendor string
		cpuidString.s32[0] = ebx;
		cpuidString.s32[1] = edx;
		cpuidString.s32[2] = ecx;
		cpuidString.s32[3] = 0;
		CHAR *p1 = cpuVendorString;
		CHAR *p2 = cpuidString.s8;
		while( *p1++ = *p2++ );
		// Model string
		strcpy( cpuidString.s8 , "n/a" );
		( f->DLL_ExecuteCpuid )( 0x80000000, 0, &eax, &ebx, &ecx, &edx );
		if ( eax >= 0x80000004 )
		{
			DWORD function = 0x80000002;
			int i = 0;
			for( i=0; i<12; i+=4 )
			{
				( f->DLL_ExecuteCpuid )( function, 0, &eax, &ebx, &ecx, &edx );
				cpuidString.s32[i+0] = eax;
				cpuidString.s32[i+1] = ebx;
				cpuidString.s32[i+2] = ecx;
				cpuidString.s32[i+3] = edx;
				function++;
			}
			cpuidString.s32[i] = 0;
			p1 = cpuidString.s8;
			p2 = cpuModelString;
			BOOLEAN flag = FALSE;
			while ( *p1 != 0 )
			{
				if ( ( *p1 != ' ' ) || ( flag == TRUE ) )
				{
					flag = TRUE;
					*p2 = *p1;
					p2++;
				}
				p1++;	
			}
			*p2 = 0;
		}
	}
	return status;
}

// Build bitmap of supported read-write methods, CPU bitmap = f( CPUID features )
void ProcessorDetector::buildCpuBitmap( DWORD64 &cpuBitmap )
{
	cpuBitmap = 0;
	int i = 0;
	BOOLEAN featureSupported = FALSE;
	do
	{
		featureSupported = getCpuidFeature( x[i].function, x[i].subfunction, x[i].reg, x[i].testBitmap );
		if ( ( x[i].maskType == UNCOND ) || ( x[i].maskType == UNCONDLAST ) )
		{
			cpuBitmap |= x[i].patchBitmap;
		}
		else if ( ( x[i].maskType == ORMASK ) || ( x[i].maskType == ORMASKLAST ) )
		{
			if (featureSupported)
			{
				cpuBitmap |= x[i].patchBitmap;
			}
		}
		else if ( ( x[i].maskType == ANDMASK ) || ( x[i].maskType == ANDMASKLAST ) )
		{
			if (!featureSupported)
			{
				cpuBitmap &= ( ~ ( x[i].patchBitmap ) );
			}
		}
		i++;
	} while ( ( x[i-1].maskType != ORMASKLAST )&&( x[i-1].maskType != ANDMASKLAST )&&( x[i-1].maskType != UNCONDLAST ) );
}

// Helper method for detect CPUID features
BOOL ProcessorDetector::getCpuidFeature( DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask )
{
	BOOL result = FALSE;
	DWORD eax=0, ebx=0, ecx=0, edx=0;
	DWORD temp = 0;
	( f->DLL_ExecuteCpuid )( function&0x80000000, 0, &eax, &ebx, &ecx, &edx );
	if ( eax >= function )
	{
		( f->DLL_ExecuteCpuid )( function, subfunction, &eax, &ebx, &ecx, &edx );
		switch (reg)
		{
			case EAX:
				temp = eax;
				break;
			case EBX:
				temp = ebx;
				break;
			case ECX:
				temp = ecx;
				break;
			case EDX:
				temp = edx;
				break;
			default:
				temp=0;
				break;
		}
		temp &= bitmask;
		result = ( temp == bitmask );
	}
	return result;
}

// Build bitmap of supported read-write methods, OS bitmap = f( OS context management features )
void ProcessorDetector::buildOsBitmap( DWORD64 &osBitmap )
{
    osBitmap = 0;
	int i = 0;
	BOOLEAN featureSupported = FALSE;
	do
	{
	featureSupported = getXgetbvFeature( y[i].testBitmap );
	if ( ( y[i].maskType == UNCOND ) || ( y[i].maskType == UNCONDLAST ) )
		{
			osBitmap |= y[i].patchBitmap;
		}
		else if ( ( y[i].maskType == ORMASK ) || ( y[i].maskType == ORMASKLAST ) )
		{
			if (featureSupported)
			{
				osBitmap |= y[i].patchBitmap;
			}
		}
		else if ( ( y[i].maskType == ANDMASK ) || ( y[i].maskType == ANDMASKLAST ) )
		{
			if ( !featureSupported )
			{
				osBitmap &= ( ~ ( y[i].patchBitmap ) );
			}
		}
	i++;
	} while ( ( y[i-1].maskType != ORMASKLAST )&&( y[i-1].maskType != ANDMASKLAST )&&( y[i-1].maskType != UNCONDLAST ) );
}

// Helper method for detect OS context management features
BOOL ProcessorDetector::getXgetbvFeature( DWORD bitmask )
{
    BOOL result = FALSE;
	DWORDLONG temp = 0;
	result = getCpuidFeature( 0x00000001, 0x00000000, ECX, ((DWORD)1)<<27 );
	if ( result )
	{
		( f->DLL_ExecuteXgetbv )( &temp );
		temp &= bitmask;
		result = ( temp == bitmask );
	}
	return result;
}


