/*
 *   Multiprocessing topology information by OS API.
 *   Build 4 information blocks: 
 *    1) Descriptors array binary dump.
 *    2) SMP topology list.
 *    3) Caches list.
 *    4) System summary.
 */

/*

TODO:
1) Don't clear screen fully.
2) When wait any key, don't use ALT, CONTROL, SHIFT.
   Or alternative variant, otherwise ALT-PrintScreen at PowerShell not possible.
3) Required support processor groups.

*/


// Standard includes
#include <stdio.h>
#include <windows.h>

// Definitions
#define LIMIT_STRING 80
#define DEFAULT_COLOR FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE   // console colors
#define DEFAULT_COLOR_MASK ~ ( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY )
typedef struct
{
	WORD attribute;
	LPSTR text;
} CSTR;
typedef CSTR* CSTRP;

// This application includes
#include "service\service.h"
#include "system\system.h"
#include "service\printhelpers.c"
#include "service\bitops.c"
#include "service\dumpbytes.c"
#include "service\dumpwords.c"
#include "service\dumpdwords.c"
#include "service\dumpqwords.c"
#include "system\getlpi.c"

// Build type string definition
#if __i386__ & _WIN32
#define BUILD_STRING "Multiprocessing topology information by OS API. v0.10.00 for Windows ia32."
#define NATIVE_LIBRARY_NAME "mpe_w_32.dll"
#define NATIVE_WIDTH 32
#elif __x86_64__ & _WIN64
#define BUILD_STRING "Multiprocessing topology information by OS API. v0.10.00 for Windows x64."
#define NATIVE_LIBRARY_NAME "mpe_w_64.dll"
#define NATIVE_WIDTH 64
#else
#define BUILD_STRING "UNSUPPORTED PLATFORM."
#endif

// Title and service strings include copyright
CHAR* stringTitle1 = BUILD_STRING;
CHAR* stringTitle2 = "(C)2018 IC Book Labs.";
CHAR* stringAnyKey = "Press any key...";

// Console application mode control: dialogue (TRUE) or redirect (FALSE)
BOOL dialogueMode;

// Console support data
HANDLE hStdin, hStdout;                // handles for standard IN , standard OUT
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;   // console control structure
COORD oldDwSize;                       // console window size with scroll, example 80x300
COORD oldDwCursorPosition;             // cursor position at application start
WORD oldWAttributes;                   // video color attribute
SMALL_RECT oldSrWindow;                // console window size visible without scrollm example 0, 0, 79, 24
COORD oldDwMaximumWindowSize;          // console window size if maximized but without scroll, example 80x68

// console additional (calculated and defined) geometry parameters
WORD defaultColor;                     // Default color for output, can be original color or constant value
DWORD dwConSize;                       // Console size used for clear screen
DWORD cCharsWritten;                   // Chars count used for clear screen
COORD coordScreen = { 0, 0 };          // Coordinates used for clear screen

// Platform detect control variables
DWORD status = -1;
PSYSTEM_LOGICAL_PROCESSOR_INFORMATION bufPtr = NULL;
DWORD bufSize = 0;
int bufCount = 0;

// Console routines declaration
BOOL initializeScreen( WORD color );
void waitAnyKey( LPSTR promptString );
void lineOfTable(int charsCount );
BOOL colorPrint( CSTRP colorStrings );
void exitWithSystemError( CHAR* operationName );
void exitWithInternalError( CHAR* messageName );
void exitWithMessage( CHAR* messageName );

// Helper routine: clear screen and set cursor position to 0,0
BOOL initializeScreen( WORD color )
{
    if ( dialogueMode )
    {
        BOOL status = FALSE;
        status = SetConsoleTextAttribute( hStdout, color );
        if (!status)
        {
            return status;
        }
        status = FillConsoleOutputCharacter( hStdout, (CHAR) ' ',
                 dwConSize, coordScreen, &cCharsWritten );
        if (!status)
        {
            return status;
        }
        status = FillConsoleOutputAttribute( hStdout, color,
                 dwConSize, coordScreen, &cCharsWritten );
        if (!status)
        {
            return status;
        }
        status = SetConsoleCursorPosition( hStdout, coordScreen );
        if (!status)
        {
            return status;
        }
    }
}

