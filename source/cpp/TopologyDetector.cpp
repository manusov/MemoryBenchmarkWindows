/*
                MEMORY PERFORMANCE ENGINE FRAMEWORK.
        Windows SMP/Cache topology detection API interface class.
    This method can be functional even if cache not detected by CPUID.
*/

#include "TopologyDetector.h"

// Pointer to global control set of functions 
FUNCTIONS_LIST* TopologyDetector::f = NULL;
// Pointers to standard and extended topology buffer (dynamically allocated buffers)
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION    TopologyDetector::pTopologyBuffer   = NULL;
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX TopologyDetector::pTopologyBufferEx = NULL;
// Limits pointers for standard and extended topology buffer (dynamically allocated buffers)
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION    TopologyDetector::pTopologyLimit   = NULL;
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX TopologyDetector::pTopologyLimitEx = NULL;
// Sizes of allocated buffers for standard and extended topology data
DWORD TopologyDetector::topologySize = 0;
DWORD TopologyDetector::topologySizeEx = 0;
// Topology summary report data, standard and extended versions
SYSTEM_TOPOLOGY_DATA TopologyDetector::topologyData;
SYSTEM_TOPOLOGY_DATA TopologyDetector::topologyDataEx;
// Tables up strings, standard version of system information
const char* TopologyDetector::tableUpCores =
	" core  affinity mask (hex)   ht flag   logical processors\n";
const char* TopologyDetector::tableUpCaches =
	" cache  type          size (KB)    associativity   line     logical processors\n";
const char* TopologyDetector::tableUpDomains =
	" domain   affinity mask (hex)   logical processors\n";
const char* TopologyDetector::tableUpPackages =
	" package   affinity mask (hex)   logical processors\n";
// Tables up strings, extended version of system information
const char* TopologyDetector::tableUpCoresEx =
	" core   ht   efficiency   group and affinity mask (hex)\n";
const char* TopologyDetector::tableUpCachesEx =
	" cache  type          size (KB)          group and affinity mask (hex)\n";
const char* TopologyDetector::tableUpDomainsEx =
	" domain   group and affinity mask (hex)\n";
//const char* TopologyDetector::tableUpPackagesEx =
//	" package   group and affinity mask (hex)\n";
const char* TopologyDetector::tableUpGroupsEx =
	" group   maximum   active   maximum   active   affinity mask (hex)\n";

// Cache types names strings
const char* TopologyDetector::cacheTypes[] =
{
	"unified",
	"instruction",
	"data",
	"trace"
};
// Status string
#define NS 81
char TopologyDetector::s[NS];

