// THIS MODULE IS RESERVED FOR FUTURE USE

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
	printf( "\r\noptions list:\r\n" );
	while ( p->name != NULL )
	{
		printf( "%s", p->name );
		p++;
		if ( p->name != NULL )
		{
			printf( " " );
		}
	}
	printf( "\r\n" );
	// This write only if "help=full" mode
	if ( enumOp == HELP_FULL )
	{
		printf( "\r\nvalues list:\r\n" );
		p = optionsList;
		while ( p->name != NULL )
		{
			dst = saveDst;
			max = saveMax;
			AppLib::printCell( dst, p->name, max, MAX_OPTION_WIDTH );
			printf( "%s= ", saveDst );
			switch ( p->routine )
			{
				case INTPARM:
					printf( "< integer value >" );
					break;
				case MEMPARM:
					printf( "< value in memory size units, can use K/M/G, example \"4M\" >" );
					break;
				case SELPARM:
					p1 = p->values;
					k = 1;
					while ( *p1 != NULL )
					{
						printf( "%s", *p1 );
						p1++;
						if ( ( *p1 != NULL ) && ( ( k % 4 ) != 0 ) )
						{
							printf( " | " );
						}
						if ( ( *p1 != NULL ) && ( ( k % 4 ) == 0 ) )
						{
							dst = saveDst;
							max = saveMax;
							AppLib::printCell( dst, " ", max, LEFT_INTERVAL );
							printf( "\r\n%s", saveDst );
						}
					k++;
					}
					break;
				case STRPARM:
					printf( "< text string >" );
					break;
				default:
					break;
			}
			p++;
			printf( "\r\n" );
		}
		// printf( "\r\n" );
	}
	printf( "\r\n%s\r\n\r\n", exampleString );
}


