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

Class for Unified Scenario Model:
parent class for benchmarks and diagnostics scenarios in this application.
TODO.

*/

#include "USM.h"

USM::USM(int op, SYSTEM_CONTROL_SET* cs, COMMAND_LINE_PARMS* pp) :
	enumOp(op), controlSet(cs), pCmd(pp)
{
}
USM::~USM()
{
	// ... destructor functionality yet reserved ...
}
void USM::execute()
{
	AppLib::writeColor("USM class must be inherited.", APPCONST::ERROR_COLOR);
}