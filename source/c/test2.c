/*
 *  Memory benchmark engineering sample #2, windows x64 console application.
 *  Debug paradigm: Java GUI + C(CPP) console + Assenbler DLL.
 *  (C)2018 IC Book Labs.
 *
 *                    THIS IS SANDBOX FOR REFACTORING.
 *
 */

#include <windows.h>
#include <stdio.h>
#include <math.h>

// Text strings control settings
#define SMIN 3           // minimum option string length, example a=b
#define SMAX 81          // maximum option string length
#define PRINT_NAME  20   // number of chars before "=" for tabulation
#define DEFAULT_COLOR FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE   // console colors
#define BOLD_COLOR FOREGROUND_GREEN | FOREGROUND_INTENSITY

// Definitions for option control list (regular input)
typedef enum { 
	INTPARM, MEMPARM, SELPARM, STRPARM 
} OPTION_TYPES;

typedef struct {
    char* name;             // pointer to parm. name for recognition NAME=VALUE
    char** values;          // pointer to array of strings pointers, text opt.
    void* data;             // pointer to updated option variable
    OPTION_TYPES routine;   // select handling method for this entry
} OPTION_ENTRY;

// Definitions for parameters visual list (regular output)
typedef enum { 
	INTEGER, MEMSIZE, SELECTOR, POINTER, HEX64, MHZ, STRNG 
} PRINT_TYPES;

typedef struct {
    char* name;             // pointer to parameter name for visual NAME=VALUE 
    char** values;          // pointer to array of strings pointers, text opt.
    void* data;             // pointer to visualized option variable
    PRINT_TYPES routine;    // select handling method for this entry
} PRINT_ENTRY;

// Definitions for console routines
void delayBeforeExit();
typedef struct COLOR_STRING
{
	WORD attribute;
	LPSTR text;
} CSTR;
typedef CSTR* CSTRP;

// Console routines declarations
BOOL colorPrint( CSTRP colorStrings );
BOOL colorPrint( CSTRP colorStrings );
BOOL clearScreen( WORD color );
void displayGivenError(CHAR* opName, DWORD dwError);
void displayError(CHAR* opName);
void clearConsoleBeforeExit();
void abortApplication();
void abortError(CHAR* errorString);
DWORD inputLine( LPSTR promptString, LPSTR returnLine, int returnSize );
DWORD inputChar( LPSTR promptString, LPSTR returnChar, int returnSize );
void waitAnyKey( LPSTR promptString );
void newLine(void); 
void scrollScreenBuffer(HANDLE, INT);
void dllFunctionCheck( void *functionPointer, CHAR *functionName, CHAR *dllName );
void optionCheck( DWORD currentSetting, DWORD defaultSetting, CHAR *optionName );

// DLL entry points declarations, see parameters comments at assembler DLL
void ( __stdcall *DLL_GetDllStrings   ) ( char** , char** , char** );
BOOL ( __stdcall *DLL_CheckCpuid      ) ( void );
void ( __stdcall *DLL_ExecuteCpuid    ) ( DWORD, DWORD, DWORD* , DWORD* , DWORD* , DWORD* );
void ( __stdcall *DLL_ExecuteXgetbv   ) ( DWORDLONG* );
BOOL ( __stdcall *DLL_MeasureTsc      ) ( DWORDLONG* );
BOOL ( __stdcall *DLL_PerformanceGate ) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* );

// Service procedures source files: headers
#include "service\printhelpers.h"
#include "service\regularinput.h"
#include "service\regularoutput.h"
#include "service\statistics.h"

// Headers for system routines
#include "system\cpuinfo.h"
#include "system\memory.h"

// Headers for options
#include "options\rwmethods.h"
#include "options\rwtargets.h"
#include "options\rwaccess.h"
#include "options\hyperthreading.h"
#include "options\pagesize.h"
#include "options\numa.h"
#include "options\precision.h"
#include "options\machinereadable.h"

// Service procedures source files: code and data
#include "service\printhelpers.c"
#include "service\regularinput.c"
#include "service\regularoutput.c"
#include "service\statistics.c"

// System routines
#include "system\cpuinfo.c"
#include "system\memory.c"

// Data structures for options
#include "options\rwmethods.c"
#include "options\rwtargets.c"
#include "options\rwaccess.c"
#include "options\hyperthreading.c"
#include "options\pagesize.c"
#include "options\numa.c"
#include "options\precision.c"
#include "options\machinereadable.c"

