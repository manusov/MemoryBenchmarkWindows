#include "DecoderCpuid.h"

// Pointer to native DLL
SYSTEM_FUNCTIONS_LIST* DecoderCpuid::pF;

// Status string
#define NS 81
char DecoderCpuid::s[NS];

// Control structure for detect optional supported CPU features
const CPUID_CONDITION DecoderCpuid::x[] = {
	// Unconditional x86-64 instructions
	{ ORMASK      , 0          , 0          , NOREG  , 0                       , CPU_FEATURES_UNCONDITIONAL },
	// SSE required
	{ ORMASK      , 0x00000001 , 0x00000000 , EDX    , 1<<25                   , CPU_FEATURES_SSE128        },
	// AVX256 include context management required
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , (1<<27)|(1<<28)         , CPU_FEATURES_AVX256        },
	// AVX512 include context management required
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1<<16                   , CPU_FEATURES_AVX512        },
    { ANDMASK     , 0x00000001 , 0x00000000 , ECX    , 1<<27                   , CPU_FEATURES_AVX512        },
    // FMA256 include context management required
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , (1<<27)|(1<<28)|(1<<12) , CPU_FEATURES_FMA256        },
	// FMA512 include context management required
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1<<16                   , CPU_FEATURES_FMA512        },
	{ ANDMASK     , 0x00000001 , 0x00000000 , ECX    , (1<<27)|(1<<12)         , CPU_FEATURES_FMA512        },
	// SSE non-temporal read by MOVNTDQA (SSE4.1)
	{ ORMASK      , 0x00000001 , 0x00000000 , ECX    , 1<<19                   , CPU_FEATURES_MOVNTDQA128   },
	// AVX256 non-temporal read by VMOVNTDQA (AVX2) include context management required
	{ ORMASK      , 0x00000007 , 0x00000000 , EBX    , 1<<5                    , CPU_FEATURES_MOVNTDQA256   },
	{ ANDMASKLAST , 0x00000001 , 0x00000000 , ECX    , 1<<27                   , CPU_FEATURES_AVX512        }
};

// Control structure for detect optional supported OS context management features
const XGETBV_CONDITION DecoderCpuid::y[] = {
	{ UNCOND     , 0                           , CPU_FEATURES_UNCONDITIONAL },
	{ UNCOND     , 0                           , CPU_FEATURES_SSE128        },
	{ UNCOND     , 0                           , CPU_FEATURES_MOVNTDQA128   },
	{ ORMASK     , 1<<2                        , CPU_FEATURES_AVX256        },
	{ ORMASK     , (1<<2)|(1<<5)|(1<<6)|(1<<7) , CPU_FEATURES_AVX512        },
	{ ORMASK     , 1<<2                        , CPU_FEATURES_FMA256        },
	{ ORMASK     , (1<<2)|(1<<5)|(1<<6)|(1<<7) , CPU_FEATURES_FMA512        },
	{ ORMASKLAST , 1<<2                        , CPU_FEATURES_MOVNTDQA256   }
};


// Class constructor
DecoderCpuid::DecoderCpuid( SYSTEM_FUNCTIONS_LIST* pFunctions )
{
	pF = pFunctions;
	snprintf( s, NS, "no data" );
}

// Class destructor
DecoderCpuid::~DecoderCpuid( )
{
	
}

