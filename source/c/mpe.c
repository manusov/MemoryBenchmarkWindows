/*
 *   Memory Performance Engine (MPE).
 *   Windows edition.
 */

/*

TODO (near):
1) Task Accept.
2) Memory allocation.
3) Task Input.
4) Task Execute.
5) Task Output.

TODO (far):
1) Initializing DLL and CPU native support, show DLL strings.
2) RW method select option.
3) Selection (Y/N) dialogue support.
4) Benchmarking.
5) Remove string pass (CHAR* returnText), because possible application exit from callee.
6) Remove unused status return, some routines can be VOID().
7) Use seconds instead nanoseconds for TSC period ?
8) Interrogate delays for all exit scenarios, exit codes 0-3. 
9) Accurate resources release for all exit points.
10) Refactoring, example mbpsCount and arraySize duplicated. NOT DUPLICATED. DUPLICATED STEPS COUNT.
11) ...

*/


// Standard includes
#include <stdio.h>
#include <windows.h>
// Definitions for console output
#define SMIN 3           // minimum option string length, example a=b
#define SMAX 81          // maximum option and status string length
#define DEFAULT_COLOR FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE   // console colors
#define DEFAULT_COLOR_MASK ~ ( FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY )
#define BOLD_COLOR FOREGROUND_GREEN | FOREGROUND_INTENSITY
typedef struct {
    WORD attribute;
    LPSTR text;
} CSTR;
typedef CSTR* CSTRP;
// Definition for status return
typedef enum {
    STATUS_OK,                   // Return this enable continue, operation executed successfully
    STATUS_EMPTY,                // Return this enable continue, operation not required
    STATUS_NOT_SUPPORTED,        // Return this enable continue, but operation is not supported
    STATUS_UNDER_CONSTRUCTION,   // Return this enable continue, but routine is under construction
    STATUS_API_ERROR,            // Return this required exit with get and decode OS error code
    STATUS_INTERNAL_ERROR        // Return >=this required exit without  OS error code, can visual this
} RETURN_STATUS;
// Strings for status show
CHAR* statusStrings[] =
{
    "OK",
    "OK, EMPTY",
    "NOT SUPPORTED",
    "UNDER CONSTRUCTION",
    "API ERROR",
    "INTERNAL ERROR"
};
// Build type string definition
#if __i386__ & _WIN32
#define BUILD_STRING "Build v0.00.00 for Windows ia32."
#elif __x86_64__ & _WIN64
#define BUILD_STRING "Build v0.00.00 for Windows x64."
#else
#define BUILD_STRING "WRONG BUILD: UNSUPPORTED PLATFORM."
#endif
// Title and service strings include copyright
CHAR* stringTitle1 = "Memory Performance Engine.";
CHAR* stringTitle2 = BUILD_STRING;
CHAR* stringTitle3 = "(C)2018 IC Book Labs.";
CHAR* stringAnyKey = "Press any key...";
// Console input buffer
#define RETURN_SIZE 255
CHAR inputBuffer[256];
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
// Application status and text strings
int applicationStatus = -1;
CHAR applicationString[SMAX];
CHAR* pApplicationString;
// Console routines declaration
void showStatus( int status );
BOOL initializeScreen( WORD color );
DWORD inputChar( LPSTR promptString, LPSTR returnChar, int returnSize );
void newLine(void);
void scrollScreenBuffer( HANDLE, INT );
void waitAnyKey( LPSTR promptString );
void lineOfTable(int charsCount );
BOOL colorPrint( CSTRP colorStrings );
void exitWithSystemError( CHAR* operationName );
void exitWithInternalError( CHAR* messageName );
void exitWithMessage( CHAR* messageName );
// This application includes, headers
#include "service\service.h"
#include "system\system.h"
#include "task\task.h"
#include "options\options.h"
// This application includes, system support modules
#include "service\statistics.c"
#include "system\mpecpu.c"
#include "system\mpetopology.c"
#include "system\mpememory.c"
#include "system\mpeos.c"
#include "system\mpeacpi.c"
#include "system\mpenuma.c"
#include "system\mpepaging.c"
// This application includes, service helpers modules
#include "service\regularinput.c"
#include "service\regularoutput.c"
#include "service\printhelpers.c"
// Command line options support
#include "options\options.c"
#include "options\optionsdetails.c"
#include "options\cpuoptions.c"
// This application includes, benchmark task modules
#include "task\taskroot.c"
#include "task\mpetaskaccept.c"
#include "task\mpetaskinput.c"
#include "task\mpetaskexecute.c"
#include "task\mpetaskoutput.c"
#include "task\memoryallocation.c"

