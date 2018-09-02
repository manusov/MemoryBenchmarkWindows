/*
 *   Memory Performance Engine.
 *   Handlers for system information and benchmark patterns.
 *   Object = CPU and native platform support DLL.
 */

// Native functions DLL handle
HINSTANCE hDLL = NULL;
// DLL entry points declarations, see parameters comments at assembler DLL
void ( __stdcall *DLL_GetDllStrings   ) ( char** , char** , char** );
BOOL ( __stdcall *DLL_CheckCpuid      ) ( void );
void ( __stdcall *DLL_ExecuteCpuid    ) ( DWORD, DWORD, DWORD* , DWORD* , DWORD* , DWORD* );
void ( __stdcall *DLL_ExecuteXgetbv   ) ( DWORDLONG* );
BOOL ( __stdcall *DLL_MeasureTsc      ) ( DWORDLONG* );
BOOL ( __stdcall *DLL_PerformanceGate ) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* );
// DLL and Functions names strings: for load and for error reporting
CHAR* dllName = "mpe_w_64.dll";
CHAR* fname1 = "GetDllStrings";
CHAR* fname2 = "CheckCpuid";
CHAR* fname3 = "ExecuteCpuid";
CHAR* fname4 = "ExecuteXgetbv";
CHAR* fname5 = "MeasureTsc";
CHAR* fname6 = "PerformanceGate";
CHAR* functionName;
// CPU information
CHAR cpuVendorString[13];
CHAR cpuModelString[49];
DWORDLONG tscClk = 0;
double tscHz = 0.0;
double tscNs = 0.0;

