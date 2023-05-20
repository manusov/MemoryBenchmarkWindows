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

Class realization for system information scenario.
TODO.

*/

#include "SysInfo.h"

SysInfo::SysInfo(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp) :
	USM(op, cs, pp)
{
}
SysInfo::~SysInfo()
{
	// ... destructor functionality yet reserved ...
}
void SysInfo::execute()
{
	// support key: "info=cpu"
	if ((enumOp == INFO_CPU) || (enumOp == INFO_ALL))
	{
		AppLib::writeColor("\r\n[Processor information]\r\n\r\n", APPCONST::GROUP_COLOR);
		if (controlSet->pProcessorDetector)
		{
			controlSet->pProcessorDetector->writeReport();
		}
	}
	// support key: "info=topology"
	if ((enumOp == INFO_TOPOLOGY) || (enumOp == INFO_ALL))
	{
		AppLib::writeColor("\r\n[Platform topology and cache information]\r\n\r\n", APPCONST::GROUP_COLOR);
		if (controlSet->pTopologyDetector)
		{
			controlSet->pTopologyDetector->writeStandardTopology();
		}
	}
	// support key: "info=topologyex"
	if ((enumOp == INFO_TOPOLOGY_EX) || (enumOp == INFO_ALL))
	{
		AppLib::writeColor("\r\n[Platform topology and cache information, extended]\r\n\r\n", APPCONST::GROUP_COLOR);
		if (controlSet->pTopologyDetector)
		{
			controlSet->pTopologyDetector->writeExtendedTopology();
		}
	}
	// support key: "info=memory"
	if ((enumOp == INFO_MEMORY) || (enumOp == INFO_ALL))
	{
		AppLib::writeColor("\r\n[Memory information]\r\n\r\n", APPCONST::GROUP_COLOR);
		if (controlSet->pMemoryDetector)
		{
			controlSet->pMemoryDetector->writeReport();
		}
	}
	// support key: "info=paging"
	if ((enumOp == INFO_PAGING) || (enumOp == INFO_ALL))
	{
		AppLib::writeColor("\r\n[Paging information]\r\n\r\n", APPCONST::GROUP_COLOR);
		if (controlSet->pPagingDetector)
		{
			controlSet->pPagingDetector->writeReport();
		}

	}
	// support key: "info=api"
	if ((enumOp == INFO_API) || (enumOp == INFO_ALL))
	{
		AppLib::writeColor("\r\n[OS API and FASM DLL information]\r\n\r\n", APPCONST::GROUP_COLOR);
		if (controlSet->pFunctionsLoader)
		{
			controlSet->pFunctionsLoader->writeReport();
		}
	}
	// support key: "info=domains"
	if ((enumOp == INFO_DOMAINS) || (enumOp == INFO_ALL))
	{
		AppLib::writeColor("\r\n[NUMA domains information]\r\n\r\n", APPCONST::GROUP_COLOR);
		if (controlSet->pDomainsBuilder)
		{
			controlSet->pDomainsBuilder->writeReport();
		}
	}
	// support key: "info=threads"
	if ((enumOp == INFO_THREADS) || (enumOp == INFO_ALL))
	{
		AppLib::writeColor("\r\n[Platform topology information]\r\n\r\n", APPCONST::GROUP_COLOR);
		if (controlSet->pThreadsBuilder)
		{
			controlSet->pThreadsBuilder->writeReport();
		}
	}



/*
	// support key: "info=topology"
	if ((enumOp == INFO_TOPOLOGY) || (enumOp == INFO_ALL))
	{
		AppConsole::transmit("[ --- MP TOPOLOGY AND CACHE INFORMATION --- ]\n\n");
		dst = saveDst;
		max = saveMax;
		controlSet->pTopologyDetector->getTopologyText(dst, max);
		*dst = 0;  // terminator must be char = 0
		AppConsole::transmit(saveDst);
		AppConsole::transmit("\n");
	}
	// support key: "info=topologyex"
	if ((enumOp == INFO_TOPOLOGY_EX) || (enumOp == INFO_ALL))
	{
		AppConsole::transmit("[ --- MP TOPOLOGY AND CACHE EXTENDED INFORMATION --- ]\n\n");
		dst = saveDst;
		max = saveMax;
		controlSet->pTopologyDetector->getTopologyTextEx(dst, max);
		*dst = 0;  // terminator must be char = 0
		AppConsole::transmit(saveDst);
		AppConsole::transmit("\n");
	}
	// support key: "info=memory"
	if ((enumOp == INFO_MEMORY) || (enumOp == INFO_ALL))
	{
		AppConsole::transmit("[ --- MEMORY INFORMATION --- ]\n\n");
		dst = saveDst;
		max = saveMax;
		controlSet->pMemoryDetector->getMemoryText(dst, max);
		*dst = 0;  // terminator must be char = 0
		AppConsole::transmit(saveDst);
		AppConsole::transmit("\n");
	}
	// support key: "info=paging"
	if ((enumOp == INFO_PAGING) || (enumOp == INFO_ALL))
	{
		AppConsole::transmit("[ --- PAGING INFORMATION --- ]\n\n");
		dst = saveDst;
		max = saveMax;
		controlSet->pPagingDetector->getPagingText(dst, max);
		*dst = 0;  // terminator must be char = 0		
		AppConsole::transmit(saveDst);
		AppConsole::transmit("\n");
	}
	// support key: "info=api"
	if ((enumOp == INFO_API) || (enumOp == INFO_ALL))
	{
		AppConsole::transmit("[ --- WINAPI AND APPLICATION LIBRARY INFORMATION --- ]\n\n");
		dst = saveDst;
		max = saveMax;
		controlSet->pFunctionsLoader->getFunctionsText(dst, max);
		*dst = 0;  // terminator must be char = 0
		AppConsole::transmit(saveDst);
		AppConsole::transmit("\n");
	}
	// support key: "info=domains"
	if ((enumOp == INFO_DOMAINS) || (enumOp == INFO_ALL))
	{
		AppConsole::transmit("[ --- NUMA DOMAINS INFORMATION --- ]\n\n");
		dst = saveDst;
		max = saveMax;
		controlSet->pDomainsBuilder->getNumaText(dst, max);
		*dst = 0;  // terminator must be char = 0
		AppConsole::transmit(saveDst);
		AppConsole::transmit("\n");
	}
	// support key: "info=threads"
	if ((enumOp == INFO_THREADS) || (enumOp == INFO_ALL))
	{
		AppConsole::transmit("[ --- THREADS INFORMATION --- ]\n\n");
		dst = saveDst;
		max = saveMax;
		controlSet->pThreadsBuilder->getThreadsText(dst, max);
		*dst = 0;  // terminator must be char = 0
		AppConsole::transmit(saveDst);
		AppConsole::transmit("\n");
	}

*/

}


