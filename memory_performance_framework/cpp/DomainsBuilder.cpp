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

Class realization for NUMA domains support:
NUMA-aware memory allocation.
TODO.

*/

#include "DomainsBuilder.h"

DomainsBuilder::DomainsBuilder(FUNCTIONS_LIST* pf)
{
	f = pf;
	numaData.nodeCount = 0;
	numaData.nodeList = nullptr;
	simpleData.nodeCount = 1;
	simpleData.nodeList = &simpleEntry;
	helperBlankNode(&simpleEntry);
	DWORD updateCount = 0;
	if (f)
	{
		// allocate memory for NUMA nodes list
		int n = APPCONST::MAXIMUM_NODES;
		int mN = sizeof(NUMA_NODE_ENTRY) * n;
		NUMA_NODE_ENTRY* pN = (NUMA_NODE_ENTRY*)malloc(mN);
		numaData.nodeList = pN;
		// build list of domains, if memory allocated without errors
		if (pN)
		{
			// Pre-blank allocated list
			for (int i = 0; i < n; i++)
			{
				helperBlankNode(pN);
				pN++;
			}
			// Detect maximum number of NUMA nodes
			if (f->api_GetNumaHighestNodeNumber)
			{
				ULONG nc = 0;
				BOOL numaStatus = (f->api_GetNumaHighestNodeNumber) (&nc);
				if (numaStatus)
				{
					numaData.nodeCount = nc + 1;  // nodes count = last node number + 1
					// Reload pointer after use it, select method for build list of nodes
					pN = numaData.nodeList;
					if (f->api_GetNumaNodeProcessorMaskEx)
					{	// Build list of nodes, branch WITH support Processor Groups
						for (int i = 0; i < APPCONST::MAXIMUM_NODES; i++)
						{
							BOOL nodeStatus = (f->api_GetNumaNodeProcessorMaskEx) (i, &pN->nodeGaff);
							if ((nodeStatus) && (pN->nodeGaff.Mask != 0))
							{
								pN->nodeId = i;
								updateCount++;
								pN++;
							}
							else
							{
								helperBlankNode(pN);
							}
						}
					}
					else if (f->api_GetNumaNodeProcessorMask)
					{	// Build list of nodes, branch WITHOUT support Processor Groups

						for (int i = 0; i < APPCONST::MAXIMUM_NODES; i++)
						{
							BOOL nodeStatus = (f->api_GetNumaNodeProcessorMask) (i, (PULONGLONG)(&pN->nodeGaff.Mask));
							if ((nodeStatus) && (pN->nodeGaff.Mask != 0))
							{
								pN->nodeId = i;
								updateCount++;
								pN++;
							}
							else
							{
								helperBlankNode(pN);
							}
						}
					}
				}
			}
		}
		numaData.nodeCount = updateCount;
	}
	if ((numaData.nodeCount) && (numaData.nodeList))
	{
		snprintf(statusString, APPCONST::MAX_TEXT_STRING, "No data.");
		status = TRUE;
	}
	else
	{
		snprintf(statusString, APPCONST::MAX_TEXT_STRING, "NUMA topology detection failed.");
		status = FALSE;
	}
}
DomainsBuilder::~DomainsBuilder()
{
	if (numaData.nodeList)
	{
		free(numaData.nodeList);
	}
}
// Return pointer to domains topology data, NUMA-aware version.
SYSTEM_NUMA_DATA* DomainsBuilder::getNumaList()
{
	return &numaData;
}
// Return pointer to domains topology data, NUMA-unaware version.
SYSTEM_NUMA_DATA* DomainsBuilder::getSimpleList()
{
	return &simpleData;
}
//Write text report about NUMA nodes.
void DomainsBuilder::writeReportNuma(BOOL line)
{
	char msg[APPCONST::MAX_TEXT_STRING];
	// Detected nodes count and allocated memory base address
	snprintf(msg, APPCONST::MAX_TEXT_STRING,
		"NUMA nodes detected = %d, list allocated at base = ", 
		numaData.nodeCount);
	if (line) AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	AppLib::storeHexPointer(msg, APPCONST::MAX_TEXT_STRING, numaData.nodeList, true);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	AppLib::writeColor(".\r\n", APPCONST::TABLE_COLOR);
	// Nodes table.
	NUMA_NODE_ENTRY* pN = numaData.nodeList;
	if ((numaData.nodeCount > 0) && (numaData.nodeList))
	{
		AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
		AppLib::writeColor(
			" ID  affinity group\\mask    allocated base    allocated size    original (hex)\r\n",
			APPCONST::TABLE_COLOR);
		AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
		// Cycle for NUMA nodes.
		for (unsigned int i = 0; i < numaData.nodeCount; i++)
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, " %2d  ", i);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::storeHex16(msg, APPCONST::MAX_TEXT_STRING, pN->nodeGaff.Group, false);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("\\", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, pN->nodeGaff.Mask, false);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("  ", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)pN->baseAtNode, false);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("  ", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)pN->sizeAtNode, false);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("  ", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)pN->trueBaseAtNode, false);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
			pN++;
		}
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
}
// Write text report about simple (NUMA-unaware) topology.
void DomainsBuilder::writeReportSimple(BOOL line)
{
	char msg[APPCONST::MAX_TEXT_STRING];
	// Detected nodes count and allocated memory base address
	snprintf(msg, APPCONST::MAX_TEXT_STRING,
		"NUMA-unaware mode, list allocated at base = ");
	if (line) AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	AppLib::storeHexPointer(msg, APPCONST::MAX_TEXT_STRING, simpleData.nodeList, true);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
	AppLib::writeColor(".\r\n", APPCONST::TABLE_COLOR);
	// Nodes table.
	NUMA_NODE_ENTRY* pN = simpleData.nodeList;
	if ((simpleData.nodeCount > 0) && (simpleData.nodeList))
	{
		AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
		AppLib::writeColor(
			" ID  allocated base    allocated size    original (all hex)\r\n",
			APPCONST::TABLE_COLOR);
		AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
		// Cycle for memory blocks
		for (unsigned int i = 0; i < simpleData.nodeCount; i++)
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, " %2d  ", i);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)pN->baseAtNode, false);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("  ", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)pN->sizeAtNode, false);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("  ", APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, (DWORD64)pN->trueBaseAtNode, false);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("\r\n", APPCONST::MAX_TEXT_STRING);
			pN++;
		}
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
}
void DomainsBuilder::writeReport()
{
	writeReportNuma();
	AppLib::write("\r\n");
	writeReportSimple();
}
// Memory allocation methods, NUMA-aware.
// Allocate memory for NUMA domains list.
BOOL DomainsBuilder::allocateNodesList(size_t memSize, DWORD pgMode, DWORD64 pgSize, BOOL swapFlag)
{
	// Get Process handle.
	HANDLE h = GetCurrentProcess();
	// Initializing base address and size.
	NUMA_NODE_ENTRY* xp = numaData.nodeList;
	int n = numaData.nodeCount;
	BOOL status = (h && xp && n);
	if(status)
	{
		size_t allocSize = (size_t)helperAlignByFactor(memSize, pgSize);  // alignment by selected page size
		DWORD allocType = MEM_RESERVE + MEM_COMMIT;
		if (pgMode == LP_USED)
		{
			allocType |= MEM_LARGE_PAGES;
		}
		// Allocate memory for NUMA nodes
		if (status && (f->api_VirtualAllocExNuma))
		{
			for (int i = 0; i < n; i++)
			{
				LPVOID base = (f->api_VirtualAllocExNuma) (h, nullptr, allocSize, allocType, PAGE_READWRITE, xp->nodeId);
				if (!base)
				{
					status = FALSE;
					break;
				}
				xp->baseAtNode = base;
				xp->sizeAtNode = allocSize;
				xp->trueBaseAtNode = base;
				xp++;
			}
			// Support domains swap mode for NUMA remote option
			xp = numaData.nodeList;
			if (status && swapFlag)
			{
				n--;
				for (int i = 0; i < n; i++)
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
		}
	}
	return status;
}
// Release memory, allocated for NUMA domains list.
BOOL DomainsBuilder::freeNodesList()
{
	// Get Process Handle.
	HANDLE h = GetCurrentProcess();
	// Initializing base address and size.
	NUMA_NODE_ENTRY* xp = numaData.nodeList;
	int n = numaData.nodeCount;
	BOOL status = (h && xp && n);
	if (status)
	{
		// Release memory for NUMA nodes.
		for (int i = 0; i < n; i++)
		{
			LPVOID trueBase = xp->trueBaseAtNode;
			if (trueBase)
			{
				if (!VirtualFreeEx(h, trueBase, 0, MEM_RELEASE))
				{
					status = FALSE;
				}
			}
			xp->baseAtNode = nullptr;
			xp->sizeAtNode = 0;
			xp->trueBaseAtNode = nullptr;
			xp++;
		}
	}
	return status;
}
// Memory allocation methods, NUMA-unaware, but same format for regularity
// Allocate memory for simple topology, NUMA-unaware mode.
BOOL DomainsBuilder::allocateSimpleList(size_t xs, DWORD pgMode, DWORD64 pgSize)
{
	NUMA_NODE_ENTRY* xp = simpleData.nodeList;
	BOOL status = (xp != nullptr);
	if (status)
	{
		size_t allocSize = (size_t)helperAlignByFactor(xs, pgSize);  // alignment by selected page size
		DWORD allocType = MEM_RESERVE + MEM_COMMIT;
		if (pgMode == LP_USED)
		{
			allocType |= MEM_LARGE_PAGES;
		}
		xp->trueBaseAtNode = VirtualAlloc(NULL, allocSize, allocType, PAGE_READWRITE);
		if (xp->trueBaseAtNode)
		{
			xp->baseAtNode = xp->trueBaseAtNode;
			xp->sizeAtNode = allocSize;
		}
		else
		{
			status = FALSE;
		}
	}
	return status;
}
// Release memory, allocated for simple topology.
BOOL DomainsBuilder::freeSimpleList()
{
	NUMA_NODE_ENTRY* xp = simpleData.nodeList;
	BOOL status = (xp != nullptr);
	if (status)
	{
		if (xp->trueBaseAtNode)
		{
			status = VirtualFree(xp->trueBaseAtNode, 0, MEM_RELEASE);
		}
		xp->trueBaseAtNode = nullptr;
		xp->baseAtNode = nullptr;
		xp->sizeAtNode = 0;
	}
	return status;
}

// Helper method, blank NUMA node entry
void DomainsBuilder::helperBlankNode(NUMA_NODE_ENTRY* p)
{
	p->nodeId = 0;
	p->nodeGaff.Mask = 0;
	p->nodeGaff.Group = 0;
	p->nodeGaff.Reserved[0] = 0;
	p->nodeGaff.Reserved[1] = 0;
	p->nodeGaff.Reserved[2] = 0;
	p->baseAtNode = nullptr;
	p->sizeAtNode = 0;
	p->trueBaseAtNode = nullptr;
}
// Helper method, block size alignment
DWORD64 DomainsBuilder::helperAlignByFactor(DWORD64 value, DWORD64 factor)
{
	DWORD64 x = value % factor;
	if (x)
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

// System NUMA topology summary report data
SYSTEM_NUMA_DATA DomainsBuilder::numaData;
// NUMA-style data for regular memory allocation mechanism, but NUMA-unaware.
SYSTEM_NUMA_DATA DomainsBuilder::simpleData;
NUMA_NODE_ENTRY DomainsBuilder::simpleEntry;
