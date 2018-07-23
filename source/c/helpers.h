/*
 *   Common helpers subroutines and associated data structures declaration.
 *   Don't move tj this file routines, associated with current console context.
 */

#define KILO 1024
#define MEGA 1024*1024
#define GIGA 1024*1024*1024
#define PRINT_LIMIT 20

int scratchMemorySize( char* scratchPointer, size_t memsize );
int printMemorySize( size_t memsize );
void printSelectedString( int select, char* names[] );
