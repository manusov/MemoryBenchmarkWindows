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

Class realization for Unified Driver Model:
parent class for components drivers in this application.
TODO.

*/

#include "UDM.h"

UDM::UDM() : status(false), f(nullptr)
{
	
}
UDM::~UDM()
{
	
}
BOOL UDM::getStatus()
{
	return status;
}
const char* const UDM::getStatusString()
{
	return statusString;
}
void UDM::writeReport()
{
	AppLib::write(msgError);
}
FUNCTIONS_LIST* UDM::getFunctionsList()
{
	return f;
}

char UDM::statusString[APPCONST::MAX_TEXT_STRING];
const char* const UDM::msgError = "Unified Driver Model class must be inherited.";

