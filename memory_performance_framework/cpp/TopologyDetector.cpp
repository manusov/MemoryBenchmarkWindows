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

Class realization for topology detector,
detects system topology include caches levels.
TODO.
1) Optimize series of snprintf as single snprintf.
2) Verify at complex NUMA/Processor groups topologies.

*/

#include "TopologyDetector.h"

// Pointers to standard and extended topology buffer (dynamically allocated buffers).
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION    TopologyDetector::pTopologyBuffer = nullptr;
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX TopologyDetector::pTopologyBufferEx = nullptr;
// Limits pointers for standard and extended topology buffer (dynamically allocated buffers).
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION    TopologyDetector::pTopologyLimit = nullptr;
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX TopologyDetector::pTopologyLimitEx = nullptr;
// Sizes of allocated buffers for standard and extended topology data.
DWORD TopologyDetector::topologySize = 0;
DWORD TopologyDetector::topologySizeEx = 0;
// Topology summary report data, standard and extended versions.
SYSTEM_TOPOLOGY_DATA TopologyDetector::topologyData;
SYSTEM_TOPOLOGY_DATA TopologyDetector::topologyDataEx;
// Cache types names strings
const char* TopologyDetector::cacheTypes[] =
{
	"unified",
	"instruction",
	"data",
	"trace"
};

