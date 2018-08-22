/*
 *    Detect CPU, visual CPU vendor and model strings.
 */

// Definitions

typedef enum {
	ORMASK , ANDMASK , ORMASKLAST , ANDMASKLAST, UNCOND, UNCONDLAST
} MASK_TYPE;

typedef enum {
	EAX, EBX, ECX, EDX
} CPUID_OUTPUT;

typedef struct {
	MASK_TYPE maskType;
	DWORD function;
	DWORD subfunction;
	CPUID_OUTPUT reg;
	DWORD testBitmap;
	DWORDLONG patchBitmap;
} CPUID_CONDITION;

typedef struct {
	MASK_TYPE maskType;
	DWORD testBitmap;
	DWORDLONG patchBitmap;
} XGETBV_CONDITION;

// This features unconditionally supported by x64 CPU
#define CPU_FEATURES_UNCONDITIONAL                      \
((DWORDLONG)1) << CPU_FEATURE_READ_IA32_X64 |           \
((DWORDLONG)1) << CPU_FEATURE_WRITE_IA32_X64 |          \
((DWORDLONG)1) << CPU_FEATURE_COPY_IA32_X64 |           \
((DWORDLONG)1) << CPU_FEATURE_MODIFY_IA32_X64 |         \
((DWORDLONG)1) << CPU_FEATURE_WRITE_STRINGS_IA32_X64 |  \
((DWORDLONG)1) << CPU_FEATURE_COPY_STRINGS_IA32_X64

// This features validated by SSE feature bit:
// CPUID function 00000001h register EDX bit 25
#define CPU_FEATURES_SSE128                        \
((DWORDLONG)1) << CPU_FEATURE_READ_SSE128 |        \
((DWORDLONG)1) << CPU_FEATURE_WRITE_SSE128 |       \
((DWORDLONG)1) << CPU_FEATURE_COPY_SSE128 |        \
((DWORDLONG)1) << CPU_FEATURE_NTW_WRITE_SSE128 |   \
((DWORDLONG)1) << CPU_FEATURE_NTW_COPY_SSE128 |    \
((DWORDLONG)1) << CPU_FEATURE_NTW_WRITE_SSE128 |   \
((DWORDLONG)1) << CPU_FEATURE_NTW_COPY_SSE128 |    \
((DWORDLONG)1) << CPU_FEATURE_NTPRW_READ_SSE128 |  \
((DWORDLONG)1) << CPU_FEATURE_NTPRW_COPY_SSE128

// This features validated by AVX256 feature bit:
// CPUID function 00000001h register ECX bit 28 = AVX,
// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
// conjunction with bit XCR0.2 = AVX state
#define CPU_FEATURES_AVX256                        \
((DWORDLONG)1) << CPU_FEATURE_READ_AVX256 |        \
((DWORDLONG)1) << CPU_FEATURE_WRITE_AVX256 |       \
((DWORDLONG)1) << CPU_FEATURE_COPY_AVX256 |        \
((DWORDLONG)1) << CPU_FEATURE_NTW_WRITE_AVX256 |   \
((DWORDLONG)1) << CPU_FEATURE_NTW_COPY_AVX256 |    \
((DWORDLONG)1) << CPU_FEATURE_NTPRW_READ_AVX256
// ((DWORDLONG)1) << CPU_FEATURE_NTPRW_COPY_AVX256

// This features validated by AVX512F feature bit:
// CPUID function 7 subfunction 0 register EBX bit 16 = AVX512F,
// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
// conjunction with bit XCR0. [2,5,6,7]
#define CPU_FEATURES_AVX512                        \
((DWORDLONG)1) << CPU_FEATURE_READ_AVX512 |        \
((DWORDLONG)1) << CPU_FEATURE_WRITE_AVX512 |       \
((DWORDLONG)1) << CPU_FEATURE_COPY_AVX512 |        \
((DWORDLONG)1) << CPU_FEATURE_NTW_WRITE_AVX512 |   \
((DWORDLONG)1) << CPU_FEATURE_NTW_COPY_AVX512 |    \
((DWORDLONG)1) << CPU_FEATURE_NTRW_READ_AVX512 |   \
((DWORDLONG)1) << CPU_FEATURE_NTRW_COPY_AVX512
// ((DWORDLONG)1) << CPU_FEATURE_NTPRW_READ_AVX512 |  \
// ((DWORDLONG)1) << CPU_FEATURE_NTPRW_COPY_AVX512 |  \

// This features validated by AVX256 and FMA features bits conjunction:
// CPUID function 00000001h register ECX bit 28 = AVX,
// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
// CPUID function 00000001h register ECX bit 12 = FMA,
// conjunction with bit XCR0.2 = AVX state
#define CPU_FEATURES_FMA256                        \
((DWORDLONG)1) << CPU_FEATURE_DOT_FMA256