// Get processor vendor and model strings, by CPUID instruction
BOOL DecoderCpuid::getVendorAndModel( char* cpuVendorString, char* cpuModelString, int NV, int NM )
{
    BOOL status = FALSE;
    if ( ( NV < 13 ) || ( NM < 49 ) )  // verify strings size limits for output data
    {
    	return status;
	}
    
	DWORD eax, ebx, ecx, edx;
	status = ( pF->DLL_CheckCpuid )( );
	if ( status != 0 )
	{
		// Get platform parameters: get CPU strings
		( pF->DLL_ExecuteCpuid )( 0, 0, &eax, &ebx, &ecx, &edx );
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
		( pF->DLL_ExecuteCpuid )( 0x80000000, 0, &eax, &ebx, &ecx, &edx );
		if ( eax >= 0x80000004 )
		{
			DWORD function = 0x80000002;
			int i = 0;
			for( i=0; i<12; i+=4 )
			{
				( pF->DLL_ExecuteCpuid )( function, 0, &eax, &ebx, &ecx, &edx );
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
void DecoderCpuid::buildCpuBitmap( DWORD64 &cpuBitmap )
{
	cpuBitmap = 0;
	int i = 0;
	BOOLEAN featureSupported = FALSE;
	do
	{
		featureSupported = getCpuidFeature( x[i].function, x[i].subfunction, x[i].reg, x[i].testBitmap );
		if ( x[i].maskType == UNCOND )
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
BOOL DecoderCpuid::getCpuidFeature( DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask )
{
	BOOL result = FALSE;
	DWORD eax=0, ebx=0, ecx=0, edx=0;
	DWORD temp = 0;
	( pF->DLL_ExecuteCpuid )( function&0x80000000, 0, &eax, &ebx, &ecx, &edx );
	if ( eax >= function )
	{
		( pF->DLL_ExecuteCpuid )( function, subfunction, &eax, &ebx, &ecx, &edx );
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
void DecoderCpuid::buldOsBitmap( DWORD64 &osBitmap )
{
    osBitmap = 0;
	int i = 0;
	BOOLEAN featureSupported = FALSE;
	do
	{
	featureSupported = getXgetbvFeature( y[i].testBitmap );
	if ( y[i].maskType == UNCOND )
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
BOOL DecoderCpuid::getXgetbvFeature( DWORD bitmask )
{
    BOOL result = FALSE;
	DWORDLONG temp = 0;
	result = getCpuidFeature( 0x00000001, 0x00000000, ECX, ((DWORD)1)<<27 );
	if ( result )
	{
		( pF->DLL_ExecuteXgetbv )( &temp );
		temp &= bitmask;
		result = ( temp == bitmask );
	}
	return result;
}

// This method used for auto select method, for temporal mode (cacheable)
DWORD DecoderCpuid::findMaxMethodTemporal( DWORD64 bitmapCpu, DWORD64 bitmapOs )
{
    DWORD64 bitmap = bitmapCpu & bitmapOs;
    DWORD mask = 0;
    DWORD select = 0;
    // Try AVX512 read memory method
    select = CPU_FEATURE_READ_AVX512;
    mask = ( ( DWORDLONG )1 ) << select;
    if ( ! ( mask & bitmap ) )
    {
        select = CPU_FEATURE_READ_AVX256;
    }
    // Try AVX256 read memory method
    mask = ( ( DWORDLONG )1 ) << select;
    if ( ! ( mask & bitmap ) )
    {
        select = CPU_FEATURE_READ_SSE128;
    }
    // Try SSE128 read memory method
    mask = ( ( DWORDLONG )1 ) << select;
    if ( ! ( mask & bitmap ) )
    {
        select = CPU_FEATURE_READ_IA32_X64;
    }
    // Try common x86 or x86-64 read memory method
    mask = ( ( DWORDLONG )1 ) << select;
    if ( ! ( mask & bitmap ) )
    {
        select = OPTION_NOT_SET;
    }
    return select;
}

// This method used for auto select method, for non-temporal mode (uncacheable)
DWORD DecoderCpuid::findMaxMethodNonTemporal( DWORD64 bitmapCpu, DWORD64 bitmapOs )
{
    DWORD64 bitmap = bitmapCpu & bitmapOs;
    DWORD mask = 0;
    DWORD select = 0;
    // Try AVX512 read memory method
    select = CPU_FEATURE_NTRW_READ_AVX512,
    mask = ( ( DWORDLONG )1 ) << select;
    if ( ! ( mask & bitmap ) )
    {
        select = CPU_FEATURE_NTPRW_READ_AVX256;
    }
    // Try AVX256 read memory method
    mask = ( ( DWORDLONG )1 ) << select;
    if ( ! ( mask & bitmap ) )
    {
        select = CPU_FEATURE_NTPRW_READ_SSE128;
    }
    // Try SSE128 read memory method
    mask = ( ( DWORDLONG )1 ) << select;
    if ( ! ( mask & bitmap ) )
    {
        select = CPU_FEATURE_READ_IA32_X64;
    }
    // Try common x86 or x86-64 read memory method
    mask = ( ( DWORDLONG )1 ) << select;
    if ( ! ( mask & bitmap ) )
    {
        select = OPTION_NOT_SET;
    }
    return select;
}

// Method returns status string, valid if error returned
char* DecoderCpuid::getStatusString( )
{
    return s;
}


