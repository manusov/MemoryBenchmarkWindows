/*
 *   Header for console input-output service routines.
 */

// Data definitions
#define KILO 1024
#define MEGA 1024*1024
#define GIGA 1024*1024*1024
#define PRINT_LIMIT 20
// Routines definitions, common
int scratchMemorySize( char* scratchPointer, size_t memsize );
int printMemorySize( size_t memsize );
void printSelectedString( int select, char* names[] );
int scratchAffinity( char* scratchPointer, DWORD64 bitmap, int bufferLimit );
DWORD countSetBits(ULONG_PTR bitMask);
// Routines definitions, dump
void dumpBytes( PBYTE pointer, SIZE_T offset, SIZE_T size, WORD defaultColor );
void lineBytes( PBYTE pointer, SIZE_T offset, int blank, int print, WORD defaultColor );
void dumpWords( PWORD pointer, SIZE_T offset, SIZE_T size, WORD defaultColor );
void lineWords( PWORD pointer, SIZE_T offset, int blank, int print, WORD defaultColor );
void dumpDwords( PDWORD32 pointer, SIZE_T offset, SIZE_T size, WORD defaultColor );
void lineDwords( PDWORD32 pointer, SIZE_T offset, int blank, int print, WORD defaultColor );
void dumpQwords( PDWORD64 pointer, SIZE_T offset, SIZE_T size, WORD defaultColor );
void lineQwords( PDWORD64 pointer, SIZE_T offset, int blank, int print, WORD defaultColor );



