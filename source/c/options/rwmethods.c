/*
 *   Read-write methods options names strings and control data.
 */

// Text strings for option recognition and visual
static char* rwMethods[] = { 
	"readmov64", "writemov64", "copymov64", "modifynot64",
	"writestring64", "copystring64",
	"readsse128", "writesse128", "copysse128",
	"readavx256", "writeavx256", "copyavx256",
	"readavx512", "writeavx512", "copyavx512",
	"dotfma256", "dotfma512",
	NULL
};

// See associated definitions at constants.h
static char* rwMethodsDetails[] = {
	"Read x86-64 (MOV)",
	"Write x86-64 (MOV)",
	"Copy x86-64 (MOV)",
	"Modify x86-64 (NOT)",
	"Write x86-64 strings (REP STOSQ)",
	"Copy x86-64 strings (REP MOVSQ)",
	"Read SSE-128 (MOVAPS)",
	"Write SSE-128 (MOVAPS)",
	"Copy SSE-128 (MOVAPS)",
	"Read AVX-256 (VMOVAPD)",
	"Write AVX-256 (VMOVAPD)",
	"Copy AVX-256 (VMOVAPD)",
	"Read AVX-512 (VMOVAPD)",
	"Write AVX-512 (VMOVAPD)",
	"Copy AVX-512 (VMOVAPD)",
	"Dot product FMA-256 (VFMADD231)",
	"Dot product FMA-512 (VFMADD231)",
	// Additions for non-temporal Write
	"Non-temporal write SSE-128 (MOVNTPS)",
	"Non-temporal copy SSE-128 (MOVAPS+MOVNTPS)",    // this duplicated 1
	"Non-temporal write AVX-256 (VMOVNTPD)",
	"Non-temporal copy AVX-256 (VMOVAPD+VMOVNTPD)",    // this duplicated 2, wrong
	"Non-temporal write AVX-512 (VMOVNTPD)",
	"Non-temporal copy AVX-512 (VMOVAPD+VMOVNTPD)",
	// Additions for non-temporal Read
	"Non-temporal read SSE-128 (MOVNTDQA)",
	"Non-temporal copy SSE-128 (MOVNTDQA+MOVNTPD)",
	"Non-temporal read AVX-256 (VMOVNTDQA)",
	"Non-temporal copy AVX-256 (VMOVNTDQA+VMOVNTPD)",
	"Non-temporal read AVX-512 (VMOVNTDQA)",
	"Non-temporal copy AVX-512 (VMOVNTDQA+VMOVNTPD)",
	// Additions for non-temporal Read if CPU not supports (V)MOVNTDQA
	// also if (V)MOVNTDQA is supported, but non-optimal
	// based on PREFETCHNTA hint
	"Non-temporal read SSE-128 (PREFETCHNTA+MOVAPS)",
	"Non-temporal copy SSE-128 (PREFETCHNTA+MOVAPS+MOVNTPS)",  // this duplicated 1
	"Non-temporal read AVX-256 (PREFETCHNTA+VMOVAPD)",
	// Reserved for extensions
	// "Non-temporal copy AVX-256 (PREFETCHNTA+VMOVAPD)"
	// "Non-temporal read AVX-512 (PREFETCHNTA+VMOVAPD)"
	// "Non-temporal copy AVX-512 (PREFETCHNTA+VMOVAPD)"
	// Reserved for FMA with non-temporal store
	NULL
};

// Control structure for detect optional supported CPU features
static CPUID_CONDITION cpuidControl[] = {
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
static XGETBV_CONDITION xgetbvControl[] = {
	{ UNCOND     , 0                           , CPU_FEATURES_UNCONDITIONAL },
	{ UNCOND     , 0                           , CPU_FEATURES_SSE128        },
	{ UNCOND     , 0                           , CPU_FEATURES_MOVNTDQA128   },
	{ ORMASK     , 1<<2                        , CPU_FEATURES_AVX256        },
	{ ORMASK     , (1<<2)|(1<<5)|(1<<6)|(1<<7) , CPU_FEATURES_AVX512        },
	{ ORMASK     , 1<<2                        , CPU_FEATURES_FMA256        },
	{ ORMASK     , (1<<2)|(1<<5)|(1<<6)|(1<<7) , CPU_FEATURES_FMA512        },
	{ ORMASKLAST , 1<<2                        , CPU_FEATURES_MOVNTDQA256   }
};


DWORDLONG buildCpuidBitmap( CPUID_CONDITION x[] )
{
	DWORDLONG y = 0;
	int i = 0;
	BOOLEAN featureSupported = FALSE;
	do
	{
		featureSupported = getCpuidFeature( x[i].function, x[i].subfunction, x[i].reg, x[i].testBitmap );
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

DWORDLONG buildXgetbvBitmap( XGETBV_CONDITION x[] )
{
	DWORDLONG y = 0;
	int i = 0;
	BOOLEAN featureSupported = FALSE;
	do
	{
	featureSupported = getXgetbvFeature( x[i].testBitmap );
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

void detectMethods( DWORD *select, DWORDLONG *bitmap, DWORDLONG *bitmapCpu, DWORDLONG *bitmapOs )
{
	*bitmapCpu = buildCpuidBitmap( cpuidControl );
	*bitmapOs = buildXgetbvBitmap( xgetbvControl );
	*bitmap = ( *bitmapCpu ) & ( *bitmapOs );
	DWORD mask = 0;
	// Try AVX512 read memory method
	*select = CPU_FEATURE_READ_AVX512;
	mask = ((DWORDLONG)1) << *select;
	if ( ! (mask & *bitmap) )
	{
		*select = CPU_FEATURE_READ_AVX256;
	}
	// Try AVX256 read memory method
	mask = ((DWORDLONG)1) << *select;
	if ( ! (mask & *bitmap) )
	{
		*select = CPU_FEATURE_READ_SSE128;
	}
	// Try SSE128 read memory method
	mask = ((DWORDLONG)1) << *select;
	if ( ! (mask & *bitmap) )
	{
		*select = CPU_FEATURE_READ_X64;
	}
	// Try common x86-64 read memory method
	mask = ((DWORDLONG)1) << *select;
	if ( ! (mask & *bitmap) )
	{
		*select = -1;
	}
}

void printMethods( DWORD select, DWORDLONG bitmap, DWORDLONG bitmapCpu, DWORDLONG bitmapOs, CHAR *methodsNames[] )
{
	printf("\n-------------------------------------------------------------------");
	printf("\n  # CPU OS Operation and CPU instruction used");
	printf("\n-------------------------------------------------------------------");
	int i = 0;
	int a, b;
	while ( methodsNames[i] != NULL )
	{
		a = bitmapCpu & 1;
		b = bitmapOs & 1;
		printf("\n %2d  %-2d %-2d %s", i, a, b, methodsNames[i] );
		bitmapCpu = bitmapCpu >> 1;
		bitmapOs = bitmapOs >> 1;
		i++;
	}
	printf("\n-------------------------------------------------------------------");
	printf("\nDefault selection = %d , bitmap = %08X%08Xh", select, bitmap >> 32, bitmap );
}


