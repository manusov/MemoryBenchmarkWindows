#include "SystemTopology.h"

// Status string
#define NS 81
char SystemTopology::s[NS];

// Class constructor
SystemTopology::SystemTopology( )
{
	snprintf( s, NS, "no data" );
}

// Class destructor
SystemTopology::~SystemTopology( )
{

}

// Note about statical or dynamical import selection.
// WinAPI function GetLogicalProcessorInformation available at WinXP, can use statical import
// WinAPI function GetLogicalProcessorInformationEx not available at WinXP, required dynamical import,
// this fact actual for future extensions
// Cache detection (by WinAPI) and topology scan method
DWORD SystemTopology::detectTopology( SYSTEM_TOPOLOGY_DATA* xc )
{
    DWORD error;
	// Pre-blank output
    xc->pointL1 = 0;
    xc->pointL2 = 0;
    xc->pointL3 = 0;
    xc->coreCount = 0;
    xc->hyperThreadingFlag = 0;
    // Initializing variables
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION bufferPtr = NULL;
    DWORD bufferSize = 0;
    // Get information into buffer, note required memory allocation for buffer
    while ( !done )
    {
        BOOL valid = GetLogicalProcessorInformation( bufferPtr, &bufferSize );
        if ( !valid ) 
		{
			error = GetLastError( );
            if ( error == ERROR_INSUFFICIENT_BUFFER ) 
            {
                if ( bufferPtr ) 
                    free( bufferPtr );

                bufferPtr = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION )malloc( bufferSize );

                if ( bufferPtr == NULL ) 
                {
                    error = GetLastError( );
					snprintf( s, NS, "allocate buffer for logical processors descriptors" );
                    return error;
                }
            } 
            else 
            {
                snprintf( s, NS, "get logical processors information" );
                return error;
            }
        } 
        else
        {
            done = TRUE;
        }
    }
    // Local variables fpr analysing buffer content
    PCACHE_DESCRIPTOR Cache;
    DWORD relationship;
    int bufferCount = bufferSize / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION );
    int i;
    // Analysing buffer content, update output
    for( i=0; i<bufferCount; i++ )
    {
    relationship = bufferPtr->Relationship;
    switch ( relationship )
        {
            case RelationCache:
            Cache = &bufferPtr->Cache;
            switch ( Cache->Level )
            {
                case 1:
                    if ( ( Cache->Type == CacheUnified )||( Cache->Type == CacheData ) )
                    {
                        xc->pointL1 = Cache->Size;
                    }
                    break;
                case 2:
                    if ( ( Cache->Type == CacheUnified )||( Cache->Type == CacheData ) )
                    {
                        xc->pointL2 = Cache->Size;
                    }
                    break;
                case 3:
                    if ( ( Cache->Type == CacheUnified )||( Cache->Type == CacheData ) )
                    {
                        xc->pointL3 = Cache->Size;
                    }
                    break;
                default:
                    break;
            }
            break;
            case RelationProcessorCore:
            	xc->coreCount++;
                if ( ( bufferPtr->ProcessorCore.Flags ) != 0 )
                {
                    xc->hyperThreadingFlag = 1;
                }
                break;
            case RelationNumaNode:
                break;
            case RelationProcessorPackage:
                break;
            default:
                break;
        }
    bufferPtr++;
    }
	return 0;
}

// Method returns status string, valid if error returned
char* SystemTopology::getStatusString( )
{
    return s;
}

