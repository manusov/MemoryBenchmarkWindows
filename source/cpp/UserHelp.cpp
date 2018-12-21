#include "UserHelp.h"

char* UserHelp::saveDst;
size_t UserHelp::saveMax;
int UserHelp::enumOp;
const OPTION_ENTRY* UserHelp::optionsList;
const char* UserHelp::exampleString =
	"example: mpe64 test=memory memory=dram";

UserHelp::UserHelp( char* pointer, size_t limit, int op, const OPTION_ENTRY* oplist )
{
	saveDst = pointer;
	saveMax = limit;
	enumOp = op;
	optionsList = oplist;
}

UserHelp::~UserHelp( )
{
	
}

#define MAX_OPTION_WIDTH 9
#define LEFT_INTERVAL 11

void UserHelp::execute( )
{
	char* dst;
	size_t max;
	int k = 0;
	const OPTION_ENTRY* p = optionsList;
	const char** p1 = NULL;
	// This unconditionally write
	AppConsole::transmit( "\r\noptions list:\r\n" );
	while ( p->name != NULL )
	{
		snprintf( saveDst, saveMax, "%s", p->name );
		AppConsole::transmit( saveDst );
		p++;
		if ( p->name != NULL )
		{
			AppConsole::transmit( " " );
		}
	}
	AppConsole::transmit( "\r\n" );
	// This write only if "help=full" mode
	if ( enumOp == HELP_FULL )
	{
		AppConsole::transmit( "\r\nvalues list:\r\n" );
		p = optionsList;
		while ( p->name != NULL )
		{
			dst = saveDst;
			max = saveMax;
			AppLib::printCell( dst, p->name, max, MAX_OPTION_WIDTH );
			AppConsole::transmit( saveDst );
			AppConsole::transmit( "= " );
			switch ( p->routine )
			{
				case INTPARM:
					AppConsole::transmit( "< integer value >" );
					break;
				case MEMPARM:
					AppConsole::transmit( "< value in memory size units, can use K/M/G, example \"4M\" >" );
					break;
				case SELPARM:
					p1 = p->values;
					k = 1;
					while ( *p1 != NULL )
					{
						snprintf( saveDst, saveMax, "%s", *p1 );
						AppConsole::transmit( saveDst );
						p1++;
						if ( ( *p1 != NULL ) && ( ( k % 4 ) != 0 ) )
						{
							AppConsole::transmit( " | " );
						}
						if ( ( *p1 != NULL ) && ( ( k % 4 ) == 0 ) )
						{
							dst = saveDst;
							max = saveMax;
							AppLib::printCell( dst, " ", max, LEFT_INTERVAL );
							AppConsole::transmit( "\r\n" );
							AppConsole::transmit( saveDst );
						}
					k++;
					}
					break;
				case STRPARM:
					AppConsole::transmit( "< text string >" );
					break;
				default:
					break;
			}
			p++;
			AppConsole::transmit( "\r\n" );
		}
	}
	snprintf( saveDst, saveMax, "\r\n%s\r\n\r\n", exampleString );
	AppConsole::transmit( saveDst );
}