// This features validated by AVX512 and FMA features bits conjunction:
// CPUID function 7 subfunction 0 register EBX bit 16 = AVX512,
// CPUID function 00000001h register ECX bit 12 = FMA,
// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
// conjunction with bit XCR0. [2,5,6,7]
#define CPU_FEATURES_FMA512                        \
((DWORDLONG)1) << CPU_FEATURE_DOT_FMA512

// This features validated by SSE4.1 feature bit:
// CPUID function 00000001h register ECX bit 19 = SSE4.1
#define CPU_FEATURES_MOVNTDQA128                   \
((DWORDLONG)1) << CPU_FEATURE_NTRW_READ_SSE128 |   \
((DWORDLONG)1) << CPU_FEATURE_NTRW_COPY_SSE128

// This features validated by AVX2 feature bit:
// CPUID function 7 subfunction 0 register EBX bit 5 = AVX2,
// conjunction with CPUID function 00000001h register ECX bit 27 = OSXSAVE
// conjunction with bit XCR0.2 = AVX state
#define CPU_FEATURES_MOVNTDQA256                   \
((DWORDLONG)1) << CPU_FEATURE_NTRW_READ_AVX256 |   \
((DWORDLONG)1) << CPU_FEATURE_NTRW_COPY_AVX256

// Control structure for detect optional supported CPU features
CPUID_CONDITION cpuidControl[] = {
	// Unconditional x86-64 instructions
	{ ORMASK      , 0          , 0          , UNCOND , 0                       , CPU_FEATURES_UNCONDITIONAL },
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
XGETBV_CONDITION xgetbvControl[] = {
	{ UNCOND     , 0                           , CPU_FEATURES_UNCONDITIONAL },
	{ UNCOND     , 0                           , CPU_FEATURES_SSE128        },
	{ UNCOND     , 0                           , CPU_FEATURES_MOVNTDQA128   },
	{ ORMASK     , 1<<2                        , CPU_FEATURES_AVX256        },
	{ ORMASK     , (1<<2)|(1<<5)|(1<<6)|(1<<7) , CPU_FEATURES_AVX512        },
	{ ORMASK     , 1<<2                        , CPU_FEATURES_FMA256        },
	{ ORMASK     , (1<<2)|(1<<5)|(1<<6)|(1<<7) , CPU_FEATURES_FMA512        },
	{ ORMASKLAST , 1<<2                        , CPU_FEATURES_MOVNTDQA256   }
};

// Local (not declared at header) helpers

BOOL detectCpu( LIST_DLL_FUNCTIONS* xf, CHAR *cpuVendorString, CHAR *cpuModelString )
{
	BOOL status = 0;
	DWORD eax, ebx, ecx, edx;
	status = ( xf->DLL_CheckCpuid )( );
	if ( status != 0 )
	{
		// Get platform parameters: get CPU strings
		( xf->DLL_ExecuteCpuid )( 0, 0, &eax, &ebx, &ecx, &edx );
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
		( xf->DLL_ExecuteCpuid )( 0x80000000, 0, &eax, &ebx, &ecx, &edx );
		if ( eax >= 0x80000004 )
		{
			DWORD function = 0x80000002;
			int i = 0;
			for( i=0; i<12; i+=4 )
			{
				( xf->DLL_ExecuteCpuid )( function, 0, &eax, &ebx, &ecx, &edx );
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

void printCpu( CHAR *cpuVendorString, CHAR *cpuModelString )
{
	printf( "\nCPU vendor = %s", cpuVendorString );
	printf( "\nCPU model = %s", cpuModelString );
}

// CPUID/RDTSC/XGETBV helpers functions

BOOL getCpuidFeature( LIST_DLL_FUNCTIONS* xf, 
                      DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask )
{
	BOOL result = FALSE;
	DWORD eax=0, ebx=0, ecx=0, edx=0;
	DWORD temp = 0;
	( xf->DLL_ExecuteCpuid )( function&0x80000000, 0, &eax, &ebx, &ecx, &edx );
	if ( eax >= function )
	{
		( xf->DLL_ExecuteCpuid )( function, subfunction, &eax, &ebx, &ecx, &edx );
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

BOOL getXgetbvFeature( LIST_DLL_FUNCTIONS* xf, 
                       DWORD bitmask )
{
	BOOL result = FALSE;
	DWORDLONG temp = 0;
	result = getCpuidFeature( xf, 0x00000001, 0x00000000, ECX, ((DWORD)1)<<27 );
	if (result)
	{
		( xf->DLL_ExecuteXgetbv )( &temp );
		temp &= bitmask;
		result = ( temp == bitmask );
	}
	return result;
}

DWORDLONG buildCpuidBitmap( LIST_DLL_FUNCTIONS* xf,
                            CPUID_CONDITION x[] )
{
	DWORDLONG y = 0;
	int i = 0;
	BOOLEAN featureSupported = FALSE;
	do
	{
		featureSupported = getCpuidFeature( xf, x[i].function, x[i].subfunction, x[i].reg, x[i].testBitmap );
		if ( x[i].maskType == UNCOND )
		{
			y |= x[i].patchBitmap;
		}
		else if ( ( x[i].maskType == ORMASK ) || ( x[i].maskType == ORMASKLAST ) )
		{
			if (featureSupported)
			{
				y |= x[i].patchBitmap;
			}
		}
		else if ( ( x[i].maskType == ANDMASK ) || ( x[i].maskType == ANDMASKLAST ) )
		{
			if (!featureSupported)
			{
				y &= ( ~ ( x[i].patchBitmap ) );
			}
		}
		i++;
	} while ( ( x[i-1].maskType != ORMASKLAST )&&( x[i-1].maskType != ANDMASKLAST )&&( x[i-1].maskType != UNCONDLAST ) );
	
	return y;
}

DWORDLONG buildXgetbvBitmap( LIST_DLL_FUNCTIONS* xf, 
                             XGETBV_CONDITION x[] )
{
	DWORDLONG y = 0;
	int i = 0;
	BOOLEAN featureSupported = FALSE;
	do
	{
	featureSupported = getXgetbvFeature( xf, x[i].testBitmap );
	if ( x[i].maskType == UNCOND )
		{
			y |= x[i].patchBitmap;
		}
		else if ( ( x[i].maskType == ORMASK ) || ( x[i].maskType == ORMASKLAST ) )
		{
			if (featureSupported)
			{
				y |= x[i].patchBitmap;
			}
		}
		else if ( ( x[i].maskType == ANDMASK ) || ( x[i].maskType == ANDMASKLAST ) )
		{
			if (!featureSupported)
			{
				y &= ( ~ ( x[i].patchBitmap ) );
			}
		}
	i++;
	} while ( ( x[i-1].maskType != ORMASKLAST )&&( x[i-1].maskType != ANDMASKLAST )&&( x[i-1].maskType != UNCONDLAST ) );
	
	return y;
}

DWORD findMaxMethod( DWORDLONG bitmapCpu, DWORDLONG bitmapOs )
{
    DWORD64 bitmap = bitmapCpu & bitmapOs;
    DWORD mask = 0;
    DWORD select = 0;
    // Try AVX512 read memory method
    select = CPU_FEATURE_READ_AVX512;
    mask = ((DWORDLONG)1) << select;
    if ( ! (mask & bitmap) )
    {
        select = CPU_FEATURE_READ_AVX256;
    }
    // Try AVX256 read memory method
    mask = ((DWORDLONG)1) << select;
    if ( ! (mask & bitmap) )
    {
        select = CPU_FEATURE_READ_SSE128;
    }
    // Try SSE128 read memory method
    mask = ((DWORDLONG)1) << select;
    if ( ! (mask & bitmap) )
    {
        select = CPU_FEATURE_READ_IA32_X64;
    }
    // Try common x86-64 read memory method
    mask = ((DWORDLONG)1) << select;
    if ( ! (mask & bitmap) )
    {
        select = NOT_SET;
    }
    return select;
}

// End of local (not declared at header) helpers

// Step action routine

void stepDetectCpu( LIST_DLL_FUNCTIONS* xf, MPE_CPU_BITMAP* xb, LIST_RELEASE_RESOURCES* xr )
{
    CHAR cpuVendorString[13];
    CHAR cpuModelString[49];
    int N = 100;
    CHAR p[N+1];
    BOOL status = FALSE;
    // Detect CPUID supported and not locked, get vendor and model strings, show
    status = detectCpu( xf, cpuVendorString, cpuModelString );
     if ( status == FALSE )
    {
        helperRelease( xr );
        snprintf( p, N, "CPUID instruction not supported or locked" );
        exitWithInternalError( p );
    }
    else
    {
        printCpu( cpuVendorString, cpuModelString );
    }
    // Detect CPU and OS context functionality bitmaps, show
    xb->bitmapCpu = buildCpuidBitmap( xf, cpuidControl );
    xb->bitmapOs = buildXgetbvBitmap( xf, xgetbvControl );
    xb->maxMethod = findMaxMethod( xb->bitmapCpu, xb->bitmapOs );
    DWORD x1 = ( ( xb->bitmapCpu ) >> 16 ) >> 16;
    DWORD x2 = xb->bitmapCpu;
    DWORD x3 = ( ( xb->bitmapOs ) >> 16 ) >> 16;
    DWORD x4 = xb->bitmapOs;
    printf( "\nCPU vector = %08X%08Xh, OS vector = %08X%08Xh", x1, x2, x3, x4 );
}


