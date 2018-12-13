/*
 *   Console application template, for windows 32/64.
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
// #include "system\system.h"
#include "service\printhelpers.c"
#include "service\bitops.c"
#include "service\dumpbytes.c"
#include "service\dumpwords.c"
#include "service\dumpdwords.c"
#include "service\dumpqwords.c"
// #include "system\getlpi.c"

// Build type string definition
#if __i386__ & _WIN32
#define BUILD_STRING "Windows console application template. v0.10.00 for Windows ia32."
#define NATIVE_LIBRARY_NAME "mpe_w_32.dll"
#define NATIVE_WIDTH 32
#elif __x86_64__ & _WIN64
#define BUILD_STRING "Windows console application template. v0.10.00 for Windows x64."
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
// RESERVED FOR TARGET FUNCTIONALITY
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
            status1 = SetConsoleTextAttribute(hStdout, colorStrings[i].attribute);
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


        // RESERVED FOR TARGET FUNCTIONALITY.


        // Terminate console application
        exitWithMessage( "\n\nOK." );
}