// Class constructor, initialize pointers, get topology data
TopologyDetector::TopologyDetector( FUNCTIONS_LIST* functions )
{
	// Blank status string
    snprintf( s, NS, "no data" );
	// global initialization
	f = functions;
	blankTopologyData( &topologyData );
	blankTopologyData( &topologyDataEx );
	pTopologyBuffer = NULL;
	pTopologyBufferEx = NULL;
	pTopologyLimit = NULL;
	pTopologyLimitEx = NULL;
	topologySize = 0;
	topologySizeEx = 0;
	// local initialization, for standard version
	BOOL status = FALSE;
	BOOL done = FALSE;
	DWORD error = 0;
    // Get topology information into buffer, note required memory allocation for buffer
    // Standard version
    if ( f->API_GetLogicalProcessorInformation != NULL )
    {
	    while ( !done )
    	{
        	status = ( f->API_GetLogicalProcessorInformation )( pTopologyBuffer, &topologySize );
        	if ( !status ) 
			{
				error = GetLastError( );
            	if ( error == ERROR_INSUFFICIENT_BUFFER ) 
            	{
                	if ( pTopologyBuffer )
                	{
						free( pTopologyBuffer );
					}
					pTopologyBuffer = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION )malloc( topologySize );
                	if ( pTopologyBuffer == NULL ) 
                	{
                    	error = GetLastError( );
						snprintf( s, NS, "allocate buffer for logical processors descriptors (standard)" );
                    	done = TRUE;  // stop if still error after memory allocation
                	}
                	else
                	{
                		done = FALSE;  // make next iteration if memory allocated OK, second try get topology info
					}
            	} 
            	else 
            	{
                	snprintf( s, NS, "get logical processors information (standard)" );
                	done = TRUE;  // stop if other error type, not insufficient memory
            	}
        	} 
        	else
        	{
            	done = TRUE;  // stop if first call success (this impossible because size=0 at first call)
        	}
    	}
	}
	pTopologyLimit = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION ) ( ( LPBYTE )pTopologyBuffer + topologySize );
	// local re-initialization, for extended version
	status = FALSE;
	done = FALSE;
	error = 0;
    // Get topology information into buffer, note required memory allocation for buffer
    // Extended version
    if ( f->API_GetLogicalProcessorInformationEx != NULL )
    {
	    while ( !done )
    	{
        	status = ( f->API_GetLogicalProcessorInformationEx )( RelationAll, pTopologyBufferEx, &topologySizeEx );
        	if ( !status ) 
			{
				error = GetLastError( );
            	if ( error == ERROR_INSUFFICIENT_BUFFER ) 
            	{
                	if ( pTopologyBufferEx )
                	{
						free( pTopologyBufferEx );
					}
					pTopologyBufferEx = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX )malloc( topologySizeEx );
                	if ( pTopologyBufferEx == NULL ) 
                	{
                    	error = GetLastError( );
						snprintf( s, NS, "allocate buffer for logical processors descriptors (standard)" );
                    	done = TRUE;  // stop if still error after memory allocation
                	}
                	else
                	{
                		done = FALSE;  // make next iteration if memory allocated OK, second try get topology info
					}
            	} 
            	else 
            	{
                	snprintf( s, NS, "get logical processors information (standard)" );
                	done = TRUE;  // stop if other error type, not insufficient memory
            	}
        	} 
        	else
        	{
            	done = TRUE;  // stop if first call success (this impossible because size=0 at first call)
        	}
    	}
	}
	pTopologyLimitEx = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ) ( ( LPBYTE )pTopologyBufferEx + topologySizeEx );
	// Decode topology information, argument = WinAPI returned buffer, result = summary data
	// Standard version
	PCACHE_DESCRIPTOR cache;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION p1 = pTopologyBuffer;
	SYSTEM_TOPOLOGY_DATA* p2 = &topologyData;
	// Cycle for entries
	while( p1 < pTopologyLimit )
	{
		switch( p1->Relationship )
		{
			case RelationCache:
				cache = &p1->Cache;
				if ( ( cache->Type == CacheUnified )||( cache->Type == CacheData ) )
				{
					switch( cache->Level )
					{
						case 1:
							p2->pointL1 = cache->Size;
							break;
						case 2:
							p2->pointL2 = cache->Size;
							break;
						case 3:
							p2->pointL3 = cache->Size;
							break;
						case 4:
							p2->pointL4 = cache->Size;
							break;
						default:
							break;
					}
				}
				break;
			case RelationProcessorCore:
				p2->coresCount++;
				if ( ( p1->ProcessorCore.Flags ) != 0 )
                {
                    p2->hyperThreadingFlag = 1;
                }
				break;
			case RelationNumaNode:
				p2->domainsCount++;
				break;
			case RelationProcessorPackage:
				p2->packagesCount++;
				break;
			default:
				break;
		}
	p1++;
	}
	// Decode topology information, argument = WinAPI returned buffer, result = summary data
	// Extended version
	PCACHE_RELATIONSHIP cacheEx;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX p3 = pTopologyBufferEx;
	SYSTEM_TOPOLOGY_DATA* p4 = &topologyDataEx;
	// Cycle for entries
	while( p3 < pTopologyLimitEx )
	{
		switch( p3->Relationship )
		{
			case RelationCache:
				cacheEx = &p3->Cache;
				if ( ( cacheEx->Type == CacheUnified )||( cacheEx->Type == CacheData ) )
				{
					switch( cacheEx->Level )
					{
						case 1:
							p4->pointL1 = cacheEx->CacheSize;
							break;
						case 2:
							p4->pointL2 = cacheEx->CacheSize;
							break;
						case 3:
							p4->pointL3 = cacheEx->CacheSize;
							break;
						case 4:
							p4->pointL4 = cacheEx->CacheSize;
							break;
						default:
							break;
					}
				}
				break;
			case RelationProcessorCore:
				p4->coresCount++;
				if ( ( p3->Processor.Flags ) != 0 )
                {
                    p4->hyperThreadingFlag = 1;
                }
				break;
			case RelationNumaNode:
				p4->domainsCount++;
				break;
			case RelationProcessorPackage:
				p4->packagesCount++;
				break;
			case RelationGroup:
				break;
			default:
				break;
		}
		p3 = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ) ( ( LPBYTE )p3 + p3->Size );
	}
}
// Helper function for constructor, blank topology data
void TopologyDetector::blankTopologyData( SYSTEM_TOPOLOGY_DATA* p )
{
	p->pointL1 = 0;
	p->pointL2 = 0;
	p->pointL3 = 0;
	p->pointL4 = 0;
	p->coresCount = 0;
	p->domainsCount = 0;
	p->packagesCount = 0;
	p->hyperThreadingFlag = 0;
}