// Default options settings
#define NOT_SET -1           // constant means no overrides for option, set default or default=f(sys)
#define AUTO_SET -2          // constant means auto-detect by platform configuration
#define DEFAULT_RW_METHOD AUTO_SET       // default method for read-write memory
#define DEFAULT_RW_TARGET L1_CACHE       // default tested object is L1 cache memory
#define DEFAULT_RW_ACCESS AUTO_SET       // non-temporal data mode disabled by default
#define DEFAULT_THREADS_COUNT AUTO_SET   // number of execution threads, default single thread
#define DEFAULT_HYPER_THREADING HTOFF    // hyper-threading disabled by default
#define DEFAULT_PAGE_SIZE MIN_PAGES      // default pages is minimal size, 4KB
#define DEFAULT_NUMA_MODE NON_AWARE      // default NUMA mode is non aware
#define DEFAULT_PRECISION SLOW           // default test mode is precision
#define DEFAULT_MACHINEREADABLE MROFF    // machine readable output disabled by default
#define DEFAULT_MIN_BLOCK 4096           // minimum size of default data block
#define DEFAULT_MAX_BLOCK 65536          // maximum size of default data block
#define DEFAULT_STEP_BLOCK 1024          // default step from min to max is 512 bytes
#define DEFAULT_PAGE_MODE NORMAL         // default page mode, 0 means classic 4KB
#define DEFAULT_BUF_ALIGN 4096           // alignment factor, 4KB is default page size for x86/x64

// Console support data
HANDLE hStdin, hStdout;                 // handles for standard IN , standard OUT
CONSOLE_SCREEN_BUFFER_INFO csbiInfo;    // console control structure
COORD oldDwSize;                        // console window size with scroll, example 80x300
COORD oldDwCursorPosition;              // cursor position at application start
WORD oldWAttributes;                    // video color attribute
SMALL_RECT oldSrWindow;                 // console window size visible without scrollm example 0, 0, 79, 24
COORD oldDwMaximumWindowSize;           // console window size if maximized but without scroll, example 80x68

// Default color for output
WORD defaultColor;

// console additional (calculated and defined) geometry parameters
DWORD dwConSize;
DWORD cCharsWritten;
COORD coordScreen = { 0, 0 };

// Console input buffer
#define RETURN_SIZE 255
CHAR inputBuffer[256];

// DLL support data
HINSTANCE hDLL;

// Parameters for read-write target memory block
LPVOID bufferBase;
SIZE_T bufferSize;
SIZE_T bufferAlignment;
LPVOID bufferAlignedSrc;
LPVOID bufferAlignedDst;
SIZE_T blockStart;
SIZE_T blockEnd;
SIZE_T blockStep;
SIZE_T blockMax;

// bitmap of supported options, bit[i]= 0:not supported, 1:supported
// this variable = f(platform configuration detect)
DWORDLONG rwMethodsBitmap;

// selected read-write method option
// this variable = f(command line options)
DWORD rwMethodSelect;

// bitmap of supported options, bit[i]= 0:not supported, 1:supported
// this variable = f(platform configuration detect)
DWORDLONG rwTargetsBitmap;

// selected read-write target option
// this variable = f(command line options)
DWORD rwTargetSelect;

// bitmap of supported options, bit[i]= 0:not supported, 1:supported
// this variable = f(platform configuration detect)
DWORDLONG rwAccessBitmap;

// selected access mode option
// this variable = f(command line options)
DWORD rwAccessSelect;

// Threads oount variable: maximum by platform
DWORD threadsCountMax;

// selected by command line options
DWORD threadsCountSelect;

// Bitmap of available logical processors by core, 1 bit per CPU,
// examples: 1 = HT not supported,
// 3 = 2 logical CPUs per core, 0Fh = 4 logical CPUs per core
// this variable = f(platform configuration detect)
DWORDLONG hyperThreadingBitmap;

// selected hyper threading option
// this variable = f(command line options)
DWORD hyperThreadingSelect;

// Bitmap of available page size modes
// this variable = f(platform configuration detect)
DWORDLONG pageSizeBitmap;

// selected page size support option
// this variable = f(command line options)
DWORD pageSizeSelect;

