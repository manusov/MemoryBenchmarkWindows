/*
           MEMORY PERFORMANCE ENGINE FRAMEWORK.
    Command line parser and parameters extractor class.
*/

#include "CommandLine.h"

// Command line parameters parse control
// Strings for command line options detect

// Keys for scenario select: user help
const char* CommandLine::keysHelp[] = {
    "brief", "full",
    NULL
};

// Keys for scenario select: system information
// yet reserved: "cache"
const char* CommandLine::keysInfo[] = {
    "cpu", "topology", "topologyex", "memory", "paging",
    "api", "domains", "threads",
    "all",
    NULL
};

// Keys for scenario select: benchmark mode
const char* CommandLine::keysTest[] = {
    "memory", "storage",
    NULL
};

// Keys for output mode select
const char* CommandLine::keysOut[] = {
    "screen", "file",
    NULL
};

// Methods for read-write memory by CPU instruction set
const char* CommandLine::keysAsm[] = 
{
    // default temporal methods
    #if NATIVE_WIDTH == 64
    "readmov64"     , "writemov64"   , "copymov64" , "modifynot64" ,
    "writestring64" , "copystring64" ,
    #endif
    #if NATIVE_WIDTH == 32
    "readmov32"     , "writemov32"   , "copymov32" , "modifynot32" ,
    "writestring32" , "copystring32" ,
    #endif
    "readmmx64"     , "writemmx64"   , "copymmx64"  ,
    "readsse128"    , "writesse128"  , "copysse128" ,
    "readavx256"    , "writeavx256"  , "copyavx256" ,
    "readavx512"    , "writeavx512"  , "copyavx512" ,
    "dotfma256"     , "dotfma512"    ,
    "gather256"     , "gather512"    , "scatter512" ,
    // default non-temporal
    "clzero" ,
    // latency
    "latencylcm"    , "latencylcm32x2"    ,
    "latencyrdrand" , "latencyrdrand32x2" ,
    // placeholders for latency walkers 32/64 and 32x2
    "latencywalk"   , "latencywalk32x2"   ,
    // non-temporal SSE128
    "ntreadsse128"         , 
    "ntpreadsse128default" , "ntpreadsse128medium" , "ntpreadsse128long" ,
    "ntwritesse128"        , "ntcopysse128"        , "ntrcopysse128" ,
    // non-temporal AVX256
    "ntreadavx256"         ,
    "ntpreadavx256default" , "ntpreadavx256medium" , "ntpreadavx256long" ,
    "ntwriteavx256"        , "ntcopyavx256"        , "ntrcopyavx256" ,
    // non-temporal AVX512
    "ntreadavx512"         ,
    "ntpreadavx512default" , "ntpreadavx512medium" , "ntpreadavx512long" ,
    "ntwriteavx512"        , "ntcopyavx512"        , "ntrcopyavx512" ,
    // keys list termination
    NULL
};

// Target objects for benchmarks
const char* CommandLine::keysMemory[] = {
    "l1", "l2", "l3", "l4", "dram",
    NULL
};

// Page size control
const char* CommandLine::keysPage[] = {
    "default", "large",
    NULL
};

