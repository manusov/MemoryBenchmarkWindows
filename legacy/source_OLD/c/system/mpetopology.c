/*
 *   Memory Performance Engine.
 *   Handlers for system information and benchmark patterns.
 *   Object = Multiprocessing topology and cache memory.
 */

DWORD initTopology( CHAR* returnText )
{
    
	return STATUS_EMPTY;
}

DWORD deinitTopology( CHAR* returnText )
{
    return STATUS_EMPTY;
}

DWORD detectTopology ( CHAR* returnText, MPE_TOPOLOGY_DATA* returnBinary )
{
    // Platform detect control variables
    DWORD status = -1;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION bufPtr = NULL;
    DWORD bufSize = 0;
    int bufCount = 0;
    status = getLPI( &bufPtr, &bufSize );
    if ( status == 1 )
    {
        exitWithSystemError( "detect platform MP topology" );
    }
    if ( ( status >= FUNCTION_INTERNAL_ERROR )||( bufPtr == NULL )||( bufSize == 0 ) )
    {
        if(bufPtr)
        {
            free(bufPtr);
        }
        exitWithInternalError( "wrong MP topology info");
    }
    bufCount = bufSize / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION );
    // Local constants and variables
    CHAR* stringCore = "CPU core";
    CHAR* stringNode = "NUMA node";
    CHAR* stringPackage = "CPU package";
    CHAR* stringUnified = "Unified";
    CHAR* stringInstruction = "Instruction";
    CHAR* stringData = "Data";
    CHAR* stringTrace = "Trace";
    CHAR* stringUnknown = "Unknown";
    CHAR* sSrc;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION bufPtr1;
    PCACHE_DESCRIPTOR Cache;
    DWORD relationship;
    DWORD64 affinity;
    CHAR s1[40];
    CHAR s2[40];
    CHAR s3[40];
    CHAR s4[40];
    CHAR stmp[40];
    int i;
    BOOLEAN flag;
    DWORD x1;
    DWORD x2;
    DWORD x3;
    DWORD x4;
    DWORD x5;
    WORD color1 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD color2 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    // Local statistics for MP topology
    int packageCount = 0;
    int numaCount = 0;
    int coreCount = 0;
    int logicalCount = 0;
    DWORD64 logicalMask = 0;
    // Local statistics for Caches
    int L1count = 0;
    int L2count = 0;
    int L3count = 0;
    int L1point = 0;
    int L2point = 0;
    int L3point = 0;
    // Sizes string
    printf( "\nBuffer base=%ph, size=%d bytes (%d descriptors)\n", bufPtr, bufSize, bufCount );
    // Build and print MP topology list
    bufPtr1 = bufPtr;
    lineOfTable( 78 );
    printf( "\n Topology unit        Logical CPU affinity       Comments\n" );
    lineOfTable( 78 );
    for( i=0; i<bufCount; i++ )
    {
        relationship = bufPtr1->Relationship;
        affinity = bufPtr1->ProcessorMask;
        flag = TRUE;
        // Build descriptor name string
        switch (relationship)
        {
            case RelationProcessorCore:
                x1 = bufPtr1->ProcessorCore.Flags;
                snprintf( s1, 39, "\n %-21s", stringCore );
                snprintf( s3, 39, "HT flag=%d", x1 );
                coreCount++;
                logicalMask |= affinity;
                break;
            case RelationNumaNode:
                x1 = bufPtr1->NumaNode.NodeNumber;
                snprintf( s1, 39, "\n %-21s", stringNode );
                snprintf( s3, 39, "Node ID=%d", x1 );
                numaCount++;
                break;
            case RelationProcessorPackage:
                snprintf( s1, 39, "\n %-21s", stringPackage );
                snprintf( s3, 39, "-" );
                packageCount++;
                break;
            default:
                flag = FALSE;
                break;
        }
        if( flag )
        {
            // Build affinity string
            scratchAffinity( stmp, affinity, 39 );
            snprintf( s2, 39, "%-27s", stmp );
            // Print all strings
            printf( "%s%s%s", s1, s2, s3 );            
        }
        bufPtr1++;
    }
    printf("\n");
    lineOfTable( 78 );
    // Build and print caches list
    bufPtr1 = bufPtr;
    printf( "\n Cache           Logical CPU affinity   Associativity   Line   Size\n" );
    lineOfTable( 78 );
    for( i=0; i<bufCount; i++ )
    {
        relationship = bufPtr1->Relationship;
        affinity = bufPtr1->ProcessorMask;
        switch( relationship )
        {
            case RelationCache:
            Cache = &bufPtr1->Cache;
            x1 = Cache->Level;
            x2 = Cache->Associativity;
            x3 = Cache->LineSize;
            x4 = Cache->Size;
            x5 = Cache->Type;
            // Build cache level string
            snprintf( s1, 39, "\n L%-2d", x1 );
            // Build cache type string
            switch(x5)
            {
                case CacheUnified:
                    sSrc = stringUnified;
                    break;
                case CacheInstruction:
                    sSrc = stringInstruction;
                    break;
                case CacheData:
                    sSrc = stringData;
                    break;
                case CacheTrace:
                    sSrc = stringTrace;
                    break;
                default:
                    sSrc = stringUnknown;
                    break;
            }
            // Modify statistics counters
            switch (x1)
            {
                case 1:
                    L1count++;
                    if ( ( x5 == CacheUnified )||( x5 == CacheData ) ) L1point = x4 / 1024;
                    break;
                case 2:
                    L2count++;
                    if ( ( x5 == CacheUnified )||( x5 == CacheData ) ) L2point = x4 / 1024;
                    break;
                case 3:
                    L3count++;
                    if ( ( x5 == CacheUnified )||( x5 == CacheData ) ) L3point = x4 / 1024;
                    break;
            }
            snprintf( s2, 39, "%-13s", sSrc );
            // Build affinity string
            scratchAffinity( stmp, affinity, 39 );
            snprintf( s3, 39, "%-23s", stmp );
            // Build combined string: associativity, line, size
            snprintf( s4, 39, "%-16d%-7d%d", x2, x3, x4 );
            // Print all strings
            printf( "%s%s%s%s", s1, s2, s3, s4 );
            break;
        }
        bufPtr1++;
    }
    printf("\n");
    lineOfTable( 78 );
    // Build and print summary parameters list
    printf( "\n Parameter              Value\n" );
    lineOfTable( 78 );
    // Topology statistics
    printf("\n %-23s%d"   , "CPU packages" , packageCount );
    printf("\n %-23s%d"   , "NUMA nodes"   , numaCount    );
    printf("\n %-23s%d"   , "CPU cores"    , coreCount    );
    logicalCount = countSetBits( logicalMask );
    printf("\n %-23s%d"   , "Logical CPUs" , logicalCount );
    // Caches statistics
    printf("\n %-23s%d, test point = %d KB"   , "L1 caches" , L1count , L1point );
    printf("\n %-23s%d, test point = %d KB"   , "L2 caches" , L2count , L2point );
    printf("\n %-23s%d, test point = %d KB\n" , "L3 caches" , L3count , L3point );
    lineOfTable( 78 );
    // Release memory, allocated for MP info buffer
    if(bufPtr)
	{
        free(bufPtr);
	}
	// Exit with update output parameters
	returnBinary->cpuLogical = logicalCount;
    returnBinary->cpuCores = coreCount;
    returnBinary->cpuPackages = packageCount;
    returnBinary->cpuNumaDomains = numaCount;
    returnBinary->pointL1 = L1point;
    returnBinary->pointL2 = L2point;
    returnBinary->pointL3 = L3point;
	// returnBinary->cpuHt = UNDER CONSTRUCTION
    return STATUS_OK;
}