// Class destructor, release memory: buffers of topology data
TopologyDetector::~TopologyDetector( )
{
	if ( pTopologyBuffer )
    {
		free( pTopologyBuffer );
	}
	if ( pTopologyBufferEx )
    {
		free( pTopologyBufferEx );
	}
}

// Return pointer to topology data, result of standard function
SYSTEM_TOPOLOGY_DATA* TopologyDetector::getTopologyList( )
{
	return &topologyData;
}

// Return pointer to topology data, result of extended function
SYSTEM_TOPOLOGY_DATA* TopologyDetector::getTopologyListEx( )
{
	return &topologyDataEx;
}

// Get text report by standard information
#define TABLE_WIDTH_CORES 58
#define TABLE_WIDTH_CACHES 79
#define TABLE_WIDTH_DOMAINS 51
#define TABLE_WIDTH_PACKAGES 52
#define CORES_COLUMN_1 6
#define CORES_COLUMN_ADD_1 6
#define CORES_COLUMN_2 10
#define CACHES_COLUMN_1 5
#define CACHES_COLUMN_2 14
#define CACHES_COLUMN_3 13
#define CACHES_COLUMN_4 16
#define CACHES_COLUMN_5 9
#define DOMAINS_COLUMN_1 9
#define DOMAINS_COLUMN_2 6
#define PACKAGES_COLUMN_1 10
#define PACKAGES_COLUMN_2 6

