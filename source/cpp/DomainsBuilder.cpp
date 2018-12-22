/*
                     MEMORY PERFORMANCE ENGINE FRAMEWORK.
                          Functions loader class.
    This class make dynamical import for Windows API and this application DLL.
*/

#include "DomainsBuilder.h"

// Pointer to global control set of functions 
FUNCTIONS_LIST* DomainsBuilder::f = NULL;
// System NUMA topology summary report data
SYSTEM_NUMA_DATA DomainsBuilder::numaData;
// NUMA-style data for regular memory allocation mechanism, but NUMA-unaware
SYSTEM_NUMA_DATA DomainsBuilder::simpleData;
NUMA_NODE_ENTRY DomainsBuilder::simpleEntry;
// Status string
#define NS 81
char DomainsBuilder::s[NS];
// Width and up string for nodes list table, for NUMA-aware version
#define TABLE_WIDTH_NUMA 79
const char* DomainsBuilder::tableUpNuma =
	" ID affinity group\\mask   allocated base   allocated size   original (all hex)\r\n";
// Width and up string for nodes list table, for NUMA-unaware version
#define TABLE_WIDTH_SIMPLE 57
const char* DomainsBuilder::tableUpSimple =
	" ID  allocated base   allocated size   original (all hex)\r\n";
// Class constructor, 
// allocate memory for lists, initialize pointers, get NUMA topology data
DomainsBuilder::DomainsBuilder( FUNCTIONS_LIST* functions )
{
	// Blank status string
    snprintf( s, NS, "no data" );
	// global initialization and pre-blank output
	f = functions;
	numaData.nodeCount = 0;
	numaData.nodeList = NULL;
	simpleData.nodeCount = 1;
	simpleData.nodeList = &simpleEntry;
	blankNode( &simpleEntry );
	DWORD updateCount = 0;
	// allocate memory for NUMA nodes list
	int n = MAXIMUM_NODES;
	int mN = sizeof( NUMA_NODE_ENTRY ) * n;
	NUMA_NODE_ENTRY* pN = ( NUMA_NODE_ENTRY* )malloc( mN );
	numaData.nodeList = pN;
	// build list of domains, if memory allocated without errors
	if ( pN != NULL )
	{
		// Pre-blank allocated list
		for( int i=0; i<n; i++ )
		{
			blankNode( pN );
			pN++;
		}
		// Detect maximum number of NUMA nodes
		if ( f->API_GetNumaHighestNodeNumber != NULL )
		{
			ULONG nc = 0;
			BOOL numaStatus = ( f->API_GetNumaHighestNodeNumber ) ( &nc );
			if ( numaStatus )
			{
				numaData.nodeCount = nc + 1;  // nodes count = last node number + 1
				// Reload pointer after use it, select method for build list of nodes
				pN = numaData.nodeList;
				if ( f->API_GetNumaNodeProcessorMaskEx != NULL )
				{	// Build list of nodes, branch WITH support Processor Groups
					for( int i=0; i<MAXIMUM_NODES; i++ )
					{
						BOOL nodeStatus = ( f->API_GetNumaNodeProcessorMaskEx ) ( i, &pN->nodeGaff );
						if ( ( nodeStatus ) && ( pN->nodeGaff.Mask != 0 ) )
						{
							pN->nodeId = i;
							updateCount++;
							pN++;							
						}
						else
						{
							blankNode( pN );
						}
					}
				}
				else if ( f->API_GetNumaNodeProcessorMask != NULL )
				{	// Build list of nodes, branch WITHOUT support Processor Groups
				
					for( int i=0; i<MAXIMUM_NODES; i++ )
					{
						BOOL nodeStatus = ( f->API_GetNumaNodeProcessorMask ) ( i, ( PULONGLONG ) ( &pN->nodeGaff.Mask ) );
						if ( ( nodeStatus ) && ( pN->nodeGaff.Mask != 0 ) )
						{
							pN->nodeId = i;
							updateCount++;
							pN++;							
						}
						else
						{
							blankNode( pN );
						}
					}
				}
			}
		}
	}
	numaData.nodeCount = updateCount;
}

// Class destructor, release memory, allocated for lists
DomainsBuilder::~DomainsBuilder( )
{
	if ( numaData.nodeList != NULL )
	{
		free( numaData.nodeList );
	}
}

// Return pointer to domains topology data, NUMA-aware version
SYSTEM_NUMA_DATA* DomainsBuilder::getNumaList( )
{
	return &numaData;	
}

// Return pointer to domains topology data, NUMA-unaware version
SYSTEM_NUMA_DATA* DomainsBuilder::getSimpleList( )
{
	return &simpleData;	
}

