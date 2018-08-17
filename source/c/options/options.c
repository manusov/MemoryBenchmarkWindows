/*
 *   Data structures for Memory Performance Engine command line options support.
 *   Base information, see also associated definitions at optionsdetails.c.
 */

// Text strings for option recognition and visual
// Methods for read-write memory by CPU instruction set
char* rwMethods[] = { 
    "readmov64", "writemov64", "copymov64", "modifynot64",
    "writestring64", "copystring64",
    "readsse128", "writesse128", "copysse128",
    "readavx256", "writeavx256", "copyavx256",
    "readavx512", "writeavx512", "copyavx512",
    "dotfma256", "dotfma512",
    NULL
};
// Target objects for benchmarks
char* rwTargets[] = {
    "l1cache", "l2cache", "l3cache", "l4cache", "dram", "userdefined",
    NULL
};
// Access methods by temporal/nontemporal classification
char* rwAccess[] = {
    "temporal", "nontemporal", "default",
    NULL
};
// Hyper-threading control
char* hyperThreading[] = {
    "off", "on"
};
// Page size control
char* pageSize[] = {
    "default", "large",
    NULL
};
// NUMA control
char* numaMode[] = {
    "nonaware", "forceoptimal", "forcenonoptimal",
    NULL
};
// Precision control
char* precision[] = {
    "slow", "fast",
    NULL
};
// Machine readable option control
char* machineReadable[] = {
    "off", "on",
    NULL
};

// Options control routines

void detectMethods( DWORD *select, DWORDLONG *bitmap, DWORDLONG bitmapCpu, DWORDLONG bitmapOs )
{
    *bitmap = bitmapCpu & bitmapOs;
	if ( *select == DEFAULT_RW_METHOD )
    {
        DWORD mask = 0;
        // Try AVX512 read memory method
        *select = CPU_FEATURE_READ_AVX512;
        mask = ((DWORDLONG)1) << *select;
        if ( ! (mask & *bitmap) )
        {
        *select = CPU_FEATURE_READ_AVX256;
        }
        // Try AVX256 read memory method
        mask = ((DWORDLONG)1) << *select;
        if ( ! (mask & *bitmap) )
        {
        *select = CPU_FEATURE_READ_SSE128;
        }
        // Try SSE128 read memory method
        mask = ((DWORDLONG)1) << *select;
        if ( ! (mask & *bitmap) )
        {
        *select = CPU_FEATURE_READ_X64;
        }
        // Try common x86-64 read memory method
        mask = ((DWORDLONG)1) << *select;
        if ( ! (mask & *bitmap) )
        {
            *select = -1;
        }
    }
}

void printMethods( DWORD select, DWORDLONG bitmap, DWORDLONG bitmapCpu, DWORDLONG bitmapOs, CHAR *methodsNames[] )
{
    int N = 78;
    printf( "\n" );
    lineOfTable( N );
    printf( "\n  # CPU OS Operation and CPU instruction used" );
    printf( "\n" );
    lineOfTable( N );
    int i = 0;
    int a, b;
    while ( methodsNames[i] != NULL )
    {
        a = bitmapCpu & 1;
        b = bitmapOs & 1;
        printf("\n %2d  %-2d %-2d %s", i, a, b, methodsNames[i] );
        bitmapCpu = bitmapCpu >> 1;
        bitmapOs = bitmapOs >> 1;
        i++;
    }
    printf("\n");
    lineOfTable( N );
    printf("\nDefault selection = %d , bitmap = %08X%08Xh", select, ( bitmap >> 16) >> 16 , bitmap );
}