// Helper routine: wait any key
void waitAnyKey( LPSTR promptString )
{
    if ( dialogueMode )
    {
        if ( promptString != NULL )
        {
            printf(promptString);
        }
        BOOL keepRunning = 1;
        while(keepRunning)
        {
            INPUT_RECORD event = {0};
            DWORD readCount = 0;
            if( PeekConsoleInput( hStdin, &event, 1, &readCount ) && readCount>0 )
            {
                if( ReadConsoleInput( hStdin, &event, 1, &readCount ) )
                {
                    if( event.EventType == KEY_EVENT )
                    {
                        KEY_EVENT_RECORD keyEvent = event.Event.KeyEvent;
                        if ( keyEvent.bKeyDown == TRUE )
                        {
                            keepRunning = 0;
                        }
                    }
                }
            }
        }
    }
    else
    {
        printf( "\nWait key skipped in the redirect mode.\n" );
    }
}

// Print line of table
void lineOfTable( int charsCount )
{
	int i = 0;
	for( i=0; i<charsCount; i++ )
	{
		printf("-");
	}
}

// Helper routine: color output 
BOOL colorPrint( CSTRP colorStrings )
{
    BOOL status1 = TRUE, status2 = TRUE;
    int i = 0;
    if ( dialogueMode )
    {
        do {
            status1 = SetConsoleTextAttribute( hStdout, colorStrings[i].attribute );
            if (status1)
            {
                printf(colorStrings[i].text);
            }
            else
            {
                break;
            }
        i++;
        } while ( colorStrings[i].text != NULL );
        status2 = SetConsoleTextAttribute( hStdout, defaultColor );
    }
    else
    {
        do {
            printf(colorStrings[i].text);
            i++;
        } while ( colorStrings[i].text != NULL );
    }
    
    return status1 && status2;
}

// Exit with error, returned by OS API,
// OS API error get and visualize,
// Failed operation name accepted as input string
void exitWithSystemError( CHAR* operationName )
{
    // Get error code from OS API
    DWORD dwError = GetLastError();
    // Local variables
    LPVOID lpvMessageBuffer;
    DWORD status;
    // Build message string = f (error code)
    status = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  		    FORMAT_MESSAGE_FROM_SYSTEM |
                  		    FORMAT_MESSAGE_IGNORE_INSERTS,
                  		    NULL, dwError,
                  		    MAKELANGID( LANG_NEUTRAL, SUBLANG_DEFAULT ),
                  		    (LPTSTR)&lpvMessageBuffer, 0, NULL );
    if ( status != 0)
    {
        // this visualized if error string build OK
        printf( "\nFAILED: %s\nERROR %d = %s", operationName, dwError, lpvMessageBuffer );
    }
    else
    {
        dwError = GetLastError();
        // this visualized if build error string FAILED
        printf( "\n\nFormat message FAILED with error %d", dwError );
    }
    // Free the buffer allocated by the system API function
    LocalFree( lpvMessageBuffer );
    // Terminate application
    if ( dialogueMode )
    {
        waitAnyKey( stringAnyKey );
        SetConsoleTextAttribute( hStdout, oldWAttributes );  	// OLD: initializeScreen( oldWAttributes );
    }
    ExitProcess( 1 );
}

// Exit with error, detected internally, 
// for example: if AVX not supported, but required
void exitWithInternalError( CHAR* messageName )
{
    if ( messageName != NULL )
    {
        printf( "\nERROR: %s\n", messageName );
    }
    if ( dialogueMode )
    {
        waitAnyKey( stringAnyKey );
        SetConsoleTextAttribute( hStdout, oldWAttributes );  	// OLD: initializeScreen( oldWAttributes );
    }
    ExitProcess( 2 );
}

// Exit without errors
void exitWithMessage( CHAR* messageName )
{
    if ( messageName != NULL )
    {
        printf( "%s\n", messageName );
    }
	if ( dialogueMode )
	{
        waitAnyKey( stringAnyKey );
        SetConsoleTextAttribute( hStdout, oldWAttributes );  	// OLD: initializeScreen( oldWAttributes );
    }
    printf( "\n" );
    ExitProcess( 0 );
}


