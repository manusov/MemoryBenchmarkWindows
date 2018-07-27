/*
 *   Get and show CPU information.
 */

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