// Hyper-Threading (HT) mode control
const char* CommandLine::keysHt[] = {
    "off", "on",
    NULL
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
    // keys for scenario select
    { "help"            , keysHelp        , &parms.optionHelp        , SELPARM } ,
    { "info"            , keysInfo        , &parms.optionInfo        , SELPARM } ,
    { "test"            , keysTest        , &parms.optionTest        , SELPARM } ,
    // key for output mode select, screen or file
    { "out"             , keysOut         , &parms.optionOut         , SELPARM } ,
    // keys for benchmarking
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
CommandLine::CommandLine()
{
    snprintf(s, NS, "no data");
    // pre-blank fields
    // Scenario selectors
    parms.optionHelp = 0;
    parms.optionInfo = 0;
    parms.optionTest = 0;
    // Benchmark base option
    parms.optionAsm = 0;
    parms.optionMemory = 0;
    parms.optionThreads = 0;
    // Large pages control
    parms.optionPageSize = 0;
    // Constant repeats control
    parms.optionRepeats = 0;
    // Advanced adaptive repeats control
    parms.optionAdaptive = 0;
    // Topology options
    parms.optionHt = 0;
    parms.optionNuma = 0;
    // Block sizes explicit set
    parms.optionBlockStart = 0;
    parms.optionBlockStop = 0;
    parms.optionBlockDelta = 0;
}

// Class destructor
CommandLine::~CommandLine()
{

}

// Get pointer to extracted parameters
COMMAND_LINE_PARMS* CommandLine::getCommandLineParms()
{
    return &parms;
}

// This function required for user help
const OPTION_ENTRY* CommandLine::getOptionsList()
{
    return options;
}

// Reset parameters before override by command line
void CommandLine::resetBeforeParse()
{
    // Scenario select options
    parms.optionHelp = DEFAULT_HELP_SCENARIO;
    parms.optionInfo = DEFAULT_INFO_SCENARIO;
    parms.optionTest = DEFAULT_TEST_SCENARIO;
    // Output options, screen or file
    parms.optionOut = OUT_SCREEN;
    // Benchmark options
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
void CommandLine::correctAfterParse()
{

}

// Definitions for parse command line
#define SMIN 3           // minimum option string length, example a=b
#define SMAX 81          // maximum option and status string length

// Parse command line, extract parameters, override defaults by user settings
DWORD CommandLine::parseCommandLine(int argc, char** argv)
{
    // Regular data input support: command line parameters extract and parsing.
    int i = 0, j = 0, k = 0, k1 = 0, k2 = 0;  // miscellaneous counters and variables
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
    int MSG_MAX = 80 + SMAX;
    // Verify command line format and extract values	
    for (i = 1; i < argc; i++)  // cycle for command line options
    {
        // initializing for parsing current string
        // because element [0] is application name, starts from [1]
        pAll = argv[i];
        for (j = 0; j < SMAX; j++)  // clear buffers
        {
            cmdName[j] = 0;
            cmdValue[j] = 0;
        }
        // check option sub-string length
        k = (int)strlen(pAll);                   // k = length of one option sub-string
        if (k < SMIN)
        {
            snprintf(s, NS, "option too short: %s", pAll);
            return -1;
        }
        if (j > SMAX)
        {
            snprintf(s, NS, "option too long: %s", pAll);
            return -1;
        }
        // Parse command line parameters, setup input parameters, visual
        // extract option name and option value substrings
        pName = cmdName;
        pValue = cmdValue;
        strcpy(pName, pAll);                 // store option sub-string to pName
        char* dummy = strtok(pName, "=");    // pName = pointer to fragment before "="
        pValue = strtok(NULL, "?");          // pValue = pointer to fragment after "="
        // check option name and option value substrings
        k1 = 0;
        k2 = 0;
        if (pName != NULL) { k1 = (int)strlen(pName); }
        if (pValue != NULL) { k2 = (int)strlen(pValue); }
        if ((k1 == 0) || (k2 == 0))
        {
            snprintf(s, NS, "invalid option: %s", pAll);
            return -1;
        }
        // detect option by comparision from list, cycle for supported options
        j = 0;
        while (options[j].name != NULL)
        {
            pPattern = (char*)options[j].name;
            recognized = strcmp(pName, pPattern);
            if (recognized == 0)
            {
                // option-type specific handling, run if name match
                t = options[j].routine;
                switch (t)
                {
                case INTPARM:  // support integer parameters
                {
                    k1 = (int)strlen(pValue);
                    for (k = 0; k < k1; k++)
                    {
                        if (isdigit(pValue[k]) == 0)
                        {
                            snprintf(s, NS, "not a number: %s", pValue);
                            return -1;
                        }
                    }
                    k = atoi(pValue);   // convert string to integer
                    pDword = (DWORD*)(options[j].data);
                    *pDword = k;
                    break;
                }
                case MEMPARM:  // support memory block size parameters
                {
                    k1 = 0;
                    k2 = (int)strlen(pValue);
                    c = pValue[k2 - 1];
                    if (isdigit(c) != 0)
                    {
                        k1 = 1;             // no units kilo, mega, giga
                    }
                    else if (c == 'K')    // K means kilobytes
                    {
                        k2--;               // last char not a digit K/M/G
                        k1 = 1024;
                    }
                    else if (c == 'M')    // M means megabytes
                    {
                        k2--;
                        k1 = 1024 * 1024;
                    }
                    else if (c == 'G')    // G means gigabytes
                    {
                        k2--;
                        k1 = 1024 * 1024 * 1024;
                    }
                    for (k = 0; k < k2; k++)
                    {
                        if (isdigit(pValue[k]) == 0)
                        {
                            k1 = 0;
                        }
                    }
                    if (k1 == 0)
                    {
                        snprintf(s, NS, "not a block size: %s", pValue);
                        return -1;
                    }
                    k = atoi(pValue);   // convert string to integer
                    k64 = k;
                    k64 *= k1;
                    pLong = (DWORDLONG*)(options[j].data);
                    *pLong = k64;
                    break;
                }
                case SELPARM:    // support parameters selected from text names
                {
                    k = 0;
                    k2 = 0;
                    pPatterns = (char**)options[j].values;
                    while (pPatterns[k] != NULL)
                    {
                        pPattern = pPatterns[k];
                        k2 = strcmp(pValue, pPattern);
                        if (k2 == 0)
                        {
                            pDword = (DWORD*)(options[j].data);
                            *pDword = k;
                            break;
                        }
                        k++;
                    }
                    if (k2 != 0)
                    {
                        snprintf(s, NS, "value invalid: %s", pAll);
                        return -1;
                    }
                    break;
                }
                case STRPARM:    // support parameter as text string
                {
                    // pPatterns = path = pointer to pathBuffer
                    // pValue = pointer to source temp parsing buffer
                    pPatterns = (CHAR**)(options[j].data);
                    strcpy(*pPatterns, pValue);
                    break;
                }
                }
                break;
            }
            j++;
        }
        // check option name recognized or not
        if (recognized != 0)
        {
            snprintf(s, NS, "option not recognized: %s", pName);
            return -1;
        }
    }
    return 0;
}

// Method returns status string, valid if error returned
char* CommandLine::getStatusString()
{
    return s;
}
