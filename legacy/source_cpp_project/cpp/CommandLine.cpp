#include "CommandLine.h"

// Command line parameters parse control
// Strings for command line options detect
// Methods for read-write memory by CPU instruction set
const char* CommandLine::keysAsm[] = { 
// default temporal methods
#if NATIVE_WIDTH == 32
    "readmov32", "writemov32", "copymov32", "modifynot32",
    "writestring32", "copystring32",
#endif
#if NATIVE_WIDTH == 64
    "readmov64", "writemov64", "copymov64", "modifynot64",
    "writestring64", "copystring64",
#endif
    "readsse128", "writesse128", "copysse128",
    "readavx256", "writeavx256", "copyavx256",
    "readavx512", "writeavx512", "copyavx512",
    "dotfma256", "dotfma512",
// methods with nontemporal write ((v)movntps, ((v)movntpd)
	"ntwwritesse128", "ntwcopysse128",
	"ntwwriteavx256", "ntwcopyavx256",
	"ntwwriteavx512", "ntwcopyavx512",
// methods with nontemporal read ((v)movntdqa) and write ((v)movntps, ((v)movntpd)
	"ntrwreadsse128", "ntrwcopysse128",
	"ntrwreadavx256", "ntrwcopyavx256",
	"ntrwreadavx512", "ntrwcopyavx512",
// methods with nontemporal prefetched read (prefetchnta) and write ((v)movntps, ((v)movntpd) 
	"ntprwreadsse128", "ntprwcopysse128",
	"ntprwreadsse128",
// Reserved for extensions ( 256(copy), 512(read, copy) )
// ... VERIFY MATCH WITH DLL 32/64 ...
// methods for latency check
	"latencylcm", "latencyrdrand",
// ... WALK ROUTINE NOT DIRECTLY ASSIGNED TO "ASM" KEY
// keys list termination
    NULL
};

// Target objects for benchmarks
const char* CommandLine::keysMemory[] = {
    "l1", "l2", "l3", "dram",
    NULL
};

// Page size control
const char* CommandLine::keysPage[] = {
    "default", "large",
    NULL
};

// Hyper-Threading (HT) mode control
const char* CommandLine::keysHt[] = {
	"off", "on"
};

// NUMA topology mode control
const char* CommandLine::keysNuma[] = {
    "unaware", "local", "remote",
    NULL
};

// Pointer command line parameters structure
COMMAND_LINE_PARMS CommandLine::parms;

// Option control list, used for command line parameters parsing (regular input)
const OPTION_ENTRY CommandLine::options[] = {
    { "asm"             , keysAsm         , &parms.optionAsm         , SELPARM } ,
    { "memory"          , keysMemory      , &parms.optionMemory      , SELPARM } ,
    { "threads"         , NULL            , &parms.optionThreads     , INTPARM } ,
    { "page"            , keysPage        , &parms.optionPageSize    , SELPARM } ,
    { "repeats"         , NULL            , &parms.optionRepeats     , INTPARM } ,
    { "adaptive"        , NULL            , &parms.optionAdaptive    , MEMPARM } ,
    { "ht"              , keysHt          , &parms.optionHt          , SELPARM } ,
    { "numa"            , keysNuma        , &parms.optionNuma        , SELPARM } ,
    { "start"           , NULL            , &parms.optionBlockStart  , MEMPARM } ,
    { "end"             , NULL            , &parms.optionBlockStop   , MEMPARM } ,
    { "step"            , NULL            , &parms.optionBlockDelta  , MEMPARM } ,
    { NULL              , NULL            , NULL                     , NOPARM  } 
};


// Status string
#define NS 81
char CommandLine::s[NS];

// Class constructor
CommandLine::CommandLine( )
{
	snprintf( s, NS, "no data" );
}

// Class destructor
CommandLine::~CommandLine( )
{
	
}

// Get pointer to extracted parameters
COMMAND_LINE_PARMS* CommandLine::getCommandLineParms( )
{
    return &parms;
}

// Reset parameters before override by command line
void CommandLine::resetBeforeParse( )
{
    parms.optionAsm = DEFAULT_ASM_METHOD;
    parms.optionMemory = DEFAULT_MEMORY_OBJECT;
    parms.optionThreads = DEFAULT_THREADS_COUNT;
    parms.optionRepeats = DEFAULT_MEASUREMENT_REPEATS;
    parms.optionAdaptive = DEFAULT_ADAPTIVE_REPEATS;
    parms.optionHt = DEFAULT_HT_MODE;
    parms.optionNuma = DEFAULT_NUMA_MODE;
    parms.optionBlockStart = DEFAULT_START_SIZE_BYTES;
    parms.optionBlockStop = DEFAULT_END_SIZE_BYTES;
    parms.optionBlockDelta = DEFAULT_DELTA_SIZE_BYTES;
}

// Correct parms. before override by command line, yet no operations
void CommandLine::correctAfterParse( )
{

}

// Definitions for parse command line
#define SMIN 3           // minimum option string length, example a=b
#define SMAX 81          // maximum option and status string length