TopologyDetector::TopologyDetector(FUNCTIONS_LIST* pf) : UDM(pf)
{
	// Blank status string.
	snprintf(statusString, APPCONST::MAX_TEXT_STRING, "No data.");
	// Global initialization.
	blankTopologyData(&topologyData);
	blankTopologyData(&topologyDataEx);
	pTopologyBuffer = nullptr;
	pTopologyBufferEx = nullptr;
	pTopologyLimit = nullptr;
	pTopologyLimitEx = nullptr;
	topologySize = 0;
	topologySizeEx = 0;
	// local initialization, for standard version
	status = FALSE;
	BOOL done = FALSE;
	DWORD error = 0;
	// Get topology information into buffer, note required memory allocation for buffer
	// Standard version
	if (f->api_GetLogicalProcessorInformation)
	{
		while (!done)
		{
			status = (f->api_GetLogicalProcessorInformation)(pTopologyBuffer, &topologySize);
			if (!status)
			{
				error = GetLastError();
				if (error == ERROR_INSUFFICIENT_BUFFER)
				{
					if (pTopologyBuffer)
					{
						free(pTopologyBuffer);
					}
					pTopologyBuffer = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc(topologySize);
					if (pTopologyBuffer == NULL)
					{
						error = GetLastError();
						snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Allocate buffer for logical processors descriptors (standard).");
						done = TRUE;  // stop if still error after memory allocation
					}
					else
					{
						done = FALSE;  // make next iteration if memory allocated OK, second try get topology info
					}
				}
				else
				{
					snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Get logical processors information (standard).");
					done = TRUE;  // stop if other error type, not insufficient memory
				}
			}
			else
			{
				done = TRUE;  // stop if first call success (this impossible because size=0 at first call)
			}
		}
	}
	pTopologyLimit = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)((LPBYTE)pTopologyBuffer + topologySize);
	// local re-initialization, for extended version
	status = FALSE;
	done = FALSE;
	error = 0;
	// Get topology information into buffer, note required memory allocation for buffer
	// Extended version
	if (f->api_GetLogicalProcessorInformationEx)
	{
		while (!done)
		{
			status = (f->api_GetLogicalProcessorInformationEx)(RelationAll, pTopologyBufferEx, &topologySizeEx);
			if (!status)
			{
				error = GetLastError();
				if (error == ERROR_INSUFFICIENT_BUFFER)
				{
					if (pTopologyBufferEx)
					{
						free(pTopologyBufferEx);
					}
					pTopologyBufferEx = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)malloc(topologySizeEx);
					if (pTopologyBufferEx == NULL)
					{
						error = GetLastError();
						snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Allocate buffer for logical processors descriptors (standard).");
						done = TRUE;  // stop if still error after memory allocation
					}
					else
					{
						done = FALSE;  // make next iteration if memory allocated OK, second try get topology info
					}
				}
				else
				{
					snprintf(statusString, APPCONST::MAX_TEXT_STRING, "Get logical processors information (standard).");
					done = TRUE;  // stop if other error type, not insufficient memory
				}
			}
			else
			{
				done = TRUE;  // stop if first call success (this impossible because size=0 at first call)
			}
		}
	}
	pTopologyLimitEx = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((LPBYTE)pTopologyBufferEx + topologySizeEx);
	// Decode topology information, argument = WinAPI returned buffer, result = summary data
	// Standard version
	PCACHE_DESCRIPTOR cache;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION p1 = pTopologyBuffer;
	SYSTEM_TOPOLOGY_DATA* p2 = &topologyData;
	// Cycle for entries
	while (p1 < pTopologyLimit)
	{
		switch (p1->Relationship)
		{
		case RelationCache:
			cache = &p1->Cache;
			if ((cache->Type == CacheUnified) || (cache->Type == CacheData))
			{
				switch (cache->Level)
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
			if ((p1->ProcessorCore.Flags) != 0)
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
	while (p3 < pTopologyLimitEx)
	{
		switch (p3->Relationship)
		{
		case RelationCache:
			cacheEx = &p3->Cache;
			if ((cacheEx->Type == CacheUnified) || (cacheEx->Type == CacheData))
			{
				switch (cacheEx->Level)
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
			if ((p3->Processor.Flags) != 0)
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
		p3 = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((LPBYTE)p3 + p3->Size);
	}
}
TopologyDetector::~TopologyDetector()
{
	if (pTopologyBuffer)
	{
		free(pTopologyBuffer);
	}
	if (pTopologyBufferEx)
	{
		free(pTopologyBufferEx);
	}
}
void TopologyDetector::writeReport()
{
	AppLib::writeColor("Standard topology info.\r\n", APPCONST::TABLE_COLOR);
	writeStandardTopology();
	AppLib::writeColor("\r\nExtended topology info.\r\n", APPCONST::TABLE_COLOR);
	writeExtendedTopology();

}
// Return pointer to topology data, result of standard function
SYSTEM_TOPOLOGY_DATA* TopologyDetector::getTopologyList()
{
	return &topologyData;
}
// Return pointer to topology data, result of extended function
SYSTEM_TOPOLOGY_DATA* TopologyDetector::getTopologyListEx()
{
	return &topologyDataEx;
}
// Get text report by standard topology information.
void TopologyDetector::writeStandardTopology()
{
	char msg[APPCONST::MAX_TEXT_STRING];
	// Table 1 of 5 = cores.
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pbuf = pTopologyBuffer;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION pmax = pTopologyLimit;
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	AppLib::writeColor(" core  affinity mask (hex)   ht   logical processors\r\n", APPCONST::TABLE_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	int i = 0;
	while (pbuf < pmax)
	{
		if (pbuf->Relationship == RelationProcessorCore)
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%2d     ", i);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, pbuf->ProcessorMask, FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "      %d    ", pbuf->ProcessorCore.Flags);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::storeBitsList(msg, APPCONST::MAX_TEXT_STRING, pbuf->ProcessorMask);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
			i++;
		}
		pbuf++;
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	// Table 2 of 5 = caches.
	PCACHE_DESCRIPTOR cache;
	pbuf = pTopologyBuffer;
	pmax = pTopologyLimit;
	AppLib::writeColor(" cache  type          size (KB)    associativity   line     logical processors\r\n", APPCONST::TABLE_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	while (pbuf < pmax)
	{
		if (pbuf->Relationship == RelationCache)
		{
			// Level.
			AppLib::writeColor(" L", APPCONST::VALUE_COLOR);
			cache = &pbuf->Cache;
			int k = cache->Level;
			if ((k > 0) && (k < 5))
			{
				snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-6d", k);
				AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			}
			else
			{
				AppLib::writeColor("?     ", APPCONST::VALUE_COLOR);
			}
			// Type.
			k = cache->Type;
			const char* s;
			if ((k >= 0) && (k <= 3))
			{
				s = cacheTypes[k];
			}
			else
			{
				s = "unknown";
			}
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-14s", cacheTypes[k]);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			// Size.
			int sizeKB = (cache->Size) / 1024;
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-13d", sizeKB);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			// Associativity.
			int associativity = cache->Associativity;
			if (associativity == CACHE_FULLY_ASSOCIATIVE)
			{
				snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-14s  ", "full");
				AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			}
			else
			{
				snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-14d  ", associativity);
				AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			}
			// Line size.
			int lineSize = cache->LineSize;
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-9d", lineSize);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::storeBitsList(msg, APPCONST::MAX_TEXT_STRING, pbuf->ProcessorMask);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
		}
		pbuf++;
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	// Table 3 of 5 = NUMA domains
	pbuf = pTopologyBuffer;
	pmax = pTopologyLimit;
	AppLib::writeColor(" domain   affinity mask (hex)   logical processors\r\n", APPCONST::TABLE_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	i = 0;
	while (pbuf < pmax)
	{
		if (pbuf->Relationship == RelationNumaNode)
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, " %-9d", i);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, pbuf->ProcessorMask, FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("      ", APPCONST::VALUE_COLOR);
			AppLib::storeBitsList(msg, APPCONST::MAX_TEXT_STRING, pbuf->ProcessorMask);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
			i++;
		}
		pbuf++;
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	// Table 4 of 5 = processor packages
	pbuf = pTopologyBuffer;
	pmax = pTopologyLimit;
	AppLib::writeColor(" package   affinity mask (hex)   logical processors\r\n", APPCONST::PARM_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	i = 0;
	while (pbuf < pmax)
	{
		if (pbuf->Relationship == RelationProcessorPackage)
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, " %-10d", i);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, pbuf->ProcessorMask, FALSE);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("      ", APPCONST::VALUE_COLOR);
			AppLib::storeBitsList(msg, APPCONST::MAX_TEXT_STRING, pbuf->ProcessorMask);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
			i++;
		}
		pbuf++;
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	// Table 5 of 5 = summary.
	// This table yet reserved.
}
typedef struct PROCESSOR_RELATIONHIP_REDEFINE {
	BYTE Flags;
	BYTE EfficiencyClass;
	BYTE Reserved[20];
	WORD GroupCount;
	GROUP_AFFINITY GroupMask[ANYSIZE_ARRAY];
} PROCESSOR_RELATIONHIP_REDEFINE, *PPROCESSOR_RELATIONHIP_REDEFINE;
// Get text report by extended topology information.
void TopologyDetector::writeExtendedTopology()
{
	char msg[APPCONST::MAX_TEXT_STRING];
	// Table 1 of 6 = cores
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pbuf = pTopologyBufferEx;
	PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX pmax = pTopologyLimitEx;
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	AppLib::writeColor(" core   ht   efficiency   group and affinity mask (hex)\r\n", APPCONST::TABLE_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	int i = 0;
	while (pbuf < pmax)
	{
		if (pbuf->Relationship == RelationProcessorCore)
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%2d      ", i);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			PPROCESSOR_RELATIONHIP_REDEFINE px = (PPROCESSOR_RELATIONHIP_REDEFINE)(&pbuf->Processor);
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%d    ", px->Flags);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%d            ", px->EfficiencyClass);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			printGroups(26, px->GroupMask, px->GroupCount);
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
			i++;
		}
		pbuf = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((LPBYTE)pbuf + pbuf->Size);
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	// Table 2 of 6 = caches
	pbuf = pTopologyBufferEx;
	pmax = pTopologyLimitEx;
	PCACHE_RELATIONSHIP cacheEx;
	AppLib::writeColor(" cache  type          size (KB)    group and affinity mask (hex)\r\n", APPCONST::TABLE_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	while (pbuf < pmax)
	{
		if (pbuf->Relationship == RelationCache)
		{
			// Level.
			AppLib::writeColor(" L", APPCONST::VALUE_COLOR);
			cacheEx = &pbuf->Cache;
			int k = cacheEx->Level;
			if ((k > 0) && (k < 5))
			{
				snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-6d", k);
				AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			}
			else
			{
				AppLib::writeColor("?     ", APPCONST::VALUE_COLOR);
			}
			// Type.
			k = cacheEx->Type;
			const char* s;
			if ((k >= 0) && (k <= 3))
			{
				s = cacheTypes[k];
			}
			else
			{
				s = "unknown";
			}
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-14s", cacheTypes[k]);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			// Size.
			int sizeKB = (cacheEx->CacheSize) / 1024;
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-13d", sizeKB);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			printGroups(36, &cacheEx->GroupMask, 1);
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
		}
		pbuf = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((LPBYTE)pbuf + pbuf->Size);
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	// Table 3 of 6 = NUMA domains
	pbuf = pTopologyBufferEx;
	pmax = pTopologyLimitEx;
	PNUMA_NODE_RELATIONSHIP numaEx;
	AppLib::writeColor(" domain   group and affinity mask (hex)\r\n", APPCONST::TABLE_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	i = 0;
	while (pbuf < pmax)
	{
		if (pbuf->Relationship == RelationNumaNode)
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, " %-9d", i);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			numaEx = &pbuf->NumaNode;
			printGroups(36, &numaEx->GroupMask, 1);
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
			i++;
		}
		pbuf = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((LPBYTE)pbuf + pbuf->Size);
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	// Table 4 of 6 = processor packages
	// Not supported, see Processor Relationship
	// Table 5 of 6 = processor groups
	pbuf = pTopologyBufferEx;
	pmax = pTopologyLimitEx;
	PGROUP_RELATIONSHIP groupEx;
	AppLib::writeColor(" group   maximum   active   maximum   active   affinity mask (hex)\r\n", APPCONST::TABLE_COLOR);
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	i = 0;
	while (pbuf < pmax)
	{
		if (pbuf->Relationship == RelationGroup)
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, " %-8d", i);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			groupEx = &pbuf->Group;
			int maxGroups = groupEx->MaximumGroupCount;
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-10d", maxGroups);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			int activeGroups = groupEx->ActiveGroupCount;
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-9d", activeGroups);
			AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
			PPROCESSOR_GROUP_INFO pGinfo = groupEx->GroupInfo;
			for (int j = 0; j < activeGroups; j++)
			{
				if (j > 0)
				{
					AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
					for (int i = 0; i < 28; i++)
					{
						AppLib::writeColor(" ", APPCONST::VALUE_COLOR);
					}
				}
				snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-10d", pGinfo->MaximumProcessorCount);
				AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
				snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-9d", pGinfo->ActiveProcessorCount);
				AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
				AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, pGinfo->ActiveProcessorMask, FALSE);
				AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
				pGinfo++;
			}

			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
			i++;
		}
		pbuf = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX)((LPBYTE)pbuf + pbuf->Size);
	}
	AppLib::writeColorLine(APPCONST::TABLE_WIDTH, APPCONST::TABLE_COLOR);
	// AppLib::printCrLf( dst, max );
	// Table 6 of 6 = summary
	// This table yet reserved
}

// Method get brief information about cache levels.
void TopologyDetector::writeCacheInfo()
{
	// ... reserved ...
}

// Helpers.
// Helper function for constructor, blank topology data.
void TopologyDetector::blankTopologyData(SYSTEM_TOPOLOGY_DATA* p)
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
// Helper method: print sequence of group affinity structures
void TopologyDetector::printGroups(int skipLeft, PGROUP_AFFINITY pGaff, int groupCount)
{
	char msg[APPCONST::MAX_TEXT_STRING];
	for (int j = 0; j < groupCount; j++)
	{
		if (j > 0)
		{
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
			for (int i = 0; i < skipLeft; i++)
			{
				AppLib::writeColor(" ", APPCONST::VALUE_COLOR);
			}
		}
		AppLib::storeHex16(msg, APPCONST::MAX_TEXT_STRING, pGaff->Group, FALSE);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		AppLib::writeColor("\\", APPCONST::VALUE_COLOR);
		AppLib::storeHex64(msg, APPCONST::MAX_TEXT_STRING, pGaff->Mask, FALSE);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		pGaff++;
	}

}
