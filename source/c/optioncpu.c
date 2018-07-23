/*
 *   Regular detector for optional functionality depend by CPU features
 */

BOOLEAN getCpuidFeature( DWORD function, DWORD subfunction, CPUID_OUTPUT reg, DWORD bitmask )
{
	BOOLEAN result = FALSE;
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

BOOLEAN getXgetbvFeature( DWORD bitmask )
{
	BOOLEAN result = FALSE;
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