// Get text report, for NUMA-aware version
void DomainsBuilder::getNumaText( LPSTR &dst, size_t &max )
{
	// Detected nodes count and allocated memory base address
	int count = 0;
	int x1 = numaData.nodeCount;
	LPVOID x2 = numaData.nodeList;
	count = snprintf( dst, max, "NUMA nodes detected = %d , list allocated at base = ", x1 );
	dst += count;
	max -= count;
	AppLib::printPointer( dst, max, x2, TRUE );
	AppLib::printCrLf( dst, max );
	AppLib::printCrLf( dst, max );
	// Nodes table
	NUMA_NODE_ENTRY* pN = numaData.nodeList;
	if ( ( x1 > 0 ) && ( x2 != NULL ) )
	{
		AppLib::printString( dst, max, tableUpNuma );
		AppLib::printLine( dst, max, TABLE_WIDTH_NUMA );
		// Cycle for NUMA nodes
		for( int i=0; i<x1; i++ )
		{
			AppLib::printString( dst, max, " " );
			AppLib::printCellNum( dst, max, i, 3 );
			DWORD32 y1 = pN->nodeGaff.Group;
			AppLib::print16( dst, max, y1, FALSE );
			AppLib::printString( dst, max, "\\" );
			DWORD64 y2 = pN->nodeGaff.Mask;
			AppLib::print64( dst, max, y2, FALSE );
			AppLib::printCell( dst, " ", max, 1 );
			y2 = ( DWORD64 ) pN->baseAtNode;
			AppLib::print64( dst, max, y2, FALSE );
			AppLib::printCell( dst, " ", max, 1 );
			y2 = ( DWORD64 ) pN->sizeAtNode;
			AppLib::print64( dst, max, y2, FALSE );
			AppLib::printCell( dst, " ", max, 1 );
			y2 = ( DWORD64 ) pN->trueBaseAtNode;
			AppLib::print64( dst, max, y2, FALSE );
			AppLib::printCell( dst, " ", max, 1 );
			AppLib::printCrLf( dst, max );
			pN++;
		}
		AppLib::printLine( dst, max, TABLE_WIDTH_NUMA );
	}
}

// Get text report, for NUMA-unaware version
void DomainsBuilder::getSimpleText( LPSTR &dst, size_t &max )
{
	// Detected nodes count and allocated memory base address
	int count = 0;
	int x1 = simpleData.nodeCount;
	LPVOID x2 = simpleData.nodeList;
	count = snprintf( dst, max, "NUMA-unaware mode, list allocated at base = " );
	dst += count;
	max -= count;
	AppLib::printPointer( dst, max, x2, TRUE );
	AppLib::printCrLf( dst, max );
	AppLib::printCrLf( dst, max );
	// Nodes table
	NUMA_NODE_ENTRY* pN = simpleData.nodeList;
	if ( ( x1 > 0 ) && ( x2 != NULL ) )
	{
		AppLib::printString( dst, max, tableUpSimple );
		AppLib::printLine( dst, max, TABLE_WIDTH_SIMPLE );
		// Cycle for NUMA nodes
		for( int i=0; i<x1; i++ )
		{
			AppLib::printString( dst, max, " " );
			AppLib::printCellNum( dst, max, i, 3 );
			AppLib::printCell( dst, " ", max, 1 );
			DWORD64 y2 = ( DWORD64 ) pN->baseAtNode;
			AppLib::print64( dst, max, y2, FALSE );
			AppLib::printCell( dst, " ", max, 1 );
			y2 = ( DWORD64 ) pN->sizeAtNode;
			AppLib::print64( dst, max, y2, FALSE );
			AppLib::printCell( dst, " ", max, 1 );
			y2 = ( DWORD64 ) pN->trueBaseAtNode;
			AppLib::print64( dst, max, y2, FALSE );
			AppLib::printCell( dst, " ", max, 1 );
			AppLib::printCrLf( dst, max );
			pN++;
		}
		AppLib::printLine( dst, max, TABLE_WIDTH_SIMPLE );
	}
}

// Method returns status string, valid if error returned
char* DomainsBuilder::getStatusString( )
{
    return s;
}

// Memory allocation methods