// Helper routines for console application context.
// Helper routine: print status string = f(status code)
void showStatus( int status )
{
    int i = status;
    CHAR* pStatus;
	if ( ( i >= STATUS_INTERNAL_ERROR ) || ( i < 0 ) )
    {
        i = STATUS_INTERNAL_ERROR;
        pStatus = statusStrings[i];
        printf( "%s %d.", pStatus, status );
    }
    else
    {
        pStatus = statusStrings[i];
        printf( "%s.", pStatus );
    }
}
// Helper routine: clear screen and set cursor position to 0,0
BOOL initializeScreen( WORD color )
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
// Modified console input, return by any key
// ReadFile returns when any input is available.  
// WriteFile is used to echo input. 
DWORD inputChar( LPSTR promptString, LPSTR returnLine, int returnSize  )
{
    // console mode variables
	DWORD fdwMode;
	DWORD fdwOldMode; 
    // save original console mode
	if (! GetConsoleMode(hStdin, &fdwOldMode)) 
    {
    	return 0;
    }
	// set required console mode
    fdwMode = fdwOldMode & ~( ENABLE_LINE_INPUT | ENABLE_ECHO_INPUT );
    if (! SetConsoleMode(hStdin, fdwMode)) 
    {
    	return 0;
    }
	
	DWORD outputCount = 0;
    DWORD inputCount = 0;
    // output prompt string
    if (! WriteFile ( 
          hStdout,                 // output handle 
          promptString,            // prompt string 
          lstrlenA(promptString),  // string length 
          &outputCount,            // bytes written 
          NULL) )                  // not overlapped 
    // if error, restore original console mode and return
	{
        SetConsoleMode(hStdin, fdwOldMode);
		return 0;
    }
	// input 1 char
    if ( ! ReadFile( hStdin, returnLine, 1, &inputCount, NULL ) )
    // if error, restore original console mode and return
	{
		SetConsoleMode(hStdin, fdwOldMode);
		return 0;
	}
	// if ENTER key, make new line and return 
    if ( returnLine[0] == '\r' )
    {
        newLine();
        SetConsoleMode(hStdin, fdwOldMode);
        return returnLine[0];
    }
    // output char to console	
    else if ( ! WriteFile(hStdout, returnLine, inputCount, &outputCount, NULL) )
    // if error, return 0
    {
		SetConsoleMode(hStdin, fdwOldMode);
		return 0;
	}
	// if no errors, return this char
    else
    {
		SetConsoleMode(hStdin, fdwOldMode);
		return returnLine[0];
	}
}
// The newLine function handles carriage returns when the processed 
// input mode is disabled. It gets the current cursor position 
// and resets it to the first cell of the next row. 
void newLine(void) 
{ 
    if (! GetConsoleScreenBufferInfo(hStdout, &csbiInfo)) 
    {
        MessageBox(NULL, "GetConsoleScreenBufferInfo", 
                         "Console Error", MB_OK); 
        return;
    }
    csbiInfo.dwCursorPosition.X = 0; 
    // If it is the last line in the screen buffer, scroll 
    // the buffer up. 
    if ((csbiInfo.dwSize.Y-1) == csbiInfo.dwCursorPosition.Y) 
    { 
        scrollScreenBuffer(hStdout, 1); 
    } 
    // Otherwise, advance the cursor to the next line. 
    else csbiInfo.dwCursorPosition.Y += 1; 

    if (! SetConsoleCursorPosition(hStdout, 
        csbiInfo.dwCursorPosition)) 
    {
        MessageBox(NULL, "SetConsoleCursorPosition", 
                         "Console Error", MB_OK); 
        return;
    }
} 
// Scroll
void scrollScreenBuffer(HANDLE h, INT x)
{
    SMALL_RECT srctScrollRect, srctClipRect;
    CHAR_INFO chiFill;
    COORD coordDest;
    srctScrollRect.Left = 0;
    srctScrollRect.Top = 1;
    srctScrollRect.Right = csbiInfo.dwSize.X - (SHORT)x; 
    srctScrollRect.Bottom = csbiInfo.dwSize.Y - (SHORT)x; 
    // The destination for the scroll rectangle is one row up. 
    coordDest.X = 0; 
    coordDest.Y = 0; 
    // The clipping rectangle is the same as the scrolling rectangle. 
    // The destination row is left unchanged. 
    srctClipRect = srctScrollRect; 
    // Set the fill character and attributes. 
    chiFill.Attributes = FOREGROUND_RED|FOREGROUND_INTENSITY; 
    chiFill.Char.AsciiChar = (char)' '; 
    // Scroll up one line. 
    ScrollConsoleScreenBuffer( 
        h,               // screen buffer handle 
        &srctScrollRect, // scrolling rectangle 
        &srctClipRect,   // clipping rectangle 
        coordDest,       // top left destination cell 
        &chiFill);       // fill character and color 
}
// Helper routine: wait any key
void waitAnyKey( LPSTR promptString )
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
    BOOL status1 = FALSE, status2 = FALSE;
    int i = 0;
    do {
        status1 = SetConsoleTextAttribute( hStdout, 
                                           colorStrings[i].attribute | ( defaultColor & DEFAULT_COLOR_MASK ) );
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
    status2 = SetConsoleTextAttribute(hStdout, defaultColor);
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
    waitAnyKey( stringAnyKey );
    SetConsoleTextAttribute( hStdout, oldWAttributes );  	// OLD: initializeScreen( oldWAttributes );
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
    waitAnyKey( stringAnyKey );
    SetConsoleTextAttribute( hStdout, oldWAttributes );  	// OLD: initializeScreen( oldWAttributes );
    ExitProcess( 2 );
}
// Exit without errors
void exitWithMessage( CHAR* messageName )
{
    if ( messageName != NULL )
    {
        printf( "%s\n", messageName );
    }
    waitAnyKey( stringAnyKey );
    SetConsoleTextAttribute( hStdout, oldWAttributes );  	// OLD: initializeScreen( oldWAttributes );
    ExitProcess( 0 );
}