// Parse command line, extract parameters, override defaults by user settings
DWORD CommandLine::parseCommandLine( int argc, char** argv )
{
    // Regular data input support: command line parameters extract and parsing.
    int i=0, j=0, k=0, k1=0, k2=0;  // miscellaneous counters and variables
    int recognized = 0;             // result of strings comparision, 0=match 
    OPTION_TYPES t = INTPARM;       // enumeration of parameters types for accept
    char* pAll = NULL;              // pointer to option full string NAME=VALUE
    char* pName = NULL;             // pointer to sub-string NAME
    char* pValue = NULL;            // pointer to sub-string VALUE
    char* pPattern = NULL;          // pointer to compared pattern string
    char** pPatterns = NULL;        // pointer to array of pointers to pat. strings
    DWORD* pDword = NULL;           // pointer to integer (32b) for variable store
    DWORDLONG* pLong = NULL;        // pointer to long (64b) for variable store
    DWORDLONG k64;                  // transit variable for memory block size
    char c = 0;                     // transit storage for char
    char cmdName[SMAX];             // extracted NAME of option string
    char cmdValue[SMAX];            // extracted VALUE of option string
    int MSG_MAX = 80+SMAX;
    // Verify command line format and extract values	
    for ( i=1; i<argc; i++ )  // cycle for command line options
    {
        // initializing for parsing current string
        // because element [0] is application name, starts from [1]
        pAll = argv[i];
        for ( j=0; j<SMAX; j++ )  // clear buffers
            {
            cmdName[j]=0;
            cmdValue[j]=0;
            }
        // check option sub-string length
        k = strlen( pAll );                   // k = length of one option sub-string
        if ( k<SMIN )
        {
            snprintf( s, NS, "option too short: %s", pAll );
            return -1;
        }
        if ( j>SMAX )
        {
            snprintf( s, NS, "option too long: %s", pAll );
            return -1;
        }
        // Parse command line parameters, setup input parameters, visual
        // extract option name and option value substrings
        pName = cmdName;
        pValue = cmdValue;
        strcpy( pName, pAll );           // store option sub-string to pName
        strtok( pName, "=" );            // pName = pointer to fragment before "="
        pValue = strtok( NULL, "?" );    // pValue = pointer to fragment after "="
        // check option name and option value substrings
        k1 = 0;
        k2 = 0;
        if ( pName  != NULL ) { k1 = strlen( pName );  }
        if ( pValue != NULL ) { k2 = strlen( pValue ); }
        if ( ( k1==0 )||( k2==0 ) )
        {
            snprintf( s, NS, "invalid option: %s", pAll );
            return -1;
        }
        // detect option by comparision from list, cycle for supported options
        j = 0;
        while ( options[j].name != NULL )
        {
            pPattern = ( char* )options[j].name;
            recognized = strcmp ( pName, pPattern );
            if ( recognized==0 )
            {
                // option-type specific handling, run if name match
                t = options[j].routine;
                switch(t)
                {
                    case INTPARM:  // support integer parameters
                    {
                        k1 = strlen( pValue );
                        for ( k=0; k<k1; k++ )
                        {
                            if ( isdigit( pValue[k] ) == 0 )
                            {
                                snprintf( s, NS, "not a number: %s", pValue );
                                return -1;
                            }
                        }
                        k = atoi( pValue );   // convert string to integer
                        pDword = ( DWORD* ) ( options[j].data );
                        *pDword = k;
                        break;
                        }
                    case MEMPARM:  // support memory block size parameters
                    {
                        k1 = 0;
                        k2 = strlen( pValue );
                        c = pValue[k2-1];
                        if ( isdigit(c) != 0 )
                        {
                            k1 = 1;             // no units kilo, mega, giga
                        }
                        else if ( c == 'K' )    // K means kilobytes
                        {
                            k2--;               // last char not a digit K/M/G
                            k1 = 1024;
                        }
                        else if ( c == 'M' )    // M means megabytes
                        {
                            k2--;
                            k1 = 1024*1024;
                        }
                        else if ( c == 'G' )    // G means gigabytes
                        {
                            k2--;
                            k1 = 1024*1024*1024;
                        }
                        for ( k=0; k<k2; k++ )
                        {
                            if ( isdigit( pValue[k] ) == 0 )
                            {
                                k1 = 0;
                            }
                        }
                        if ( k1==0 )
                        {
                            snprintf( s, NS, "not a block size: %s", pValue );
                            return -1;
                        }
                        k = atoi( pValue );   // convert string to integer
                        k64 = k;
                        k64 *= k1;
                        pLong = ( DWORDLONG* ) ( options[j].data );
                        *pLong = k64;
                        break;
                        }
                    case SELPARM:    // support parameters selected from text names
                    {
                        k = 0;
                        k2 = 0;
                        pPatterns = ( char** ) options[j].values;
                        while ( pPatterns[k] != NULL )
                        {
                            pPattern = pPatterns[k];
                            k2 = strcmp ( pValue, pPattern );
                            if ( k2==0 )
                            {
                                pDword = ( DWORD* )( options[j].data );
                                *pDword = k;
                                break;
                            }
                        k++;
                        }
                        if ( k2 != 0 )
                        {
                            snprintf( s, NS, "value invalid: %s", pAll );
                            return -1;
                        }
                        break;
                    }
                    case STRPARM:    // support parameter as text string
                    {
                        // pPatterns = path = pointer to pathBuffer
                        // pValue = pointer to source temp parsing buffer
                        pPatterns = ( CHAR** )( options[j].data );
                        strcpy ( *pPatterns, pValue );
                        break;
                    }
                }
            break;
            }
        j++;
        }
        // check option name recognized or not
        if ( recognized != 0 )
        {
            snprintf( s, NS, "option not recognized: %s", pName );
            return -1;
        }
    }
    return 0;
}

// Method returns status string, valid if error returned
char* CommandLine::getStatusString( )
{
    return s;
}