void TopologyDetector::getTopologyText( LPSTR &dst, size_t &max )
{
	// Table 1 of 5 = cores
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pbuf = pTopologyBuffer;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pmax = pTopologyLimit;
	AppLib::printString( dst, max, tableUpCores );
	AppLib::printLine( dst, max, TABLE_WIDTH_CORES );
	int i = 0;
	while( pbuf < pmax )
	{
		if ( pbuf->Relationship == RelationProcessorCore )
		{
			AppLib::printString( dst, max, " " );
			AppLib::printCellNum( dst, max, i, CORES_COLUMN_1 );
			AppLib::print64( dst, max, pbuf->ProcessorMask, FALSE );
			AppLib::printCell( dst, " ", max, CORES_COLUMN_ADD_1 );
			AppLib::printCellNum( dst, max, pbuf->ProcessorCore.Flags, CORES_COLUMN_2 );
			AppLib::printBitsList( dst, max, pbuf->ProcessorMask );
			AppLib::printCrLf( dst, max );
			i++;
		}
		pbuf++;		
	}
	AppLib::printLine( dst, max, TABLE_WIDTH_CORES );
	AppLib::printCrLf( dst, max );
	// Table 2 of 5 = caches
	PCACHE_DESCRIPTOR cache;
	pbuf = pTopologyBuffer;
	pmax = pTopologyLimit;
	AppLib::printString( dst, max, tableUpCaches );
	AppLib::printLine( dst, max, TABLE_WIDTH_CACHES );
	while( pbuf < pmax )
	{
		if ( pbuf->Relationship == RelationCache )
		{
			AppLib::printString( dst, max, " L" );
			cache = &pbuf->Cache;
			int k = cache->Level;
			if ( ( k>0 )&&( k<5 ) )
			{
				AppLib::printCellNum( dst, max, k, 1 );
			}
			else
			{
				AppLib::printString( dst, max, "?" );
			}
			AppLib::printCell( dst, " ", max, CACHES_COLUMN_1 );
			k = cache->Type;
			if ( ( k>=0 )&&( k<=3 ) )
			{
				AppLib::printCell( dst, cacheTypes[k], max, CACHES_COLUMN_2 );
			}
			else
			{
				AppLib::printCell( dst, "unknown",  max, CACHES_COLUMN_2 );
			}
			int sizeKB = ( cache->Size ) / 1024;
			AppLib::printCellNum( dst, max, sizeKB, CACHES_COLUMN_3 );
			int associativity = cache->Associativity;
			if ( associativity == CACHE_FULLY_ASSOCIATIVE )
			{
				AppLib::printCell( dst, "full", max, CACHES_COLUMN_4 );
			}
			else
			{
				AppLib::printCellNum( dst, max, associativity, CACHES_COLUMN_4 );
			}
			int lineSize = cache->LineSize;
			AppLib::printCellNum( dst, max, lineSize, CACHES_COLUMN_5 );
			ULONG_PTR processorMask = pbuf->ProcessorMask;
			AppLib::printBitsList( dst, max, pbuf->ProcessorMask );
			AppLib::printCrLf( dst, max );
		}
		pbuf++;		
	}
	AppLib::printLine( dst, max, TABLE_WIDTH_CACHES );
	AppLib::printCrLf( dst, max );
	// Table 3 of 5 = NUMA domains
	pbuf = pTopologyBuffer;
	pmax = pTopologyLimit;
	AppLib::printString( dst, max, tableUpDomains );
	AppLib::printLine( dst, max, TABLE_WIDTH_DOMAINS );
	i = 0;
	while( pbuf < pmax )
	{
		if ( pbuf->Relationship == RelationNumaNode )
		{
			AppLib::printString( dst, max, " " );
			AppLib::printCellNum( dst, max, i, DOMAINS_COLUMN_1 );
			AppLib::print64( dst, max, pbuf->ProcessorMask, FALSE );
			AppLib::printCell( dst, " ", max, DOMAINS_COLUMN_2 );
			AppLib::printBitsList( dst, max, pbuf->ProcessorMask );
			AppLib::printCrLf( dst, max );
			i++;
		}
		pbuf++;
	}
	AppLib::printLine( dst, max, TABLE_WIDTH_DOMAINS );
	AppLib::printCrLf( dst, max );
	// Table 4 of 5 = processor packages
	pbuf = pTopologyBuffer;
	pmax = pTopologyLimit;
	AppLib::printString( dst, max, tableUpPackages );
	AppLib::printLine( dst, max, TABLE_WIDTH_PACKAGES );
	i = 0;
	while( pbuf < pmax )
	{
		if ( pbuf->Relationship == RelationProcessorPackage )
		{
			AppLib::printString( dst, max, " " );
			AppLib::printCellNum( dst, max, i, PACKAGES_COLUMN_1 );
			AppLib::print64( dst, max, pbuf->ProcessorMask, FALSE );
			AppLib::printCell( dst, " ", max, PACKAGES_COLUMN_2 );
			AppLib::printBitsList( dst, max, pbuf->ProcessorMask );
			AppLib::printCrLf( dst, max );
			i++;
		}
		pbuf++;
	}
	AppLib::printLine( dst, max, TABLE_WIDTH_PACKAGES );
	// AppLib::printCrLf( dst, max );
	// Table 5 of 5 = summary
	// This table yet reserved
}

// Get text report by extended information
#define EX_TABLE_WIDTH_CORES 57
#define EX_TABLE_WIDTH_CACHES 71
#define EX_TABLE_WIDTH_DOMAINS 40
// #define EX_TABLE_WIDTH_PACKAGES 41
#define EX_TABLE_WIDTH_GROUPS 67

#define EX_CORES_COLUMN_1 7
#define EX_CORES_COLUMN_2 5
#define EX_CORES_COLUMN_3 13
#define EX_CACHES_COLUMN_1 5
#define EX_CACHES_COLUMN_2 14
#define EX_CACHES_COLUMN_3 19
#define EX_DOMAINS_COLUMN_1 9
// #define EX_PACKAGES_COLUMN_1 10
#define EX_GROUPS_COLUMN_1 8
#define EX_GROUPS_COLUMN_2 9
#define EX_GROUPS_COLUMN_3 10
#define EX_GROUPS_COLUMN_4 9
#define EX_GROUPS_COLUMN_5 10

