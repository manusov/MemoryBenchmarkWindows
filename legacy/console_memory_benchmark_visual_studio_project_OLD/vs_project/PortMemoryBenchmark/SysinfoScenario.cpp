/*
				  MEMORY PERFORMANCE ENGINE FRAMEWORK.
				   System Information Scenario class.
	This class make sequence of different components detectors routines.
*/

#include "SysinfoScenario.h"

char* SysinfoScenario::saveDst;
size_t SysinfoScenario::saveMax;
int SysinfoScenario::enumOp;
SYSTEM_CONTROL_SET* SysinfoScenario::controlSet;

SysinfoScenario::SysinfoScenario(char* pointer, size_t limit, int op, SYSTEM_CONTROL_SET* cs)
{
	saveDst = pointer;
	saveMax = limit;
	enumOp = op;
	controlSet = cs;
}

SysinfoScenario::~SysinfoScenario()
{
	// ... destructor functionality yet reserved ...
}

void SysinfoScenario::execute()
{
	char* dst = NULL;
	size_t max = 0;
	AppConsole::transmit("\n");
	// support key: "info=cpu"
	if ((enumOp == INFO_CPU) || (enumOp == INFO_ALL))
	{
		AppConsole::transmit("[ --- PROCESSOR INFORMATION --- ]\n\n");
		dst = saveDst;
		max = saveMax;
		controlSet->pProcessorDetector->getProcessorText(dst, max);
		*dst = 0;  // terminator must be char = 0
		AppConsole::transmit(saveDst);
		AppConsole::transmit("\n");
	}
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
}
