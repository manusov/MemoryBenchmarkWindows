/*
      MEMORY PERFORMANCE ENGINE FRAMEWORK.
    Common library, text strings builder class.
*/

#include "AppLib.h"

// Print string
void AppLib::printString( LPSTR &dst, size_t &max, LPCSTR src )
{
	int count = snprintf( dst, max, "%s", src );
	dst += count;
	max -= count;
	return;
}
// Print string with fixed size, add spaces if required
void AppLib::printCell( LPSTR &dst, LPCSTR src, size_t &max, size_t min )
{
	int count = snprintf( dst, max, "%s", src );
	dst += count;
	max -= count;
	int extra = min - count;
	if ( extra > 0 )
	{
		for( int i=0; i<extra; i++ )
		{
			count = snprintf( dst, max, " " );
			dst += count;
			max -= count;
		}
	}
}
// Print line for tables
void AppLib::printLine( LPSTR &dst, size_t &max, size_t cnt )
{
	int count = 0;
	if ( cnt > 0 )
	{
		for( int i=0; i<cnt; i++ )
		{
			count = snprintf( dst, max, "-" );
			dst += count;
			max -= count;
		}
	}
	count = snprintf( dst, max, "\r\n" );
	dst += count;
	max -= count;
	return;
}
// Print CR, LF, next string
void AppLib::printCrLf( LPSTR &dst, size_t &max )
{
	int count = snprintf( dst, max, "\r\n" );
	dst += count;
	max -= count;
}
// Print decimal number to fixed size cell
void AppLib::printCellNum( LPSTR &dst, size_t &max, DWORD32 x, size_t min )
{
	int count = snprintf( dst, max, "%d", x );
	dst += count;
	max -= count;
	int extra = min - count;
	if ( extra > 0 )
	{
		for( int i=0; i<extra; i++ )
		{
			count = snprintf( dst, max, " " );
			dst += count;
			max -= count;
		}
	}
	
}
// Print 32-bit hex value = x to output string = s with size limit = n
void AppLib::print16( LPSTR &dst, size_t &max, DWORD32 x, BOOL h )
{
	int count = 0;
	if ( h )
	{
		count = snprintf( dst, max, "%04Xh", x );
	}
	else
	{
		count = snprintf( dst, max, "%04X", x );
	}
	dst += count;
	max -= count;
	return;
}
// Print 32-bit hex value = x to output string = s with size limit = n
void AppLib::print32( LPSTR &dst, size_t &max, DWORD32 x, BOOL h )
{
	int count = 0;
	if ( h )
	{
		count = snprintf( dst, max, "%08Xh", x );
	}
	else
	{
		count = snprintf( dst, max, "%08X", x );
	}
	dst += count;
	max -= count;
	return;
}
// Print 64-bit hex value = x to output string = s with size limit = n
void AppLib::print64( LPSTR &dst, size_t &max, DWORD64 x, BOOL h )
{
	DWORD32 xLow = x;
	DWORD32 xHigh = ( x >> 16 ) >> 16;
	int count = 0;
	if ( h )
	{
		count = snprintf( dst, max, "%08X%08Xh", xHigh, xLow );
	}
	else
	{
		count = snprintf( dst, max, "%08X%08X", xHigh, xLow );
	}
	dst += count;
	max -= count;
	return;
}
// Print address hex value = x to output string = s with size limit = n
// Adaptive width, 32 or 64 bits
void AppLib::printPointer( LPSTR &dst, size_t &max, LPVOID x, BOOL h )
{
	DWORD64 y = ( DWORD64 )x;
#if NATIVE_WIDTH == 32
	AppLib::print32( dst, max, y, h );
#endif
#if NATIVE_WIDTH == 64
	AppLib::print64( dst, max, y, h );
#endif
	return;
}
// Print list of "1" bits
void AppLib::printBitsList( LPSTR &dst, size_t &max, DWORD64 x )
{
	int count =0;
	int i = 0;
	int j = 0;
	DWORD64 mask = 1;
	BOOL flag = FALSE;
	// Print cycle
	while ( i < 64 )
	{
		// Detect group of set bits
		while ( ( !( x & mask ) ) && ( i < 64 ) )
		{
			i++;
			mask = ( DWORD64 )1 << i;
		}
		j = i;
		while ( ( x & mask ) && ( j < 64 ) )
		{
			j++;
			mask = ( DWORD64 )1 << j;
		}
		j--;
		// Print group of set bits
		if ( i < 64 )
		{
			if ( flag )
			{
				count = snprintf( dst, max, ", " );
				dst += count;
				max -= count;
			}
			if ( i == j )
			{
				count = snprintf( dst, max, "%d", i );
			}
			else
			{
				count = snprintf( dst, max, "%d-%d", i, j );
			}
			dst += count;
			max -= count;
			j++;
			i = j;
			flag = TRUE;
		}
	}
	return;
}
// Print memory size
#define KILO 1024
#define MEGA 1024*1024
#define GIGA 1024*1024*1024
void AppLib::printCellMemorySize( LPSTR &dst, size_t &max, DWORD64 x, size_t min )
{
	// Print number
    double xd = x;
    int count = 0;
    if ( x < KILO )
    {
        int xi = x;
        count = snprintf( dst, max, "%d bytes", xi );
    }
    else if ( x < MEGA )
    {
        xd /= KILO;
        count = snprintf( dst, max, "%.2lf KB", xd );
    }
    else if ( x < GIGA )
    {
        xd /= MEGA;
        count = snprintf( dst, max, "%.2lf MB", xd );
    }
    else
    {
        xd /= GIGA;
        count = snprintf( dst, max, "%.2lf GB", xd );
    }
	dst += count;
	max -= count;
    // Print extra spaces for fill fixed size cell
    int extra = min - count;
	if ( extra > 0 )
	{
		for( int i=0; i<extra; i++ )
		{
			count = snprintf( dst, max, " " );
			dst += count;
			max -= count;
		}
	}
    return;
}

// Print memory base and size as hex
void AppLib::printBaseAndSize(  LPSTR &dst, size_t &max, DWORD64 base, DWORD64 size )
{
	int count = 0;
	count = snprintf( dst, max, "BASE=" );
	dst += count;
	max -= count;
	AppLib::print64( dst, max, base, TRUE );
	count = snprintf( dst, max, ", SIZE=" );
	dst += count;
	max -= count;
	AppLib::print64( dst, max, size, TRUE );
}

// Calculate statistics after benchmark:
// min, max, average, median for array results with elements count = length
void AppLib::calculateStatistics
		( int length, double results[], double* min, double* max, double* average, double* median )
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

// Print decoded OS error code
void AppLib::printSystemError( DWORD x )
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