typedef struct PROCESSOR_RELATIONHIP_REDEFINE {
	BYTE Flags;
	BYTE EfficiencyClass;
	BYTE Reserved[20];
	WORD GroupCount;
	GROUP_AFFINITY GroupMask[ANYSIZE_ARRAY];
} PROCESSOR_RELATIONHIP_REDEFINE, *PPROCESSOR_RELATIONHIP_REDEFINE;


void TopologyDetector::getTopologyTextEx( LPSTR &dst, size_t &max )
{
	// Table 1 of 6 = cores
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pbuf = pTopologyBufferEx;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pmax = pTopologyLimitEx;
	AppLib::printString( dst, max, tableUpCoresEx );
	AppLib::printLine( dst, max, EX_TABLE_WIDTH_CORES );
	int i = 0;
	while( pbuf < pmax )
	{
		if ( pbuf->Relationship == RelationProcessorCore )
		{
			AppLib::printString( dst, max, " " );
			AppLib::printCellNum( dst, max, i, EX_CORES_COLUMN_1 );
			PPROCESSOR_RELATIONHIP_REDEFINE px = ( PPROCESSOR_RELATIONHIP_REDEFINE ) ( &pbuf->Processor);
			AppLib::printCellNum( dst, max, px->Flags, EX_CORES_COLUMN_2 );
			AppLib::printCellNum( dst, max, px->EfficiencyClass, EX_CORES_COLUMN_3 );
			PGROUP_AFFINITY py = px->GroupMask;
			int n = px->GroupCount;
			int skip = 1 + EX_CORES_COLUMN_1 + EX_CORES_COLUMN_2 + EX_CORES_COLUMN_3;
			printGroups( dst, max, skip, py, n );
			AppLib::printCrLf( dst, max );
			i++;
		}
		pbuf = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ) ( ( LPBYTE )pbuf + pbuf->Size );
	}
	AppLib::printLine( dst, max, EX_TABLE_WIDTH_CORES );
	AppLib::printCrLf( dst, max );
	// Table 2 of 6 = caches
	pbuf = pTopologyBufferEx;
	pmax = pTopologyLimitEx;
	PCACHE_RELATIONSHIP cacheEx;
	AppLib::printString( dst, max, tableUpCachesEx );
	AppLib::printLine( dst, max, EX_TABLE_WIDTH_CACHES );
	while( pbuf < pmax )
	{
		if ( pbuf->Relationship == RelationCache )
		{
			AppLib::printString( dst, max, " L" );
			cacheEx = &pbuf->Cache;
			int k = cacheEx->Level;
			if ( ( k>0 )&&( k<5 ) )
			{
				AppLib::printCellNum( dst, max, k, 1 );
			}
			else
			{
				AppLib::printString( dst, max, "?" );
			}
			AppLib::printCell( dst, " ", max, EX_CACHES_COLUMN_1 );
			k = cacheEx->Type;
			if ( ( k>=0 )&&( k<=3 ) )
			{
				AppLib::printCell( dst, cacheTypes[k], max, EX_CACHES_COLUMN_2 );
			}
			else
			{
				AppLib::printCell( dst, "unknown",  max, EX_CACHES_COLUMN_2 );
			}
			int sizeKB = ( cacheEx->CacheSize ) / 1024;
			AppLib::printCellNum( dst, max, sizeKB, EX_CACHES_COLUMN_3 );
			PGROUP_AFFINITY py = &cacheEx->GroupMask;
			int n = 1;
			int skip = 1 + EX_CACHES_COLUMN_1 + EX_CACHES_COLUMN_2 + EX_CACHES_COLUMN_3;
			printGroups( dst, max, skip, py, n );
			AppLib::printCrLf( dst, max );
		}
		pbuf = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ) ( ( LPBYTE )pbuf + pbuf->Size );
	}
	AppLib::printLine( dst, max, EX_TABLE_WIDTH_CACHES );
	AppLib::printCrLf( dst, max );
	// Table 3 of 6 = NUMA domains
	pbuf = pTopologyBufferEx;
	pmax = pTopologyLimitEx;
	PNUMA_NODE_RELATIONSHIP numaEx;
	AppLib::printString( dst, max, tableUpDomainsEx );
	AppLib::printLine( dst, max, EX_TABLE_WIDTH_DOMAINS );
	i = 0;
	while( pbuf < pmax )
	{
		if ( pbuf->Relationship == RelationNumaNode )
		{
			AppLib::printString( dst, max, " " );
			AppLib::printCellNum( dst, max, i, EX_DOMAINS_COLUMN_1 );
			numaEx = &pbuf->NumaNode;
			PGROUP_AFFINITY py = &numaEx->GroupMask;
			int n = 1;
			int skip = 1 + EX_DOMAINS_COLUMN_1;
			printGroups( dst, max, skip, py, n );
			AppLib::printCrLf( dst, max );
			i++;
		}
		pbuf = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ) ( ( LPBYTE )pbuf + pbuf->Size );
	}
	AppLib::printLine( dst, max, EX_TABLE_WIDTH_DOMAINS );
	AppLib::printCrLf( dst, max );
	// Table 4 of 6 = processor packages
	// Not supported, see Processor Relationship
	// Table 4 of 6 = processor groups
	pbuf = pTopologyBufferEx;
	pmax = pTopologyLimitEx;
	PGROUP_RELATIONSHIP groupEx;
	AppLib::printString( dst, max, tableUpGroupsEx );
	AppLib::printLine( dst, max, EX_TABLE_WIDTH_GROUPS );
	i = 0;
	while( pbuf < pmax )
	{
		if ( pbuf->Relationship == RelationGroup )
		{
			AppLib::printString( dst, max, " " );
			AppLib::printCellNum( dst, max, i, EX_GROUPS_COLUMN_1 );
			groupEx = &pbuf->Group;
			int maxGroups = groupEx->MaximumGroupCount;
			AppLib::printCellNum( dst, max, maxGroups, EX_GROUPS_COLUMN_2 );
			int activeGroups = groupEx->ActiveGroupCount;
			AppLib::printCellNum( dst, max, activeGroups, EX_GROUPS_COLUMN_3 );
			PPROCESSOR_GROUP_INFO pGinfo = groupEx->GroupInfo;
			int skipLeft = 1 + EX_GROUPS_COLUMN_1 + EX_GROUPS_COLUMN_2 + EX_GROUPS_COLUMN_3;
			for( int j=0; j<activeGroups; j++ )
			{
				if ( j > 0 )
				{
					AppLib::printCrLf( dst, max );
					AppLib::printCell( dst, " ", max, skipLeft );
				}
				AppLib::printCellNum( dst, max, pGinfo->MaximumProcessorCount, EX_GROUPS_COLUMN_4 );
				AppLib::printCellNum( dst, max, pGinfo->ActiveProcessorCount, EX_GROUPS_COLUMN_5 );
				AppLib::print64( dst, max, pGinfo->ActiveProcessorMask, FALSE );
				pGinfo++;
			}
			AppLib::printCrLf( dst, max );
			i++;
		}
		pbuf = ( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX ) ( ( LPBYTE )pbuf + pbuf->Size );
	}
	AppLib::printLine( dst, max, EX_TABLE_WIDTH_GROUPS );
	// AppLib::printCrLf( dst, max );
	// Table 6 of 6 = summary
	// This table yet reserved
}
// Helper method: print sequence of group affinity structures
void TopologyDetector::printGroups( LPSTR &dst, size_t &max, int skipLeft, PGROUP_AFFINITY pGaff, int groupCount )
{
	for( int j=0; j<groupCount; j++ )
	{
		if ( j > 0 )
		{
			AppLib::printCrLf( dst, max );
			AppLib::printCell( dst, " ", max, skipLeft );
		}
		AppLib::print16( dst, max, pGaff->Group, FALSE );
		AppLib::printString( dst, max, " \\ " );
		AppLib::print64( dst, max, pGaff->Mask, FALSE );
		pGaff++;
	}
	
}

// Method get brief information about cache levels
void TopologyDetector::getCacheText( LPSTR &dst, size_t &max )
{
	// ... reserved ...
}


// Method returns status string, valid if error returned
char* TopologyDetector::getStatusString( )
{
    return s;
}



