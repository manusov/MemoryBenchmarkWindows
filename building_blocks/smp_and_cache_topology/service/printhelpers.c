/*
 *   Common strings write helpers subroutines and associated data structures declaration.
 *   Don't move to this file routines, associated with current console context.
 */

// Helper routine for print memory size: bytes/KB/MB/GB, overloaded
int scratchMemorySize( char* scratchPointer, size_t memsize )
{
    double xd = memsize;
    int nchars = 0;
    if ( memsize < KILO )
    {
        int xi = memsize;
        nchars = snprintf( scratchPointer, PRINT_LIMIT, "%d bytes", xi );
    }
    else if ( memsize < MEGA )
    {
        xd /= KILO;
        nchars = snprintf( scratchPointer, PRINT_LIMIT, "%.2lfK", xd );
    }
    else if ( memsize < GIGA )
    {
        xd /= MEGA;
        nchars = snprintf( scratchPointer, PRINT_LIMIT, "%.2lfM", xd );
    }
    else
    {
        xd /= GIGA;
        nchars = snprintf( scratchPointer, PRINT_LIMIT, "%.2lfG", xd );
    }
    return nchars;
}

// Helper routine for print memory size: bytes/KB/MB/GB, overloaded
int printMemorySize( size_t memsize )
{
    double xd = memsize;
    int nchars = 0;
    if ( memsize < KILO )
    {
        int xi = memsize;
        nchars = printf( "%d bytes", xi );
    }
    else if ( memsize < MEGA )
    {
        xd /= KILO;
        nchars = printf( "%.2lfK", xd );
    }
    else if ( memsize < GIGA )
    {
        xd /= MEGA;
        nchars = printf( "%.2lfM", xd );
    }
    else
    {
        xd /= GIGA;
        nchars = printf( "%.2lfG", xd );
    }
    return nchars;
}

// Helper routine for print selected string from strings array
void printSelectedString( int select, char* names[] )
{
    printf( "%s", names[select] );
}

// Print list of set bits, used for logical processors affinity masks
#define ONE_ENTRY 6
int scratchAffinity( char* scratchPointer, DWORD64 bitmap, int bufferLimit )
{
	int count = 0;
	int delta =0;
	DWORD64 mask = 1;
	int i = 0;
	int j = 0;
	
	while ( ( count < ( bufferLimit - ONE_ENTRY ) ) && ( i < 64 ) )
	{
		// Detect group of set bits
		while ( ( !( bitmap & mask ) ) && ( i < 64 ) )
		{
			i++;
			mask = (DWORD64)1<<i;
		}
		j = i;
		while ( ( bitmap & mask ) && ( j < 64 ) )
		{
			j++;
			mask = (DWORD64)1<<j;
		}
		j--;
		
		// Print geoup of set bits
		if ( i < 64 )
		{
			if ( count > 0 )
			{
				delta = snprintf( scratchPointer, ONE_ENTRY, ", " );
				count += delta;
				scratchPointer += delta;
			}
			
			if ( ( i == j ) && ( count < ( bufferLimit - ONE_ENTRY ) ) )
			{
				delta = snprintf( scratchPointer, ONE_ENTRY, "%d", i );
			}
			else if ( count < ( bufferLimit - ONE_ENTRY ) )
			{
				delta = snprintf( scratchPointer, ONE_ENTRY, "%d-%d", i, j );
			}
			else
			{
				delta = snprintf( scratchPointer, ONE_ENTRY, "..." );
			}
			
			count += delta;
			scratchPointer += delta;
			j++;
			i = j;
		}
	}
	
	return count;
}




