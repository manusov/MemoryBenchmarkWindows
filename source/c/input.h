/*
 *   Regular data input support: command line parameters extract and parsing.
 */

// Data for command line parsing and extract parameters
typedef enum { 
	INTPARM, MEMPARM, SELPARM, STRPARM 
} OPTION_TYPES;

typedef struct {
    char* name;             // pointer to parm. name for recognition NAME=VALUE
    char** values;          // pointer to array of strings pointers, text opt.
    void* data;             // pointer to updated option variable
    OPTION_TYPES routine;   // select handling method for this entry
} OPTION_ENTRY;

void regularInput ( int argc, char** argv, OPTION_ENTRY option_list[] );
