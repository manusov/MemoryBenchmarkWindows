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

Class realization for User help:
write user help information to console output.
TODO.

*/

#include "UserHelp.h"

const char* UserHelp::exampleString = "example: mpe64 test=memory memory=dram";

UserHelp::UserHelp(int op, const OPTION_ENTRY* oplist) : 
	USM (op, nullptr, nullptr), optionsList(oplist)
{
}
UserHelp::~UserHelp()
{
}
void UserHelp::execute()
{
	char msg[APPCONST::MAX_TEXT_STRING];
	//constexpr int MAX_OPTION_WIDTH = 9;
	//constexpr int LEFT_INTERVAL    = 11;
	int k = 0;
	const OPTION_ENTRY* p = optionsList;
	const char** p1 = nullptr;
	// This unconditionally write
	AppLib::writeColor("\r\noptions list:\r\n", APPCONST::TABLE_COLOR);
	while (p->name)
	{
		snprintf(msg, APPCONST::MAX_TEXT_STRING, "%s ", p->name);
		AppLib::writeColor(msg, APPCONST::VALUE_COLOR);
		p++;
	}
	AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
	// This write only if "help=full" mode
	if (enumOp == HELP_FULL)
	{
		AppLib::writeColor("\r\nvalues list:\r\n", APPCONST::TABLE_COLOR);
		p = optionsList;
		while (p->name != NULL)
		{
			snprintf(msg, APPCONST::MAX_TEXT_STRING, "%-9s= ", p->name);
			AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
			switch (p->routine)
			{
			case INTPARM:
				AppLib::writeColor("< integer value >", APPCONST::VALUE_COLOR);
				break;
			case MEMPARM:
				AppLib::writeColor("< value in memory size units, can use K/M/G, example \"4M\" >", APPCONST::VALUE_COLOR);
				break;
			case SELPARM:
				p1 = p->values;
				k = 1;
				while (*p1)
				{
					AppLib::writeColor(*p1, APPCONST::VALUE_COLOR);
					p1++;
					if ((*p1) && ((k % 5) != 0))
					{
						AppLib::writeColor(" | ", APPCONST::VALUE_COLOR);
					}
					if ((*p1) && ((k % 5) == 0))
					{
						AppLib::writeColor("\r\n           ", APPCONST::VALUE_COLOR);
					}
					k++;
				}
				break;
			case STRPARM:
				AppLib::writeColor("< text string >", APPCONST::VALUE_COLOR);
				break;
			default:
				break;
			}
			p++;
			AppLib::writeColor("\r\n", APPCONST::VALUE_COLOR);
		}
	}
	snprintf(msg, APPCONST::MAX_TEXT_STRING, "\r\n%s\r\n", exampleString);
	AppLib::writeColor(msg, APPCONST::TABLE_COLOR);
}