// Bitmap of available NUMA modes
// this variable = f(platform configuration detect)
DWORDLONG numaModesBitmap;

// selected NUMA topology support option
// this variable = f(command line options)
DWORD numaModeSelect;

// Bitmap of available precision modes
// this variable = f(platform configuration detect), really reserved
DWORDLONG precisionBitmap;

// selected precision support option
// this variable = f(command line options)
DWORD precisionSelect;

// Bitmap of available machine readable modes
// this variable = f(platform configuration detect), really reserved
DWORDLONG machineReadableBitmap;

// selected machine readable support option
// this variable = f(command line options)
DWORD machineReadableSelect;

// CPU information
CHAR cpuVendorString[13];
CHAR cpuModelString[49];
DWORDLONG tscClk = 0;
double tscHz = 0.0;
double tscNs = 0.0;

// Product and vendor names text strings
CHAR sName[]      = "Memory benchmark engineering sample #2.";
CHAR sVersion[]   = "v0.00.00. With extended debug messages.";
CHAR sCopyright[] = "(C)2018 IC Book Labs.";

// Option control list, used for command line parameters parsing (regular input)
static OPTION_ENTRY command_line_options_list[] = {
    { "operation"       , rwMethods       , &rwMethodSelect        , SELPARM } ,
    { "target"          , rwTargets       , &rwTargetSelect        , SELPARM } ,
    { "access"          , rwAccess        , &rwAccessSelect        , SELPARM } ,
    { "threads"         , NULL            , &threadsCountSelect    , INTPARM } ,
    { "hyperthreading"  , hyperThreading  , &hyperThreadingSelect  , SELPARM } ,
    { "pagesize"        , pageSize        , &pageSizeSelect        , SELPARM } ,
    { "numa"            , numaMode        , &numaModeSelect        , SELPARM } ,
    { "precision"       , precision       , &precisionSelect       , SELPARM } ,
    { "machinereadable" , machineReadable , &machineReadableSelect , SELPARM } ,
    { "start"           , NULL            , &blockStart            , MEMPARM } ,
    { "end"             , NULL            , &blockEnd              , MEMPARM } ,
    { "step"            , NULL            , &blockStep             , MEMPARM } ,
    { NULL              , NULL            , NULL                   , 0       }
};

// System configuration print list, used for system configuration visual (regular output)
static PRINT_ENTRY test_parameters_print_list[] = {
// YET DISABLED BECAUSE UNDER CONSTRUCTION
//    { "CPU operation"       , rwMethodsDetails       , &rwMethodSelect        , SELECTOR } ,
//    { "Target object"       , rwTargetsDetails       , &rwTargetSelect        , SELECTOR } ,
//    { "Cacheability mode"   , rwAccessDetails        , &rwAccessSelect        , SELECTOR } ,
//    { "Threads count"       , NULL                   , &threadsCountSelect    , INTEGER  } ,
//    { "Hyper-threading"     , hyperThreadingDetails  , &hyperThreadingSelect  , SELECTOR } ,
//    { "Paging mode"         , pagingModeDetails      , &pagingModeSelect      , SELECTOR } ,
//    { "NUMA topology"       , numaModeDetails        , &numaModeSelect        , SELECTOR } ,
//    { "Precision mode"      , precisionDetails       , &precisionSelect       , SELECTOR } ,
//    { "Machine readable"    , machineReadableDetails , &machineReadableSelect , SELECTOR } ,
    { "Allocated buffer"    , NULL                   , &bufferBase            , POINTER  } ,
    { "Aligned source"      , NULL                   , &bufferAlignedSrc      , POINTER  } ,
    { "Aligned destination" , NULL                   , &bufferAlignedDst      , POINTER  } ,
    { "Start block size"    , NULL                   , &blockStart            , MEMSIZE  } ,
    { "End block size"      , NULL                   , &blockEnd              , MEMSIZE  } ,
    { "Block size step"     , NULL                   , &blockStep             , MEMSIZE  } ,
    { NULL                  , NULL                   , NULL                   , 0        }
}; 

// This for debug (replace to wait key)
void delayBeforeExit()
{
	// sleep(10);
	// inputChar( "\n\nPress any key...", inputBuffer, RETURN_SIZE );
	// printf("\n\nPress any key...");
	// getchar();
	waitAnyKey( "\n\nPress any key..." );
}

