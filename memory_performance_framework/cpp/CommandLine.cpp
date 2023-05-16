/*

MEMORY PERFORMANCE ENGINE (MPE) FRAMEWORK.
-------------------------------------------
System informationand performance analysing application,
include console version of NCRB(NUMA CPUand RAM Benchmark).
See also memory benchmark projects :
https://github.com/manusov/NCRBv2
https://github.com/manusov/MemoryBenchmarkWindows
Experimental samples :
https://github.com/manusov/Prototyping
Special thanks for C++ lessons :
https://ravesli.com/uroki-cpp/

Class realization for application command line support:
get command line and builds parameters list.
TODO.

*/

#include "CommandLine.h"

CommandLine::CommandLine()
{
    snprintf(statusString, APPCONST::MAX_TEXT_STRING, "No data.");
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
CommandLine::~CommandLine()
{

}
void CommandLine::writeReport()
{
    AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
    AppLib::writeColor("Benchmark scenario options.\r\n", APPCONST::TABLE_COLOR);
    AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);

    char msg[APPCONST::MAX_TEXT_STRING];
    const OPTION_ENTRY* p = options;
    while (p->name)
    {
        switch (p->routine)
        {
            case INTPARM:
                snprintf(msg, APPCONST::MAX_TEXT_STRING, "%d", *((int*)(p->data)));
                break;
            case SELPARM:
            {
                int index = *((int*)(p->data));
                if (index < 0)
                {
                    snprintf(msg, APPCONST::MAX_TEXT_STRING, "default");
                }
                else
                {
                    const char** keyNames = p->values;
                    const char* name = keyNames[index];
                    snprintf(msg, APPCONST::MAX_TEXT_STRING, "%s", name);;
                }
                break;
            }
            case MEMPARM:
            {
                DWORD64 a = *((DWORD64*)(p->data));
                if (a == 0xFFFFFFFFFFFFFFFFL)
                {
                    snprintf(msg, APPCONST::MAX_TEXT_STRING, "default");
                }
                else
                {
                    AppLib::storeCellMemorySize(msg, APPCONST::MAX_TEXT_STRING, a, 20);
                }
                break;
            }
            case STRPARM:
                snprintf(msg, APPCONST::MAX_TEXT_STRING, (char*)(p->data));
                break;
            case NOPARM:
            default:
                snprintf(msg, APPCONST::MAX_TEXT_STRING, "?");
                break;
        }
        AppLib::writeColor(" ", APPCONST::TABLE_COLOR);
        AppLib::writeParmAndValue(p->name, msg, 11);
        p++;
    }
    AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
}
// Get pointer to extracted parameters.
COMMAND_LINE_PARMS* CommandLine::getCommandLineParms()
{
    return &parms;
}
// This function required for user help.
const OPTION_ENTRY* CommandLine::getOptionsList()
{
    return options;
}
// Reset parameters before override by command line.
void CommandLine::resetBeforeParse()
{
    // Scenario select options
    parms.optionHelp = APPCONST::DEFAULT_HELP_SCENARIO;
    parms.optionInfo = APPCONST::DEFAULT_INFO_SCENARIO;
    parms.optionTest = APPCONST::DEFAULT_TEST_SCENARIO;
    // Output options, screen or file
    parms.optionOut = OUT_SCREEN;
    // Benchmark options
    parms.optionAsm        = APPCONST::DEFAULT_ASM_METHOD;
    parms.optionMemory     = APPCONST::DEFAULT_MEMORY_OBJECT;
    parms.optionThreads    = APPCONST::DEFAULT_THREADS_COUNT;
    parms.optionRepeats    = APPCONST::DEFAULT_MEASUREMENT_REPEATS;
    parms.optionAdaptive   = APPCONST::DEFAULT_ADAPTIVE_REPEATS;
    parms.optionHt         = APPCONST::DEFAULT_HT_MODE;
    parms.optionNuma       = APPCONST::DEFAULT_NUMA_MODE;
    parms.optionBlockStart = APPCONST::DEFAULT_START_SIZE_BYTES;
    parms.optionBlockStop  = APPCONST::DEFAULT_END_SIZE_BYTES;
    parms.optionBlockDelta = APPCONST::DEFAULT_DELTA_SIZE_BYTES;
}
// Correct parms. before override by command line, yet no operations.
void CommandLine::correctAfterParse()
{
    // Reserved
}
// Definitions for parse command line
constexpr int SMIN = 3;      // Minimum option string length, example a=b.
constexpr int SMAX = 81;     // Maximum option and status string length.
// Parse command line, extract parameters, override defaults by user settings.
DWORD CommandLine::parseCommandLine(int argc, char** argv)
{
    // Regular data input support: command line parameters extract and parsing.
    int i = 0, j = 0, k = 0, k1 = 0, k2 = 0;  // miscellaneous counters and variables
    int recognized = 0;             // result of strings comparision, 0=match 
    OPTION_TYPES t = INTPARM;       // enumeration of parameters types for accept
    char* pAll = nullptr;           // pointer to option full string NAME=VALUE
    char* pName = nullptr;          // pointer to sub-string NAME
    char* pValue = nullptr;         // pointer to sub-string VALUE
    char* pPattern = nullptr;       // pointer to compared pattern string
    char** pPatterns = nullptr;     // pointer to array of pointers to pat. strings
    DWORD* pDword = nullptr;        // pointer to integer (32b) for variable store
    DWORDLONG* pLong = nullptr;     // pointer to long (64b) for variable store
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
            snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Option too short: %s.", pAll);
            return -1;
        }
        if (j > SMAX)
        {
            snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Option too long: %s.", pAll);
            return -1;
        }
        // Parse command line parameters, setup input parameters, visual
        // extract option name and option value substrings
        pName = cmdName;
        pValue = cmdValue;
        strcpy(pName, pAll);                 // store option sub-string to pName
        char* dummy = strtok(pName, "=");    // pName = pointer to fragment before "="
        pValue = strtok(nullptr, "?");          // pValue = pointer to fragment after "="
        // check option name and option value substrings
        k1 = 0;
        k2 = 0;
        if (pName) { k1 = (int)strlen(pName); }
        if (pValue) { k2 = (int)strlen(pValue); }
        if ((k1 == 0) || (k2 == 0))
        {
            snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Invalid option: %s.", pAll);
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
                            snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Not a number: %s.", pValue);
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
                        snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Not a block size: %s.", pValue);
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
                        snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Value invalid: %s.", pAll);
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
            snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Option not recognized: %s.", pName);
            return -1;
        }
    }
    return 0;
}
// Command line parameters parse control.
// Strings for command line options detect.
// Keys for scenario select: user help.
const char* CommandLine::keysHelp[] = {
    "brief", "full",
    nullptr
};
// Keys for scenario select: system information.
// yet reserved: "cache"
const char* CommandLine::keysInfo[] = {
    "cpu", "topology", "topologyex", "memory", "paging",
    "api", "domains", "threads",
    "all",
    nullptr
};
// Keys for scenario select: benchmark mode.
const char* CommandLine::keysTest[] = {
    "memory", "storage",
    nullptr
};
// Keys for output mode select
const char* CommandLine::keysOut[] = {
    "screen", "file",
    nullptr
};
// Methods for read-write memory by CPU instruction set.
const char* CommandLine::keysAsm[] =
{
    // default temporal methods
    #ifdef NATIVE_WIDTH_64
    "readmov64"     , "writemov64"   , "copymov64" , "modifynot64" ,
    "writestring64" , "copystring64" ,
    #endif
    #ifdef NATIVE_WIDTH_32
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
    nullptr
};
// Target objects for benchmarks.
const char* CommandLine::keysMemory[] = {
    "l1", "l2", "l3", "l4", "dram",
    nullptr
};
// Page size control
const char* CommandLine::keysPage[] = {
    "default", "large",
    nullptr
};
// Hyper-Threading (HT) mode control.
const char* CommandLine::keysHt[] = {
    "off", "on",
    nullptr
};
// NUMA topology mode control
const char* CommandLine::keysNuma[] = {
    "unaware", "local", "remote",
    nullptr
};
// Pointer command line parameters structure.
COMMAND_LINE_PARMS CommandLine::parms;
// Option control list, used for command line parameters parsing (regular input).
const OPTION_ENTRY CommandLine::options[] = {
    // Keys for scenario select.
    { "help"            , keysHelp        , &parms.optionHelp        , SELPARM } ,
    { "info"            , keysInfo        , &parms.optionInfo        , SELPARM } ,
    { "test"            , keysTest        , &parms.optionTest        , SELPARM } ,
    // Key for output mode select, screen or file.
    { "out"             , keysOut         , &parms.optionOut         , SELPARM } ,
    // Keys for benchmarking.
    { "asm"             , keysAsm         , &parms.optionAsm         , SELPARM } ,
    { "memory"          , keysMemory      , &parms.optionMemory      , SELPARM } ,
    { "threads"         , nullptr         , &parms.optionThreads     , INTPARM } ,
    { "page"            , keysPage        , &parms.optionPageSize    , SELPARM } ,
    { "repeats"         , nullptr         , &parms.optionRepeats     , INTPARM } ,
    { "adaptive"        , nullptr         , &parms.optionAdaptive    , MEMPARM } ,
    { "ht"              , keysHt          , &parms.optionHt          , SELPARM } ,
    { "numa"            , keysNuma        , &parms.optionNuma        , SELPARM } ,
    { "start"           , nullptr         , &parms.optionBlockStart  , MEMPARM } ,
    { "end"             , nullptr         , &parms.optionBlockStop   , MEMPARM } ,
    { "step"            , nullptr         , &parms.optionBlockDelta  , MEMPARM } ,
    { nullptr           , nullptr         , nullptr                  , NOPARM  }
};
