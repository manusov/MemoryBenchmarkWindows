/*
 *   Common helpers subroutines and associated data structures implementation.
 *   Don't move tj this file routines, associated with current console context.
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