// Helper routine: color output 
BOOL colorPrint( CSTRP colorStrings )
{
	BOOL status1 = FALSE, status2 = FALSE;
	int i = 0;
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
	return status1 && status2;
}

// Helper routine: clear screen
BOOL clearScreen( WORD color )
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

// Helper routine: output error message = f(error code) as string and exit
void displayGivenError(CHAR* opName, DWORD dwError)
{
    LPVOID lpvMessageBuffer;
    DWORD status;
	// Build message string
    status = FormatMessage( FORMAT_MESSAGE_ALLOCATE_BUFFER |
                  			FORMAT_MESSAGE_FROM_SYSTEM |
                  			FORMAT_MESSAGE_IGNORE_INSERTS,
                  			NULL, dwError,
                  			MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                  			(LPTSTR)&lpvMessageBuffer, 0, NULL );
    if ( status != 0)
    {
    	//... now display this string
    	printf( "\n\nError at %s\nCode %d = %s", opName, dwError, lpvMessageBuffer );
	}
	else
	{
		printf( "\n\nFormat message failed with error %d", GetLastError()) ;
	}
    // Free the buffer allocated by the system
    LocalFree(lpvMessageBuffer);
	// Terminate application
	delayBeforeExit();
	clearScreen( oldWAttributes );
    ExitProcess(GetLastError());
}

void displayError(CHAR* opName)
{
	displayGivenError(opName, GetLastError());
}

// clear screen
void clearConsoleBeforeExit()
{
	if (! clearScreen( oldWAttributes ) )
	{
		displayError( "clear screen and set cursor position" );
	}
}

// Abort application
void abortApplication()
{
	delayBeforeExit();
    clearConsoleBeforeExit();
    ExitProcess(GetLastError());
}

// Abort with given error message
void abortError(CHAR* errorString)
{
	printf( errorString );
	abortApplication();
}

// Write to STDOUT and read from STDIN by using the default 
// modes. Input is echoed automatically, and ReadFile 
// does not return until a carriage return is typed. 
// 
// The default input modes are line, processed, and echo. 
// The default output modes are processed and wrap at EOL. 