// Initializing native CPU support, load DLL and functions
DWORD initCpu( CHAR* returnText )
{
    // Load DLL
    hDLL = LoadLibrary( dllName );
	if( hDLL == NULL )
    {
        int N = 100;
        CHAR p[N+1];
        snprintf( p, N, "load module=%s", dllName );
        exitWithSystemError( p );	
    }
    // Load function: GetDllStrings
    functionName = fname1;
    DLL_GetDllStrings = ( void (__stdcall *) ( char** , char** , char** ) )
    GetProcAddress( hDLL, functionName );
    dllFunctionCheck( DLL_GetDllStrings, functionName, dllName );
    // Load function: CheckCpuid
    functionName = fname2;
    DLL_CheckCpuid = ( BOOL (__stdcall *) ( void ) )
    GetProcAddress( hDLL, functionName );
    dllFunctionCheck( DLL_CheckCpuid, functionName, dllName );
    // Load function: ExecuteCpuid
    functionName = fname3;
    DLL_ExecuteCpuid = ( void (__stdcall *) ( DWORD, DWORD, DWORD*, DWORD*, DWORD*, DWORD* ) )
    GetProcAddress( hDLL, functionName );
    dllFunctionCheck( DLL_ExecuteCpuid, functionName, dllName );
    // Load function: ExecuteXgetbv
    functionName = fname4;
    DLL_ExecuteXgetbv = ( void (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( hDLL, functionName );
	dllFunctionCheck( DLL_ExecuteXgetbv, functionName, dllName );
    // Load function: MeasureTsc
    functionName = fname5;
    DLL_MeasureTsc = ( BOOL (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( hDLL, functionName );
	dllFunctionCheck( DLL_MeasureTsc, functionName, dllName );
    // Load function: PerformanceGate
    functionName = fname6;
    DLL_PerformanceGate = ( BOOL (__stdcall *) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* ) )
	GetProcAddress( hDLL, functionName );
	dllFunctionCheck( DLL_PerformanceGate, functionName, dllName );
    // Exit
    return STATUS_OK;
}

// De-initializing native CPU support, unload DLL
DWORD deinitCpu( CHAR* returnText )
{
    BOOL status = FALSE;
    if ( hDLL != NULL )
    {
        status = FreeLibrary( hDLL );
        if ( status == FALSE )
        {
        int N = 100;
        CHAR p[N+1];
        snprintf( p, N, "unload module=%s", dllName );
        exitWithSystemError( p );	
        }
        return STATUS_OK;
    }
    else
    {
        return STATUS_EMPTY;
    }
}

// Get and print DLL strings: name, version, copyright
DWORD showDll( CHAR* returnText )
{
   	char *dllProduct, *dllVersion, *dllVendor;
	DLL_GetDllStrings( &dllProduct, &dllVersion, &dllVendor );
	printf( "\n%s %s\n%s", dllProduct, dllVersion, dllVendor );
    return STATUS_OK;
}

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

// Build supported read-write methods bitmaps
DWORD bitmapCpu ( CHAR* returnText, MPE_CPU_BITMAP* returnBinary )
{
    int N = 100;
    CHAR p[N+1];
    BOOL status = FALSE;
    status = detectCpu( cpuVendorString, cpuModelString );
    if ( status == FALSE )
    {
        snprintf( p, N, "CPUID instruction not supported or locked" );
        exitWithInternalError( p );
    }
    else
    {
        printCpu( cpuVendorString, cpuModelString );
    }
    returnBinary->bitmapCpu = buildCpuidBitmap( cpuidControl );
	returnBinary->bitmapOs = buildXgetbvBitmap( xgetbvControl );
    return STATUS_OK;
}

// Measure TSC clock
DWORD measureCpu( CHAR* returnText, MPE_CPU_MEASURE* returnBinary )
{
    int N = 100;
    CHAR p[N+1];
    int status = 0;
    status = measureTsc( &tscClk, &tscHz, &tscNs );
    if ( status == 1 )
    {
        snprintf( p, N, "TSC not supported or locked" );
        exitWithInternalError( p );
    }
    else if ( status == 2 )
    {
        snprintf( p, N, "TSC clock measurement failed" );
        exitWithInternalError( p );
    }
    else if ( status != 0 )
    {
        snprintf( p, N, "TSC access unknown error" );
        exitWithInternalError( p );
    }
    else
    {
        printTsc( tscHz, tscNs );
    }
    returnBinary->dtsc = tscClk;
    returnBinary->frequencyTsc = tscHz;
    returnBinary->periodTsc = tscNs;
    return STATUS_OK;
}

// Call performance pattern ( UNUSED YET BECAUSE DIRECT CALL )
DWORD performanceCpu( CHAR* returnText, MPE_CPU_PERFORMANCE* returnBinary )
{
    return STATUS_UNDER_CONSTRUCTION;
}

// Common helpers functions

// verify after load DLL function, if error, show error message and exit
void dllFunctionCheck( void *functionPointer, CHAR *functionName, CHAR *dllName )
{
    if ( functionPointer == NULL )
    {
    	int N = 100;
		CHAR p[N+1];
    	snprintf(p, N, "load function=%s from module=%s", functionName, dllName );
		exitWithSystemError( p );	
	}
}

// CPUID/RDTSC/XGETBV helpers functions

BOOL getCpuidFeature( DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask )
{
	BOOL result = FALSE;
	DWORD eax=0, ebx=0, ecx=0, edx=0;
	DWORD temp = 0;
	DLL_ExecuteCpuid( function&0x80000000, 0, &eax, &ebx, &ecx, &edx );
	if ( eax >= function )
	{
		DLL_ExecuteCpuid( function, subfunction, &eax, &ebx, &ecx, &edx );
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

BOOL getXgetbvFeature( DWORD bitmask )
{
	BOOL result = FALSE;
	DWORDLONG temp = 0;
	result = getCpuidFeature( 0x00000001, 0x00000000, ECX, ((DWORD)1)<<27 );
	if (result)
	{
		DLL_ExecuteXgetbv( &temp );
		temp &= bitmask;
		result = ( temp == bitmask );
	}
	return result;
}

BOOL detectCpu( CHAR *cpuVendorString, CHAR *cpuModelString )
{
	BOOL status = 0;
	DWORD eax, ebx, ecx, edx;
	status = DLL_CheckCpuid();
	if ( status != 0 )
	{
		// Get platform parameters: get CPU strings
		DLL_ExecuteCpuid( 0, 0, &eax, &ebx, &ecx, &edx );
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
		DLL_ExecuteCpuid( 0x80000000, 0, &eax, &ebx, &ecx, &edx );
		if ( eax >= 0x80000004 )
		{
			DWORD function = 0x80000002;
			int i = 0;
			for( i=0; i<12; i+=4 )
			{
				DLL_ExecuteCpuid( function, 0, &eax, &ebx, &ecx, &edx );
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

int measureTsc( DWORDLONG *tscClk, double *tscHz, double *tscNs )
{
	DWORD errorType = 1;  // This error code if TSC not supported or locked
	BOOL status = 0;
	status = getCpuidFeature(0x00000001, 0x00000000, EDX, 0x00000010 );
	if ( status != 0 )
	{
		errorType = 2;  // This error code if measurement error
		status = DLL_MeasureTsc( tscClk );
		if ( status != 0 )
		{
			errorType = 0;    // This error code if no errors
			*tscHz = *tscClk;
			*tscNs = *tscHz;
			*tscNs = 1000000000.0 / *tscNs;
		}
	}
	return errorType;
}

void printCpu( CHAR *cpuVendorString, CHAR *cpuModelString )
{
	printf( "\nCPU Vendor = %s", cpuVendorString );
	printf( "\nCPU Model = %s", cpuModelString );
}

void printTsc( double tscHz, double tscNs )
{
	double tscMHz = tscHz / 1000000.0;
	printf( "\nTSC frequency = %.2f MHz, period = %.3f ns", tscMHz, tscNs );
}

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