// Console application entry point
int main( int argc, char** argv ) 
{
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
        exitWithSystemError( "get screen buffer configuration" );
    }
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
    // First message with console parameters
    CSTR cstrStarting[] = {	{ BOLD_COLOR , "\nStarting..." } , { 0, NULL } };
    colorPrint ( cstrStarting );
    printf( "\nsize(%d,%d) cursor(%d,%d) attribute %04xh window(%d,%d,%d,%d) maximum(%d,%d)" , 
            oldDwSize.X, oldDwSize.Y, oldDwCursorPosition.X, oldDwCursorPosition.Y, oldWAttributes,
            oldSrWindow.Left, oldSrWindow.Top, oldSrWindow.Right, oldSrWindow.Bottom,
            oldDwMaximumWindowSize.X, oldDwMaximumWindowSize.Y );
    // Print application first message strings
    printf( "\n%s %s\n%s", stringTitle1, stringTitle2, stringTitle3 );
    
    // Benchmark target task
    pApplicationString = applicationString;
    *pApplicationString = 0;
    applicationStatus = taskRoot( argc, argv, pApplicationString );
    // Target task done at this point
    
    // Exit console application
    CSTR cstrExiting[] = {	{ BOLD_COLOR , "\n\nExiting..." } , { 0, NULL } };
    colorPrint ( cstrExiting );
    printf("\nMain task return: ");
    showStatus( applicationStatus );
    int as = applicationStatus;
    if ( as > STATUS_INTERNAL_ERROR )
    {
        as = STATUS_INTERNAL_ERROR;
    }
    switch ( as )
    {
        // Branch if no errors
        case STATUS_OK:
        case STATUS_EMPTY:
        case STATUS_NOT_SUPPORTED:
        case STATUS_UNDER_CONSTRUCTION:
            exitWithMessage( " " );
            break;
        // Branch if error returned by OS API
        case STATUS_API_ERROR:
            exitWithSystemError( pApplicationString );
            break;
        // Branch if error detected by application logic
        case STATUS_INTERNAL_ERROR:
            exitWithInternalError( pApplicationString );
            break;
        // Branch for unrecognized status code    
        default:
            break;
    }
    // This executed if unrecognized status code
    exitWithMessage( "\nStatus interpreting error." );
}