// Standard console input, terminated by ENTER
DWORD inputLine( LPSTR promptString, LPSTR returnLine, int returnSize )
{
    DWORD outputCount = 0;
    DWORD inputCount = 0;
	// console output, prompt string
	if (! WriteFile ( 
          hStdout,                  // output handle 
          promptString,             // prompt string 
          lstrlenA(promptString),   // string length 
          &outputCount,             // bytes written 
          NULL) )                   // not overlapped 
    {
        return 0;
    }
		// console input, any key
    if (! ReadFile ( 
          hStdin,       // input handle 
          returnLine,   // buffer to read into 
          returnSize,   // size of buffer 
          &inputCount,  // actual bytes read 
          NULL) )       // not overlapped 
	{
		return 0;
	}
	else
	{
		return inputCount;
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

// Wait any key
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

// verify after load DLL function, if error, show error message and exit
void dllFunctionCheck( void *functionPointer, CHAR *functionName, CHAR *dllName )
{
    if ( functionPointer == NULL )
    {
    	int N = 100;
		CHAR p[N+1];
    	snprintf(p, N, "load function=%s from module=%s", functionName, dllName );
		displayError( p );	
	}
}

// verify current and default settings match, if mismatch, show error message and exit
void optionCheck( DWORD currentSetting, DWORD defaultSetting, CHAR *optionName )
{
	if ( currentSetting != defaultSetting )
	{
    	int N = 100;
		CHAR p[N+1];
    	snprintf(p, N, "\n\nCannot change default \"%s\" option in the engineering release", optionName );
		abortError( p );	
	}
	
}

// Application entry point

int main(int argc, char** argv) {
	
// Initializing console support, save original settings
	hStdin = GetStdHandle(STD_INPUT_HANDLE);
	if ( hStdin == NULL ) 
	{
		displayError( "get standard input handle" ) ;
	}
	hStdout = GetStdHandle(STD_OUTPUT_HANDLE);
	if ( hStdout == NULL ) 
	{
		displayError( "get standard output handle" ) ;
	}
	if (! GetConsoleScreenBufferInfo(hStdout, &csbiInfo))
	{
		displayError( "get screen buffer configuration" ) ;
	}
	oldDwSize = csbiInfo.dwSize;
	oldDwCursorPosition = csbiInfo.dwCursorPosition;
	oldWAttributes = csbiInfo.wAttributes;
	oldSrWindow = csbiInfo.srWindow;
	oldDwMaximumWindowSize = csbiInfo.dwMaximumWindowSize;
	dwConSize = oldDwSize.X * oldDwSize.Y;
	
// Set console window title string
	if ( !SetConsoleTitle( sName ) )
	{
		displayError( "set console title" ) ;
	}
	
// Clear screen with set default color, set cursor position
	defaultColor = DEFAULT_COLOR;
	if (! clearScreen( defaultColor ) )
	{
		displayError( "clear screen and set cursor position" );
	}
	
// First message with console parameters
	CSTR cstrStarting[] = {	{ BOLD_COLOR , "\nStarting..." } , { 0, NULL } };
	colorPrint ( cstrStarting );
	printf( "\nsize(%d,%d) cursor(%d,%d) attribute %04xh window(%d,%d,%d,%d) maximum(%d,%d)" , 
	           oldDwSize.X, oldDwSize.Y, oldDwCursorPosition.X, oldDwCursorPosition.Y, oldWAttributes,
			   oldSrWindow.Left, oldSrWindow.Top, oldSrWindow.Right, oldSrWindow.Bottom,
			   oldDwMaximumWindowSize.X, oldDwMaximumWindowSize.Y );
			   
// Output console application strings: name, version, copyright
	printf( "\n%s\n%s\n%s", sName , sVersion , sCopyright );
	
// Load DLL and functions
	CSTR cstrLoad[] = {	{ BOLD_COLOR , "\n\nLoad DLL..." } , { 0, NULL } };
	colorPrint ( cstrLoad );
	CHAR* dllName = "test2.dll";
	hDLL = LoadLibrary( dllName );
	CHAR* functionName;
	if( hDLL == NULL )
	{
		int N = 100;
		CHAR p[N+1];
    	snprintf(p, N, "load module=%s", dllName );
		displayError( p );	
	}
	// Load function = Get DLL strings
	functionName = "GetDllStrings";
	DLL_GetDllStrings = ( void (__stdcall *) ( char** , char** , char** ) )
    GetProcAddress( hDLL, functionName );
    dllFunctionCheck( DLL_GetDllStrings, functionName, dllName );
    // Load function = Check CPUID support
    functionName = "CheckCpuid";
	DLL_CheckCpuid = ( BOOL (__stdcall *) ( void ) )
    GetProcAddress( hDLL, functionName );
    dllFunctionCheck( DLL_CheckCpuid, functionName, dllName );
    // Load function = Execute CPUID
    functionName = "ExecuteCpuid";
	DLL_ExecuteCpuid = ( void (__stdcall *) ( DWORD, DWORD, DWORD*, DWORD*, DWORD*, DWORD* ) )
    GetProcAddress( hDLL, functionName );
    dllFunctionCheck( DLL_ExecuteCpuid, functionName, dllName );
    // load function = Execute XGETBV
    functionName = "ExecuteXgetbv";
	DLL_ExecuteXgetbv = ( void (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( hDLL, functionName );
    dllFunctionCheck( DLL_ExecuteXgetbv, functionName, dllName );
    // Load function = Measure TSC clock
    functionName = "MeasureTsc";
	DLL_MeasureTsc = ( BOOL (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( hDLL, functionName );
    dllFunctionCheck( DLL_MeasureTsc, functionName, dllName );
    // Load function = Performance Gate
    functionName = "PerformanceGate";
	DLL_PerformanceGate = ( BOOL (__stdcall *) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* ) )
	GetProcAddress( hDLL, functionName );
	dllFunctionCheck( DLL_PerformanceGate, functionName, dllName );
	// Get strings from DLL, show DLL strings: name, version, copyright
	char *dllProduct, *dllVersion, *dllVendor;
	DLL_GetDllStrings( &dllProduct, &dllVersion, &dllVendor );
	printf( "\n%s\n%s\n%s", dllProduct, dllVersion, dllVendor );
	
// Set defaults, get command line with overrides, and visual parameters
	CSTR cstrCmd[] = {	{ BOLD_COLOR , "\n\nGet command line parameters..." } , { 0, NULL } };
	colorPrint ( cstrCmd );
	// Set default parameters: benchmark options
	rwMethodSelect = DEFAULT_RW_METHOD;               // default method for read-write memory
	rwTargetSelect = DEFAULT_RW_TARGET;               // default tested object is L1 cache memory
	rwAccessSelect = DEFAULT_RW_ACCESS;               // non-temporal data mode disabled by default
	threadsCountSelect = DEFAULT_THREADS_COUNT;       // number of execution threads, default single thread
	hyperThreadingSelect = DEFAULT_HYPER_THREADING;   // hyper-threading disabled by default
	pageSizeSelect = DEFAULT_PAGE_SIZE;               // page size minimal=4KB by default
	numaModeSelect = DEFAULT_NUMA_MODE;               // default NUMA mode is non aware
	precisionSelect = DEFAULT_PRECISION;              // default test mode is precision
	machineReadableSelect = DEFAULT_MACHINEREADABLE;  // machine readable output disabled by default
	// Set default parameters: memory blocks base and size
	blockStart = DEFAULT_MIN_BLOCK;
	blockEnd = DEFAULT_MAX_BLOCK;
	blockStep = DEFAULT_STEP_BLOCK;
	blockMax = DEFAULT_MAX_BLOCK;
	bufferBase = NULL;
	bufferAlignment = DEFAULT_BUF_ALIGN;
	bufferSize = 2 * blockMax + bufferAlignment;
	bufferAlignedSrc = NULL;
	bufferAlignedDst = NULL;
	// Additional temporary variables
	BOOL status;
	DWORD errorCode;
	DWORDLONG bitmapCpu, bitmapOs;
	DWORDLONG mask;
	DWORD tempSelection;
	// Get and interpreting command line parameters, override defaults
	regularInput ( argc, argv, command_line_options_list );
	// Verify options settings, include engineering sample limitations
	optionCheck ( rwMethodSelect, DEFAULT_RW_METHOD, command_line_options_list[0].name );
	optionCheck ( rwTargetSelect, DEFAULT_RW_TARGET, command_line_options_list[1].name );
	optionCheck ( rwAccessSelect, DEFAULT_RW_ACCESS, command_line_options_list[2].name );
	optionCheck ( threadsCountSelect, DEFAULT_THREADS_COUNT, command_line_options_list[3].name );
	optionCheck ( hyperThreadingSelect, DEFAULT_HYPER_THREADING, command_line_options_list[4].name );
	optionCheck ( pageSizeSelect, DEFAULT_PAGE_SIZE, command_line_options_list[5].name );
	optionCheck ( numaModeSelect, DEFAULT_NUMA_MODE, command_line_options_list[6].name );
	optionCheck ( precisionSelect, DEFAULT_PRECISION, command_line_options_list[7].name );
	optionCheck ( machineReadableSelect, DEFAULT_MACHINEREADABLE, command_line_options_list[8].name );
	
// Get and output CPU vendor and model strings
	CSTR cstrCpuid[] = { { BOLD_COLOR , "\n\nGet CPUID parameters and measure TSC clock..." } , { 0, NULL } };
	colorPrint ( cstrCpuid );
	status = detectCpu( cpuVendorString, cpuModelString );
	if ( status == 0 )
	{
		abortError( "\n\nCPUID instruction not supported or locked" );
	}
	printCpu( cpuVendorString, cpuModelString );
// Measure and output CPU TSC frequency
	errorCode = measureTsc( &tscClk, &tscHz, &tscNs );
	if ( errorCode == 1)
	{
		abortError( "\n\nTSC not supported or locked" );
	}
	else if ( errorCode > 1 )
	{
		abortError( "\n\nTSC clock measurement failed" );
	}
	printTsc( tscHz, tscNs );
// Detect and output supported read-write methods bitmap, by CPU and OS
	detectMethods( &tempSelection, &rwMethodsBitmap, &bitmapCpu, &bitmapOs );
	printMethods( tempSelection, rwMethodsBitmap, bitmapCpu, bitmapOs, rwMethodsDetails );
	if ( tempSelection == -1 )
	{
		abortError( "\n\nCannot select read/write method" );
	}
	// Check for incorrect constant assign
	if ( rwMethodSelect == AUTO_SET )  // required select method = f(platform capabilities)
	{
		rwMethodSelect = tempSelection;
	}
	else if ( rwMethodSelect == NOT_SET )  // wrong case 
	{
		abortError( "\n\nWrong internal settings for read/write method option" );
	}
	else  // required verification of user settings
	{
		mask = ((DWORDLONG)1) << rwMethodSelect;
		if ( ! ( bitmapCpu & mask ) )
		{
			abortError( "\n\nSelected read/write method not supported by CPU" );
		}
		if ( ! ( bitmapOs & mask ) )
		{
			abortError( "\n\nSelected read/write method not supported by OS context management" );
		}
	}

// Other options support under construction.

// ...


// Memory allocation
	allocateBuffer( bufferSize, bufferAlignment, blockMax, 
                    &bufferBase, &bufferAlignedSrc, &bufferAlignedDst );

// Output test operational parameters summary
	CSTR cstrOparm[] = {	{ BOLD_COLOR , "\n\nReady to start with parameters..." } , { 0, NULL } };
	colorPrint ( cstrOparm );
	regularOutput ( test_parameters_print_list );
	
// Wait user press key (y/n)
	char c = inputChar("\nStart (y/n) ? ", inputBuffer, 1);
	c = tolower(c);
	if ( c != 'y' )
	{
		releaseBuffer( bufferBase );
		printf("\nExiting...\n" );
		clearConsoleBeforeExit();
    	ExitProcess(GetLastError());
	} 
	
// Start benchmarking process

	BYTE bytesPerInstruction[] = {
	8, 8, 8, 8, 8, 8, 16, 16, 16, 32, 32, 32, 64, 64, 64, 32, 64
	};

	DWORDLONG deltaTSC = 0;
	size_t repeatsCount = 2000000;
	size_t instructionsCount = 0;
	int count = 1;
	double cpi = 0.0;
	double nspi = 0.0;
	double megabytes = 0.0;
	double seconds = 1.0 / tscHz;
	double mbps = 0.0;
	
	int stepsCount = ( blockEnd - blockStart ) / blockStep + 1;
	int arraySize = stepsCount * sizeof(double);
	double *mbpsStatistics;
	mbpsStatistics = (double*)malloc(arraySize);
	
	CSTR cstrRun[] = {	{ BOLD_COLOR , "\n\n   #    size   CPI     nsPI    MBPS\n\n" } , { 0, NULL } };
	colorPrint ( cstrRun );
	
    // pre-heat
 	instructionsCount = blockMax / bytesPerInstruction[ rwMethodSelect ];
	status = DLL_PerformanceGate( rwMethodSelect ,  bufferAlignedSrc , bufferAlignedDst ,
	                              instructionsCount , repeatsCount * 3 , &deltaTSC );
	if ( status == 0 )
	{
		abortError( "\n\nError, memory target operation failed at pre-heat phase" );
	}
	
	// measurement cycle
	while ( blockStart <= blockEnd )
	{
		instructionsCount = blockStart / bytesPerInstruction[ rwMethodSelect ];
		status = DLL_PerformanceGate( rwMethodSelect ,  bufferAlignedSrc , bufferAlignedDst ,
	                                  instructionsCount , repeatsCount , &deltaTSC );
		if ( status == 0 )
		{
			abortError( "\n\nError, memory target operation failed at measurement phase" );
		}
		cpi = deltaTSC;
        cpi /= ( instructionsCount * repeatsCount );
        nspi = cpi * tscNs;
        mbps = blockStart * repeatsCount;
        mbps /= deltaTSC * seconds;
		mbps /= 1000000.0;
		mbpsStatistics[count-1] = mbps;
    	printf ( " %3d  %6d   %5.3f   %5.3f   %-10.3f\n" , count , blockStart , cpi , nspi , mbps );
    	count++;
    	blockStart += blockStep;
 	}

// Stop benchmarking process, calculate statistics
	double min=0.0, max=0.0, average=0.0, median=0.0;
	calculateStatistics( count-1 , mbpsStatistics , &min , &max , &average , &median );

// Output test result parameters summary
	printf( "\nMBPS max=%.2f , min=%.2f , average=%.2f , median=%.2f\n\n",
	                max , min , average , median );

// Memory release
	releaseBuffer( bufferBase );

// Restore screen and exit
	delayBeforeExit();
	clearConsoleBeforeExit();
	return 0;
}

