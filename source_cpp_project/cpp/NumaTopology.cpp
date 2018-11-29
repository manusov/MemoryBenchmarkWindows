#include "NumaTopology.h"

// Status string
#define NS 81
char NumaTopology::s[NS];

const char* NumaTopology::ncsLibrary = "kernel32";
NUMA_CONTROL_SET NumaTopology::ncs;

// Class constructor, blank status string
NumaTopology::NumaTopology( )
{
    snprintf( s, NS, "no data" );
}

// Class destructor, yet no operations
NumaTopology::~NumaTopology( )
{
    
}

// Load functions, used for NUMA, by dynamical import
void NumaTopology::loadControlSet( )
{
	ncs.API_GlobalMemoryStatusEx = NULL;
	ncs.API_GetNumaHighestNodeNumber = NULL;
	ncs.API_GetNumaNodeProcessorMask = NULL;
	ncs.API_VirtualAllocExNuma = NULL;
	ncs.API_SetThreadAffinityMask = NULL;
	ncs.API_GetSystemFirmwareTable = NULL;
	ncs.API_GetLogicalProcessorInformation = NULL;
	ncs.API_GetLargePageMinimum = NULL;
	ncs.API_GetActiveProcessorCount = NULL;
	ncs.API_GetActiveProcessorGroupCount = NULL;
	ncs.API_GetNumaNodeProcessorMaskEx = NULL;
	ncs.API_SetThreadGroupAffinity = NULL;
	HMODULE hmod = NULL;
	hmod = GetModuleHandle( ncsLibrary );
    if ( hmod != NULL )
    {
		ncs.API_GlobalMemoryStatusEx = ( BOOL(*)( LPMEMORYSTATUSEX ) )
			GetProcAddress ( hmod, "GlobalMemoryStatusEx" );
		ncs.API_GetNumaHighestNodeNumber = ( BOOL(*)( PULONG ) )
			GetProcAddress ( hmod, "GetNumaHighestNodeNumber" ); 
		ncs.API_GetNumaNodeProcessorMask = ( BOOL(*)( UCHAR, PULONGLONG ) )
			GetProcAddress ( hmod, "GetNumaNodeProcessorMask" ); 
		ncs.API_VirtualAllocExNuma = ( LPVOID(*)( HANDLE, LPVOID, SIZE_T, DWORD, DWORD, DWORD ) )
			GetProcAddress ( hmod, "VirtualAllocExNuma" ); 
		ncs.API_SetThreadAffinityMask = ( DWORD_PTR(*)( HANDLE, DWORD_PTR ) )
			GetProcAddress ( hmod, "SetThreadAffinityMask" ); 
		ncs.API_GetSystemFirmwareTable = ( UINT(*)( DWORD, DWORD, PVOID, DWORD ) )
			GetProcAddress ( hmod, "GetSystemFirmwareTable" ); 
		ncs.API_GetLogicalProcessorInformation = ( BOOL(*)( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD ) )
			GetProcAddress ( hmod, "GetLogicalProcessorInformation" ); 
		ncs.API_GetLargePageMinimum = ( SIZE_T(*)() )
			GetProcAddress ( hmod, "GetLargePageMinimum" ); 
		ncs.API_GetActiveProcessorCount = ( DWORD(*)( WORD ) )
			GetProcAddress ( hmod, "GetActiveProcessorCount" ); 
		ncs.API_GetActiveProcessorGroupCount = ( WORD(*)() )
			GetProcAddress ( hmod, "GetActiveProcessorGroupCount" ); 
		ncs.API_GetNumaNodeProcessorMaskEx = ( BOOL(*)( USHORT, PGROUP_AFFINITY ) )
			GetProcAddress ( hmod, "GetNumaNodeProcessorMaskEx" ); 
		ncs.API_SetThreadGroupAffinity = ( BOOL(*)( HANDLE, GROUP_AFFINITY, PGROUP_AFFINITY ) )
			GetProcAddress ( hmod, "SetThreadGroupAffinity" ); 
	}
}

// Get list of functions pointers, used for NUMA, loaded by dynamical import
NUMA_CONTROL_SET* NumaTopology::getControlSet( )
{
	return &ncs;
}