// Allocate memory to NUMA nodes, enumerated by existed list
BOOL DomainsBuilder::allocateNodesList( size_t xs, DWORD pgMode, DWORD64 pgSize, BOOL swapFlag )
{
	// Initializing
	NUMA_NODE_ENTRY* xp = numaData.nodeList;
	int n = numaData.nodeCount;
	SIZE_T allocSize = alignByFactor( xs, pgSize );  // alignment by selected page size
	DWORD allocType = MEM_RESERVE + MEM_COMMIT;
	if ( pgMode == LP_USED )
	{
		allocType |= MEM_LARGE_PAGES;
	}
	// Get Process Handle
	HANDLE h = GetCurrentProcess( );
	if ( h == NULL ) return FALSE;
	// Allocate memory for NUMA nodes
	int i = 0;
	if ( f->API_VirtualAllocExNuma != NULL )
	{
		for( i=0; i<n; i++ )
		{
			LPVOID base = ( f->API_VirtualAllocExNuma ) ( h, NULL, allocSize, allocType, PAGE_READWRITE, xp->nodeId );
			xp->baseAtNode = base;
			xp->sizeAtNode = allocSize;
			xp->trueBaseAtNode = base;
			if ( base == NULL ) return FALSE;
			xp++;
		}
		// Support domains swap mode for NUMA remote option
		xp = numaData.nodeList;
		if ( swapFlag )
		{
			n--;
			for( i=0; i<n; i++ )
			{
			LPVOID x1 = xp->baseAtNode;         // load X from entry[i]
			SIZE_T x2 = xp->sizeAtNode;
			LPVOID x3 = xp->trueBaseAtNode;
			xp++;
			LPVOID y1 = xp->baseAtNode;         // load Y from entry[i+1]
			SIZE_T y2 = xp->sizeAtNode;
			LPVOID y3 = xp->trueBaseAtNode;
			xp->baseAtNode = x1;                // save X to entry[i+2]
			xp->sizeAtNode = x2;
			xp->trueBaseAtNode = x3;
			xp--;
			xp->baseAtNode = y1;                // save Y to entry[i]
			xp->sizeAtNode = y2;
			xp->trueBaseAtNode = y3;
			xp++;                               // advance pointer to next pair
			}
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// Release memory allocated at NUMA nodes, enumerated by existed list
BOOL DomainsBuilder::freeNodesList( )
{
	// Initializing
	NUMA_NODE_ENTRY* xp = numaData.nodeList;
	int n = numaData.nodeCount;
	int i = 0;
	BOOL status = TRUE;
	// Get Process Handle
	HANDLE h = GetCurrentProcess( );
	if ( h == NULL ) return FALSE;
	// Release memory for NUMA nodes
	for( i=0; i<n; i++ )
	{
		LPVOID trueBase = xp->trueBaseAtNode;
		if ( trueBase != NULL )
		{
			BOOL status1 = VirtualFreeEx( h, trueBase, 0, MEM_RELEASE );
			status = status && status1;
		}
		xp->baseAtNode = NULL;
		xp->sizeAtNode = 0;
		xp->trueBaseAtNode = NULL;
		xp++;
	}
	return status;
}

// Allocate memory for simple (NUMA-unaware) mode
BOOL DomainsBuilder::allocateSimpleList( size_t xs, DWORD pgMode, DWORD64 pgSize )
{
	// Initializing
	NUMA_NODE_ENTRY* xp = simpleData.nodeList;
	SIZE_T allocSize = alignByFactor( xs, pgSize );  // alignment by selected page size
	DWORD allocType = MEM_RESERVE + MEM_COMMIT;
	if ( pgMode == LP_USED )
	{
		allocType |= MEM_LARGE_PAGES;
	}
	xp->trueBaseAtNode = VirtualAlloc( NULL, allocSize, allocType, PAGE_READWRITE );
	if ( xp->trueBaseAtNode != NULL )
	{
		xp->baseAtNode = xp->trueBaseAtNode;
		xp->sizeAtNode = allocSize;
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// Release memory for simple (NUMA-unaware) mode
BOOL DomainsBuilder::freeSimpleList( )
{
	NUMA_NODE_ENTRY* xp = simpleData.nodeList;
	BOOL status = TRUE;
	if (  xp->trueBaseAtNode != NULL )
	{
		status = VirtualFree( xp->trueBaseAtNode, 0, MEM_RELEASE );
	}
	xp->trueBaseAtNode = NULL;
	xp->baseAtNode = NULL;
	xp->sizeAtNode = 0;
}

// Helpers methods

// Helper method, blank NUMA node entry
void DomainsBuilder::blankNode( NUMA_NODE_ENTRY* p )
{
	p->nodeId = 0;
	p->nodeGaff.Mask = 0;
	p->nodeGaff.Group = 0;
	p->nodeGaff.Reserved[0];
	p->nodeGaff.Reserved[1];
	p->nodeGaff.Reserved[2];
	p->baseAtNode = NULL;
	p->sizeAtNode = 0;
	p->trueBaseAtNode = NULL;
}

// Helper method, block size alignment
DWORD64 DomainsBuilder::alignByFactor( DWORD64 value, DWORD64 factor )
{
	DWORD64 x = value % factor;
	if ( x )
	{
		x = factor - x;
	}
	else
	{
		x = 0;
	}
	value += x;
	return value;
}