// Helpers

// Get Logical Processor Information, use OS API function
DWORD getLPI( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION *ptrPtr, DWORD *ptrSize )
{
	// Local variables
    HANDLE hglpi = NULL;
    LPFN_GLPI glpi = NULL;
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION tempPtr = NULL;
    DWORD tempSize = 0;
    // Load WinAPI function
    hglpi = GetModuleHandle( "kernel32" );
    if ( hglpi == NULL ) return FUNCTION_API_ERROR;
    glpi = (LPFN_GLPI) GetProcAddress( hglpi, "GetLogicalProcessorInformation" );
    if ( glpi == NULL ) return FUNCTION_API_ERROR;
    // Get information, note required memory allocation for buffer
    while (!done)
    {
        DWORD rc = glpi( tempPtr, &tempSize);
        if ( rc == FALSE ) 
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
            {
                if ( tempPtr ) 
                    free( tempPtr );

                tempPtr = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc( tempSize );

                if ( tempPtr == NULL ) 
                {
                	return FUNCTION_API_ERROR;
                }
            } 
            else 
            {
            	return FUNCTION_API_ERROR;
            }
        } 
        else
        {
            done = TRUE;
        }
    }
    *ptrPtr = tempPtr;
    *ptrSize = tempSize;
	return FUNCTION_NO_ERRORS;    
}

// Helper function to count set bits in the processor mask.
DWORD countSetBits( ULONG_PTR bitMask )
{
    DWORD LSHIFT = sizeof(ULONG_PTR)*8 - 1;
    DWORD bitSetCount = 0;
    ULONG_PTR bitTest = (ULONG_PTR)1 << LSHIFT;    
    DWORD i;
    
    for (i = 0; i <= LSHIFT; ++i)
    {
        bitSetCount += ((bitMask & bitTest)?1:0);
        bitTest/=2;
    }

    return bitSetCount;
}

// Print list of set bits, used for logical processors affinity masks
int scratchAffinity( char* scratchPointer, DWORD64 bitmap, int bufferLimit )
{
	int count = 0;
	int delta =0;
	DWORD64 mask = 1;
	int i = 0;
	int j = 0;
	
	while ( ( count < ( bufferLimit - ONE_ENTRY ) ) && ( i < 64 ) )
	{
		// Detect group of set bits
		while ( ( !( bitmap & mask ) ) && ( i < 64 ) )
		{
			i++;
			mask = (DWORD64)1<<i;
		}
		j = i;
		while ( ( bitmap & mask ) && ( j < 64 ) )
		{
			j++;
			mask = (DWORD64)1<<j;
		}
		j--;
		
		// Print geoup of set bits
		if ( i < 64 )
		{
			if ( count > 0 )
			{
				delta = snprintf( scratchPointer, ONE_ENTRY, ", " );
				count += delta;
				scratchPointer += delta;
			}
			
			if ( ( i == j ) && ( count < ( bufferLimit - ONE_ENTRY ) ) )
			{
				delta = snprintf( scratchPointer, ONE_ENTRY, "%d", i );
			}
			else if ( count < ( bufferLimit - ONE_ENTRY ) )
			{
				delta = snprintf( scratchPointer, ONE_ENTRY, "%d-%d", i, j );
			}
			else
			{
				delta = snprintf( scratchPointer, ONE_ENTRY, "..." );
			}
			
			count += delta;
			scratchPointer += delta;
			j++;
			i = j;
		}
	}
	
	return count;
}