// Console application entry point
int main( int argc, char** argv ) 
{
    // Initial mode is dialogue
    dialogueMode = TRUE;
    // Initializing console support, save original settings
    hStdin = GetStdHandle(STD_INPUT_HANDLE);
    if ( hStdin == NULL ) 
    {
        exitWithSystemError( "get standard input handle" );
    }
    hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
    if ( hStdout == NULL ) 
    {
        exitWithSystemError( "get standard output handle" );
    }
    if (! GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
    {
		// exitWithSystemError( "get screen buffer configuration" );
		// changed to skip dialogue mode instead error exit
        dialogueMode = FALSE;
        printf( "\nUse console redirect mode.\n" );
    }
    
    if ( dialogueMode )
    {
        oldDwSize = csbiInfo.dwSize;
        oldDwCursorPosition = csbiInfo.dwCursorPosition;
        oldWAttributes = csbiInfo.wAttributes;
        oldSrWindow = csbiInfo.srWindow;
        oldDwMaximumWindowSize = csbiInfo.dwMaximumWindowSize;
        dwConSize = oldDwSize.X * oldDwSize.Y;
        // Clear screen with set default color, set cursor position
        defaultColor = oldWAttributes;   // can use constant DEFAULT_COLOR
        if ( ! initializeScreen( defaultColor ) )
        {
        exitWithSystemError( "clear screen and set cursor position" );
        }
        // Set console title string
        if ( ! SetConsoleTitle( stringTitle1 ) )
        {
        exitWithSystemError( "set console title" );
        }
	}

    // Print application first message strings
    printf( "%s\n%s", stringTitle1, stringTitle2 );
	
    // Get multiprocessing topology information by OS API
    status = getLPI( &bufPtr, &bufSize );
    if ( status == 1 )
    {
        exitWithSystemError( "detect platform MP topology" );
    }
    if ( ( status >= FUNCTION_INTERNAL_ERROR )||( bufPtr == NULL )||( bufSize == 0 ) )
    {
        if(bufPtr)
        {
            free(bufPtr);
        }
        exitWithInternalError( "wrong MP topology info");
    }
    bufCount = bufSize / sizeof( SYSTEM_LOGICAL_PROCESSOR_INFORMATION );
    // Local constants and variables
    CHAR* stringCore = "CPU core";
    CHAR* stringNode = "NUMA node";
    CHAR* stringPackage = "CPU package";
    CHAR* stringUnified = "Unified";
    CHAR* stringInstruction = "Instruction";
    CHAR* stringData = "Data";
    CHAR* stringTrace = "Trace";
    CHAR* stringUnknown = "Unknown";
    CHAR* sSrc;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION bufPtr1;
    PCACHE_DESCRIPTOR Cache;
    DWORD relationship;
    DWORD64 affinity;
    CHAR s1[40];
    CHAR s2[40];
    CHAR s3[40];
    CHAR s4[40];
    CHAR stmp[40];
    int i;
    BOOLEAN flag;
    DWORD x1;
    DWORD x2;
    DWORD x3;
    DWORD x4;
    DWORD x5;
    WORD color1 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
    WORD color2 = defaultColor & DEFAULT_COLOR_MASK | FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
    // Local statistics for MP topology
    int packageCount = 0;
    int numaCount = 0;
    int coreCount = 0;
    int logicalCount = 0;
    DWORD64 logicalMask = 0;
    // Local statistics for Caches
    int L1count = 0;
    int L2count = 0;
    int L3count = 0;
    int L1point = 0;
    int L2point = 0;
    int L3point = 0;
    // Sizes string
    printf( "\n\nBuffer base=%ph, size=%d bytes (%d descriptors)", bufPtr, bufSize, bufCount );
    // Print buffer dump
    printf( "\n\n Offset    x0 x1 x2 x3 x4 x5 x6 x7 x8 x9 xA xB xC xD xE xF  ASCII\n" );
    lineOfTable( LIMIT_STRING-1 );
    dumpBytes( (PBYTE)bufPtr, 0, bufSize, defaultColor );
    printf("\n");
    lineOfTable( LIMIT_STRING - 1 );
	
    // Build and print MP topology list
    bufPtr1 = bufPtr;
    printf( "\n\n Topology unit        Logical CPU affinity       Comments\n" );
    lineOfTable( LIMIT_STRING-1 );
    for( i=0; i<bufCount; i++ )
    {
        relationship = bufPtr1->Relationship;
        affinity = bufPtr1->ProcessorMask;
        flag = TRUE;
        // Build descriptor name string
        switch (relationship)
        {
            case RelationProcessorCore:
                x1 = bufPtr1->ProcessorCore.Flags;
                snprintf( s1, 39, "\n %-21s", stringCore );
                snprintf( s3, 39, "HT flag=%d", x1 );
                coreCount++;
                logicalMask |= affinity;
                break;
            case RelationNumaNode:
                x1 = bufPtr1->NumaNode.NodeNumber;
                snprintf( s1, 39, "\n %-21s", stringNode );
                snprintf( s3, 39, "Node ID=%d", x1 );
                numaCount++;
                break;
            case RelationProcessorPackage:
                snprintf( s1, 39, "\n %-21s", stringPackage );
                snprintf( s3, 39, "-" );
                packageCount++;
                break;
            default:
                flag = FALSE;
                break;
        }
        if( flag )
        {
            // Build affinity string
            scratchAffinity( stmp, affinity, 39 );
            snprintf( s2, 39, "%-27s", stmp );
            // Print all strings
            CSTR cs1[] = { { color1, s1 },
                           { color2, s2 },
                           { color2, s3 },
                           { 0, NULL    } };
            colorPrint(cs1);
        }
        bufPtr1++;
    }
    printf("\n");
    lineOfTable( LIMIT_STRING - 1 );
	
    // Build and print caches list
    bufPtr1 = bufPtr;
    printf( "\n\n Cache           Logical CPU affinity   Associativity   Line   Size\n" );
    lineOfTable( LIMIT_STRING - 1 );
    for( i=0; i<bufCount; i++ )
    {
        relationship = bufPtr1->Relationship;
        affinity = bufPtr1->ProcessorMask;
        switch( relationship )
        {
            case RelationCache:
            Cache = &bufPtr1->Cache;
            x1 = Cache->Level;
            x2 = Cache->Associativity;
            x3 = Cache->LineSize;
            x4 = Cache->Size;
            x5 = Cache->Type;
            // Build cache level string
            snprintf( s1, 39, "\n L%-2d", x1 );
            // Build cache type string
            switch(x5)
            {
                case CacheUnified:
                    sSrc = stringUnified;
                    break;
                case CacheInstruction:
                    sSrc = stringInstruction;
                    break;
                case CacheData:
                    sSrc = stringData;
                    break;
                case CacheTrace:
                    sSrc = stringTrace;
                    break;
                default:
                    sSrc = stringUnknown;
                    break;
            }
            // Modify statistics counters
            switch (x1)
            {
                case 1:
                    L1count++;
                    if ( ( x5 == CacheUnified )||( x5 == CacheData ) ) L1point = x4 / 1024;
                    break;
                case 2:
                    L2count++;
                    if ( ( x5 == CacheUnified )||( x5 == CacheData ) ) L2point = x4 / 1024;
                    break;
                    case 3:
                    L3count++;
                    if ( ( x5 == CacheUnified )||( x5 == CacheData ) ) L3point = x4 / 1024;
                    break;
            }
            snprintf( s2, 39, "%-13s", sSrc );
            // Build affinity string
            scratchAffinity( stmp, affinity, 39 );
            snprintf( s3, 39, "%-23s", stmp );
            // Build combined string: associativity, line, size
            snprintf( s4, 39, "%-16d%-7d%d", x2, x3, x4 );
			
            // Print all strings
            CSTR cs1[] = { { color1, s1 },
                           { color1, s2 },
                           { color2, s3 },
                           { color2, s4 },
                           { 0, NULL } };
            colorPrint(cs1);
            break;
        }
        bufPtr1++;
    }
    printf("\n");
    lineOfTable( LIMIT_STRING - 1 );
	
    // Build and print summary parameters list
    printf( "\n\n Parameter              Value\n" );
    lineOfTable( LIMIT_STRING - 1 );
    // Topology statistics
    printf("\n %-23s%d"   , "CPU packages" , packageCount );
    printf("\n %-23s%d"   , "NUMA nodes"   , numaCount    );
    printf("\n %-23s%d"   , "CPU cores"    , coreCount    );
    logicalCount = countSetBits( logicalMask );
    printf("\n %-23s%d"   , "Logical CPUs" , logicalCount );
    // Caches statistics
    printf("\n %-23s%d, test point = %d KB"   , "L1 caches" , L1count , L1point );
    printf("\n %-23s%d, test point = %d KB"   , "L2 caches" , L2count , L2point );
    printf("\n %-23s%d, test point = %d KB\n" , "L3 caches" , L3count , L3point );
	
    lineOfTable( LIMIT_STRING - 1 );
    // Release memory, allocated for MP info buffer
    if(bufPtr)
    {
        free(bufPtr);
    }

    // Terminate console application
    exitWithMessage("\n\nOK.");
}

