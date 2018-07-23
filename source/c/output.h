/*
 *   Regular data output support: write to console and/or report file.
 */

// Data for parameters visual
typedef enum { 
	INTEGER, MEMSIZE, SELECTOR, POINTER, HEX64, MHZ, STRNG 
} PRINT_TYPES;

typedef struct {
    char* name;             // pointer to parameter name for visual NAME=VALUE 
    char** values;          // pointer to array of strings pointers, text opt.
    void* data;             // pointer to visualized option variable
    PRINT_TYPES routine;    // select handling method for this entry
} PRINT_ENTRY;

void regularOutput ( PRINT_ENTRY print_list[] );

