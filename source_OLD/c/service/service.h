/*
 *   Service helpers data and routines definitions
 */

// Constants definition
#define PRINT_NAME  20   // number of chars before "=" for tabulation
#define KILO 1024
#define MEGA 1024*1024
#define GIGA 1024*1024*1024
#define PRINT_LIMIT 20

// Definitions for option control list (regular input)
typedef enum { 
	INTPARM, MEMPARM, SELPARM, STRPARM 
} OPTION_TYPES;

typedef struct {
    char* name;             // pointer to parm. name for recognition NAME=VALUE
    char** values;          // pointer to array of strings pointers, text opt.
    void* data;             // pointer to updated option variable
    OPTION_TYPES routine;   // select handling method for this entry
} OPTION_ENTRY;

// Definitions for parameters visual list (regular output)
typedef enum { 
	INTEGER, MEMSIZE, SELECTOR, POINTER, HEX64, MHZ, STRNG 
} PRINT_TYPES;

typedef struct {
    char* name;             // pointer to parameter name for visual NAME=VALUE 
    char** values;          // pointer to array of strings pointers, text opt.
    void* data;             // pointer to visualized option variable
    PRINT_TYPES routine;    // select handling method for this entry
} PRINT_ENTRY;

// Regular data output support: write to console and/or report file.
void regularOutput ( PRINT_ENTRY print_list[] );
// Regular data input support: command line parameters extract and parsing.
void regularInput ( int argc, char** argv, OPTION_ENTRY option_list[] );
// Statistics analysing of benchmarks results
void calculateStatistics( int length , double results[] , 
                          double* min , double* max , double* average , double* median );
                          
// Print helpers
int scratchMemorySize( char* scratchPointer, size_t memsize );
int printMemorySize( size_t memsize );
void printSelectedString( int select, char* names[] );