// Scan platform configuration: build ;ist of NUMA nodes
int NumaTopology::buildNodesList( NUMA_NODE_ENTRY* xp )
{
	// Pre-blank nodes list
	int i = 0;
	for( i=0; i<MAXIMUM_NODES; i++ )
	{
		xp->nodeId = 0;
		xp->baseAtNode = 0;
		xp->sizeAtNode = 0;
		xp->trueBaseAtNode = 0;
		xp->nodeGaff.Mask = 0;
		xp->nodeGaff.Group = 0;
	}
	
	// Get maximum node number, 0-based
	ULONG maxNode = -1;
	BOOL status = FALSE;
	if ( ( ncs.API_GetNumaHighestNodeNumber == NULL ) ||
	     ( ncs.API_GetNumaNodeProcessorMask == NULL )  )
		{
	    return 0; 	
		}
	status = ( ncs.API_GetNumaHighestNodeNumber ) ( &maxNode );
	if ( !status ) return 0;
	
	// Initializing variables and select method for build node list with affinity mask
	maxNode++;
	int activeNodes = 0;
	if ( ncs.API_GetNumaNodeProcessorMaskEx != NULL )
	{
		// Build node list with affinity mask, variant WITH Processor Group
		for( i=0; i<maxNode; i++ )
		{
			status = ( ncs.API_GetNumaNodeProcessorMask ) ( i, &( xp->nodeGaff.Mask ) );
			if ( status )
			{
				xp->nodeId = i;
				xp++;
				activeNodes++;
			}
		}
	}
	else if ( ncs.API_GetNumaNodeProcessorMask != NULL )
	{
		// Build node list with affinity mask, variant WITHOUT Processor Group
		for( i=0; i<maxNode; i++ )
		{
			status = ( ncs.API_GetNumaNodeProcessorMaskEx ) ( i, &( xp->nodeGaff ) );
			if ( status )
			{
				xp->nodeId = i;
				xp++;
				activeNodes++;
			}
		}
	}
	else
	{
		return 0;
	}
	// Return nodes count
	return activeNodes;
}

// Allocate memory to NUMA nodes, enumerated by existed list
BOOL NumaTopology::allocateNodesList( size_t xs, int n, NUMA_NODE_ENTRY* xp )
{
	// Initializing
	DWORD allocType = MEM_RESERVE + MEM_COMMIT;
	int i = 0;
	// Get Process Handle
	HANDLE h = GetCurrentProcess( );
	if ( h == NULL ) return FALSE;
	// Allocate memory for NUMA nodes
	if ( ncs.API_VirtualAllocExNuma != NULL )
	{
		for( i=0; i<n; i++ )
		{
			LPVOID base = ( ncs.API_VirtualAllocExNuma ) ( h, NULL, xs, allocType, PAGE_READWRITE, xp->nodeId );
			xp->baseAtNode = base;
			xp->sizeAtNode = xs;
			xp->trueBaseAtNode = base;
			if ( base == NULL ) return FALSE;
			xp++;
		}
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

// Release memory allocated at NUMA nodes, enumerated by existed list
BOOL NumaTopology::freeNodesList( int n, NUMA_NODE_ENTRY* xp )
{
	// Initializing
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
		xp++;
	}
	return status;
}

// Allocate memory for threads by NUMA-allocation results
BOOL NumaTopology::allocateThreadsList
	( int nNodes, int nThreads, NUMA_NODE_ENTRY* pNodes, THREAD_CONTROL_ENTRY* pThreads )
{
	// Verify input parameters
	if ( ( nNodes <=0 ) || ( nThreads <= 0 ) ) return FALSE;
	if ( nThreads % nNodes != 0 ) return FALSE;
	int threadsPerNode = nThreads / nNodes;
	if ( threadsPerNode <= 0 ) return FALSE;
	
	// Initializing
	int i = 0;
	int j = 0;
	
	// Cycle for nodes
	for( i=0; i<nNodes; i++ )
	{
		LPBYTE base = ( LPBYTE )( pNodes->baseAtNode );
		SIZE_T size = pNodes->sizeAtNode / threadsPerNode;
		SIZE_T halfSize = size / 2;
		// Cycle for threads, associated with this node
		for( j=0; j<threadsPerNode; j++ )
		{
			pThreads->base1 = base;
			pThreads->base2 = base + halfSize;
			base += size;
			pThreads++;
		}
		pNodes++;	
	}
	return TRUE;	
}

// Method returns status string, valid if error returned,
// YET RESERVED, NOT USED
char* NumaTopology::getStatusString( )
{
    return s;
}

