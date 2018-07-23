/*
 *  Memory benchmark engineering sample #1, windows x64 console application.
 *  Debug paradigm: Java GUI + C(CPP) console + Assenbler DLL.
 *  (C)2018 IC Book Labs.
 *
 *                    THIS IS SANDBOX FOR REFACTORING.
 *
 */

/*
TODO LOCAL:
1) Add not existed patterns, verify all, include AVX512 under emulator.
2) Replace MOVAPD to MOVAPS, MOVNTPD to MOVNTPS for SSE 1 functionality.
3) Verify all instruction modes (but non-temporal when non-temporal option).

Backports to NCRB.
 */

/*
TODO GLOBAL:
I.
9 steps by options, include detect platform capabilities.
II.
1) BUG: mbpsStatistics[100] size limited, allocate memory for array
2) Console help.
3) Ergonomic, frequency measurement delay must be in the correct moment.
4) Use regularOutput subroutine for all parameters.
5) Optimize by CONST keyword.
6) Correct support for UNICODE strings: TCHAR, TEXT, TPRINTF,
verify ASCII and UNICODE variants.
III.
Port for ia32.
IV.
Ports for linux.
V.
Java GUI. JavaCrossPlatformCPUID first JCA application:
JCA = Java/C/Assembler.
---
Support all options sequentally.
DEBUG //printf( "\n\n%08X %08X %08X %08X", eax, ebx, ecx, edx );
*/

#include <windows.h>
#include <stdio.h>
#include <math.h>

// Console routines declarations
void delayBeforeExit();
typedef struct COLOR_STRING
{
	WORD attribute;
	LPSTR text;
} CSTR;
typedef CSTR* CSTRP;
BOOL colorPrintGivenHandle( HANDLE handle, CSTRP colorStrings );
BOOL colorPrint( CSTRP colorStrings );
BOOL clearScreen( WORD color );
void displayGivenError(CHAR* opName, DWORD dwError);
void displayError(CHAR* opName);
void clearConsoleBeforeExit();
void abortApplication();
void abortError(CHAR* errorString);
// This functions yet without unicode support
DWORD inputLine( LPSTR promptString, LPSTR returnLine, int returnSize );
DWORD inputChar( LPSTR promptString, LPSTR returnChar, int returnSize );
void NewLine(void); 
void ScrollScreenBuffer(HANDLE, INT); 

// DLL entry points declarations, see parameters comments at assembler DLL
void ( __stdcall *DLL_GetDllStrings   ) ( char** , char** , char** );
BOOL ( __stdcall *DLL_CheckCpuid      ) ( void );
void ( __stdcall *DLL_ExecuteCpuid    ) ( DWORD, DWORD, DWORD* , DWORD* , DWORD* , DWORD* );
void ( __stdcall *DLL_ExecuteXgetbv   ) ( DWORDLONG* );
BOOL ( __stdcall *DLL_MeasureTsc      ) ( DWORDLONG* );
BOOL ( __stdcall *DLL_PerformanceGate ) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* );

// Text strings control settings
#define SMIN 3           // minimum option string length, example a=b
#define SMAX 81          // maximum option string length
#define PRINT_NAME  20   // number of chars before "=" for tabulation
#define DEFAULT_COLOR FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE   // console colors
#define BOLD_COLOR FOREGROUND_GREEN | FOREGROUND_INTENSITY

// Connect source files: headers
#include "constants.h"
#include "helpers.h"
#include "input.h"
#include "output.h"
#include "statistics.h"
#include "optioncpu.h"

// Connect source files: code and data
#include "constants.c"
#include "helpers.c"
#include "input.c"
#include "output.c"
#include "statistics.c"
#include "optioncpu.c"

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

// Bitmap of available paging modes
// this variable = f(platform configuration detect)
DWORDLONG pagingModesBitmap;

// selected paging modes support option
// this variable = f(command line options)
DWORD pagingModeSelect;

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

// Option control list, used for command line parameters parsing
static OPTION_ENTRY option_list[] = {
	{ "operation"       , rwMethods       , &rwMethodSelect        , SELPARM } ,
	{ "target"          , rwTargets       , &rwTargetSelect        , SELPARM } ,
	{ "access"          , rwAccess        , &rwAccessSelect        , SELPARM } ,
	{ "threads"         , NULL            , &threadsCountSelect    , INTPARM } ,
	{ "hyperthreading"  , hyperThreading  , &hyperThreadingSelect  , SELPARM } ,
	{ "paging"          , pagingMode      , &pagingModeSelect      , SELPARM } ,
	{ "numa"            , numaMode        , &numaModeSelect        , SELPARM } ,
	{ "precision"       , precision       , &precisionSelect       , SELPARM } ,
	{ "machinereadable" , machineReadable , &machineReadableSelect , SELPARM } ,
	{ "start"           , NULL            , &blockStart            , MEMPARM } ,
    { "end"             , NULL            , &blockEnd              , MEMPARM } ,
    { "step"            , NULL            , &blockStep             , MEMPARM } ,
	{ NULL              , NULL            , NULL                   , 0       }
};

static PRINT_ENTRY print_list[] = {
// YET DISABLED BECAUSE UNDER CONSTRUCTION
    { "CPU operation"       , rwMethodsDetails       , &rwMethodSelect        , SELECTOR } ,
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
	inputChar( "Press any key...", inputBuffer, RETURN_SIZE );
}

// Helper routine: color output 
BOOL colorPrintGivenHandle( HANDLE handle, CSTRP colorStrings )
{
	BOOL status1 = FALSE, status2 = FALSE;
	int i = 0;
	do {
		status1 = SetConsoleTextAttribute(handle, colorStrings[i].attribute);
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
	status2 = SetConsoleTextAttribute(handle, defaultColor);
	return status1 && status2;
}

BOOL colorPrint( CSTRP colorStrings )
{
	return colorPrintGivenHandle( hStdout, colorStrings );
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
	// console input
	if (! WriteFile ( 
          hStdout,                  // output handle 
          promptString,             // prompt string 
          lstrlenA(promptString),   // string length 
          &outputCount,             // bytes written 
          NULL) )                   // not overlapped 
    {
        return 0;
    }
		// console output
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
    
    if (! WriteFile ( 
          hStdout,                 // output handle 
          promptString,            // prompt string 
          lstrlenA(promptString),  // string length 
          &outputCount,            // bytes written 
          NULL) )                  // not overlapped 
    {
        SetConsoleMode(hStdin, fdwOldMode);
		return 0;
    }

    if ( ! ReadFile( hStdin, returnLine, 1, &inputCount, NULL ) )
	{
		SetConsoleMode(hStdin, fdwOldMode);
		return 0;
	} 
    if ( returnLine[0] == '\r' )
    {
        NewLine();
        SetConsoleMode(hStdin, fdwOldMode);
        return returnLine[0];
    }
    	
    else if ( ! WriteFile(hStdout, returnLine, inputCount, &outputCount, NULL) )
    {
		SetConsoleMode(hStdin, fdwOldMode);
		return 0;
	}
    else
    {
		SetConsoleMode(hStdin, fdwOldMode);
		return returnLine[0];
	}
}

// The NewLine function handles carriage returns when the processed 
// input mode is disabled. It gets the current cursor position 
// and resets it to the first cell of the next row. 

void NewLine(void) 
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
        ScrollScreenBuffer(hStdout, 1); 
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

void ScrollScreenBuffer(HANDLE h, INT x)
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
	
// Debug input
//	DWORD a;
//	LPSTR b = "\nPlease input: ";
//	// a = inputLine( b, inputBuffer, RETURN_SIZE );
//	// printf( "\ninput count = %d", a );
//	CHAR c;
//	c = inputChar( b, inputBuffer, RETURN_SIZE );
//	printf( "\ninput char = %c", c );
//  ExitProcess(GetLastError());
//
	
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
	CSTR cstrStarting[] = {	{ BOLD_COLOR , "Starting..." } , { 0, NULL } };
	colorPrint ( cstrStarting );
	printf( "\nsize(%d,%d) cursor(%d,%d) attribute %04xh window(%d,%d,%d,%d) maximum(%d,%d)" , 
	           oldDwSize.X, oldDwSize.Y, oldDwCursorPosition.X, oldDwCursorPosition.Y, oldWAttributes,
			   oldSrWindow.Left, oldSrWindow.Top, oldSrWindow.Right, oldSrWindow.Bottom,
			   oldDwMaximumWindowSize.X, oldDwMaximumWindowSize.Y );
			   
// Output console application strings: name, version, copyright
	printf( "\n%s\n%s\n%s", sName , sVersion , sCopyright );
	
// Load DLL and functions
	CSTR cstrLoad[] = {	{ BOLD_COLOR , "\nLoad DLL..." } , { 0, NULL } };
	colorPrint ( cstrLoad );
	hDLL = LoadLibrary( "test1.dll" );
	if( hDLL == NULL )
	{
		displayError( "load DLL" );	
	}
	// Load function = Get DLL strings
	DLL_GetDllStrings = ( void (__stdcall *) ( char** , char** , char** ) )
    GetProcAddress( hDLL, "GetDllStrings" );
    if ( DLL_GetDllStrings == NULL )
    {
    	displayError( "load function GetDllStrings from DLL" );	
	}
	// Load function = Check CPUID support
	DLL_CheckCpuid = ( BOOL (__stdcall *) ( void ) )
    GetProcAddress( hDLL, "CheckCpuid" );
    if ( DLL_CheckCpuid == NULL )
    {
    	displayError( "load function CheckCpuid from DLL" );	
	}
	// Load function = Execute CPUID
	DLL_ExecuteCpuid = ( void (__stdcall *) ( DWORD, DWORD, DWORD*, DWORD*, DWORD*, DWORD* ) )
    GetProcAddress( hDLL, "ExecuteCpuid" );
    if ( DLL_ExecuteCpuid == NULL )
    {
    	displayError( "load function ExecuteCpuid from DLL" );	
	}
	// load function = Execute XGETBV
	DLL_ExecuteXgetbv = ( void (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( hDLL, "ExecuteXgetbv" );
    if ( DLL_ExecuteXgetbv == NULL )
    {
    	displayError( "load function ExecuteXgetbv from DLL" );	
	}
	// Load function = Measure TSC clock
	DLL_MeasureTsc = ( BOOL (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( hDLL, "MeasureTsc" );
    if ( DLL_MeasureTsc == NULL )
    {
    	displayError( "load function MeasureTsc from DLL" );	
	}
	// Load function = Performance Gate
	DLL_PerformanceGate = ( BOOL (__stdcall *) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* ) )
	GetProcAddress( hDLL, "PerformanceGate" );
    if ( DLL_PerformanceGate == NULL )
    {
    	displayError( "load function PerformanceGate from DLL" );	
	}

// Get strings from DLL, output DLL strings: name, version, copyright
	char *dllProduct, *dllVersion, *dllVendor;
	DLL_GetDllStrings( &dllProduct, &dllVersion, &dllVendor );
	printf( "\n%s\n%s\n%s", dllProduct, dllVersion, dllVendor );
	
// Set default parameters: benchmark options
	rwMethodSelect = DEFAULT_RW_METHOD;               // default method for read-write memory
	rwTargetSelect = DEFAULT_RW_TARGET;               // default tested object is L1 cache memory
	rwAccessSelect = DEFAULT_RW_ACCESS;               // non-temporal data mode disabled by default
	threadsCountSelect = DEFAULT_THREADS_COUNT;       // number of execution threads, default single thread
	hyperThreadingSelect = DEFAULT_HYPER_THREADING;   // hyper-threading disabled by default
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

// Get command line and visual parameters
	CSTR cstrCmd[] = {	{ BOLD_COLOR , "\nGet command line parameters..." } , { 0, NULL } };
	colorPrint ( cstrCmd );
	regularInput ( argc, argv, option_list );

// Verify input parameters validity	
	if ( rwMethodSelect != DEFAULT_RW_METHOD )
	{
		abortError( "\n\nError, cannot cahnge default RW_METHOD option in the engineering release" );
	}
	
	if ( rwTargetSelect != DEFAULT_RW_TARGET )
	{
		abortError( "\n\nError, cannot cahnge default RW_TARGET option in the engineering release" );
	}
	
	if ( rwAccessSelect != DEFAULT_RW_ACCESS )
	{
		abortError( "\n\nError, cannot cahnge default RW_ACCESS option in the engineering release" );
	}
	
	if ( threadsCountSelect != DEFAULT_THREADS_COUNT )
	{
		abortError( "\n\nError, cannot cahnge default THREADS_COUNT option in the engineering release" );
	}
	
	if ( hyperThreadingSelect != DEFAULT_HYPER_THREADING )
	{
		abortError( "\n\nError, cannot cahnge default HYPER_THREADING option in the engineering release" );
	}
	
	if ( numaModeSelect != DEFAULT_NUMA_MODE )
	{
		abortError( "\n\nError, cannot cahnge default NUMA_MODE option in the engineering release" );
	}
	
	if ( precisionSelect != DEFAULT_PRECISION )
	{
		abortError( "\n\nError, cannot cahnge default PRECISION option in the engineering release" );
	}
	
	if ( machineReadableSelect != DEFAULT_MACHINEREADABLE )
	{
		abortError( "\n\nError, cannot cahnge default MACHINEREADABLE option in the engineering release" );
	}
	
// Start get CPU parameters
	CSTR cstrCpu[] = {	{ BOLD_COLOR , "\nGet processor information..." } , { 0, NULL } };
	colorPrint ( cstrCpu );
	BOOL status;
	DWORD eax, ebx, ecx, edx;
	DWORDLONG hz;
	double frequency, nanoseconds;
	status = DLL_CheckCpuid();
	if ( status == 0 )
	{
		abortError( "\n\nError, CPUID instruction not supported or locked" );
	}

// Get platform parameters: get CPU strings
	DLL_ExecuteCpuid( 0, 0, &eax, &ebx, &ecx, &edx );
	typedef union {
		CHAR s8[52]; DWORD s32[13];
	} CPUID_STRING;
	CPUID_STRING cpuidString;
	// Vendor string
	cpuidString.s32[0] = ebx;
	cpuidString.s32[1] = edx;
	cpuidString.s32[2] = ecx;
	cpuidString.s32[3] = 0;
	printf( "\nCPU Vendor = %s", cpuidString.s8 );
	// Model string
	strcpy( cpuidString.s8 , "n/a" );
	DLL_ExecuteCpuid( 0x80000000, 0, &eax, &ebx, &ecx, &edx );
	if ( eax >= 0x80000004 )
	{
		DWORD function = 0x80000002;
		int i = 0;
		for( i=0; i<12; i+=4 )
		{
			DLL_ExecuteCpuid( function, 0, &eax, &ebx, &ecx, &edx );
			cpuidString.s32[i+0] = eax;
			cpuidString.s32[i+1] = ebx;
			cpuidString.s32[i+2] = ecx;
			cpuidString.s32[i+3] = edx;
			function++;
		}
		cpuidString.s32[i] = 0;
		CHAR* p1 = cpuidString.s8;
		CHAR* p2 = p1;
		BOOLEAN flag = FALSE;
		while ( *p1 != 0 )
		{
			if ( ( *p1 != ' ' ) || ( flag == TRUE ) )
			{
				flag = TRUE;
				*p2 = *p1;
				p2++;
			}
		p1++;	
		}
		*p2 = 0;
	}
	printf( "\nCPU Model  = %s", cpuidString.s8 );

// Get platform parameters: measure CPU TSC frequency
	status = DLL_MeasureTsc( &hz );
	if ( status == 0 )
	{
		abortError( "\n\nError, TSC clock measurement failed" );
	}
	frequency = hz;
	frequency /= 1000000.0;                      // frequency visualization units = MHz
	nanoseconds = hz;
	nanoseconds = 1000000000.0 / nanoseconds;    // period visualization units = ps (picoseconds)
	printf( "\nTSC frequency = %.2f , period = %.3f ns", frequency , nanoseconds );
	
// Get platform parameters: get CPU features
	DWORDLONG bitsCpuid=0, bitsXgetbv=0;
	bitsCpuid  = buildCpuidBitmap ( cpuidControl );
	bitsXgetbv = buildXgetbvBitmap( xgetbvControl );
	rwMethodsBitmap = bitsCpuid & bitsXgetbv;

// Verify platform parameters and user option validity: CPU read-write method
	DWORDLONG mask = 0;
	if ( rwMethodSelect == AUTO_SET )  // required select method = f(platform capabilities)
	{
		// Try AVX512 read memory method
		rwMethodSelect = CPU_FEATURE_READ_AVX512;
		mask = ((DWORDLONG)1) << rwMethodSelect;
		if ( ! (mask & rwMethodsBitmap) )
		{
			rwMethodSelect = CPU_FEATURE_READ_AVX256;
		}
		// Try AVX256 read memory method
		mask = ((DWORDLONG)1) << rwMethodSelect;
		if ( ! (mask & rwMethodsBitmap) )
		{
			rwMethodSelect = CPU_FEATURE_READ_SSE128;
		}
		// Try SSE128 read memory method
		mask = ((DWORDLONG)1) << rwMethodSelect;
		if ( ! (mask & rwMethodsBitmap) )
		{
			rwMethodSelect = CPU_FEATURE_READ_X64;
		}
		// Try common x86-64 read memory method
		mask = ((DWORDLONG)1) << rwMethodSelect;
		if ( ! (mask & rwMethodsBitmap) )
		{
			abortError( "\n\nError, compatible RW_METHOD not found" );
		}
	}
	// Check for incorrect constant assign
	else if ( rwMethodSelect == NOT_SET )  // wrong case 
	{
		abortError( "\n\nError, wrong internal settings for RW_METHOD option" );
	}
	else  // required verification of user settings
	{
		mask = ((DWORDLONG)1) << rwMethodSelect;
		if ( ! (bitsCpuid & mask ) )
		{
			abortError( "\n\nError, selected RW_METHOD not supported by CPU" );
		}
		if ( ! (bitsXgetbv & mask ) )
		{
			abortError( "\n\nError, selected RW_METHOD not supported by OS context management" );
		}
	}
	
	/* DEBUG
	printf("\n\ncpuid map=%08X%08Xh, xgetbv map=%08X%08Xh, select=%08Xh\n" , 
	        bitsCpuid >> 32, bitsCpuid, 
			bitsXgetbv >> 32, bitsXgetbv,
			rwMethodSelect );  // DEBUG
	// printf("\n\nbitmap = %08X%08Xh , select %08Xh\n" , rwMethodsBitmap >> 32, rwMethodsBitmap, rwMethodSelect );  // DEBUG
	// rwMethodsBitmap = CPU_FEATURES_UNCONDITIONAL;
	printf("\n\nmethod=%d\n\n", rwMethodSelect);
	rwMethodSelect = 0;  // DEBUG
	DEBUG */	

// Get platform parameters: detect cache memory


// Verify platform parameters and user option validity: cache memory

// Get platform parameters: detect threads count

// Verify platform parameters and user option validity: threads count

// Get platform parameters: detect page size

// Verify platform parameters and user option validity: page size

// Get platform parameters: detect NUMA configuration

// Verify platform parameters and user option validity: NUMA mode


// Get platform parameters: allocate memory (optional NUMA aware)
	bufferBase = VirtualAlloc( NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE );
	if ( bufferBase == NULL )
	{
		displayError( "memory allocation" );
	}
	DWORD_PTR x1 = (DWORD_PTR)bufferBase;
	DWORD_PTR x2 = x1 % bufferAlignment;
	if ( x2 != 0 )
	{
		x2 = bufferAlignment - x2;
		x1 += x2;
	}
	bufferAlignedSrc = (LPVOID)x1;
	bufferAlignedDst = (LPVOID)(x1 + blockMax);
	
// Verify input parameters compatible with platform	


// Output test operational parameters summary
	CSTR cstrOparm[] = {	{ BOLD_COLOR , "\nReady to start with parameters..." } , { 0, NULL } };
	colorPrint ( cstrOparm );
	regularOutput ( print_list );

// Wait user press key (y/n)


// Start benchmarking process
	DWORDLONG deltaTSC = 0;
	size_t repeatsCount = 2000000;
	size_t instructionsCount = 0;
	int count = 1;
	double cpi = 0.0;
	double nspi = 0.0;
	double megabytes = 0.0;
	double seconds = 1.0 / hz;
	double mbps = 0.0;
	
	double mbpsStatistics[100];
	
	CSTR cstrRun[] = {	{ BOLD_COLOR , "\n   #    size   CPI     nsPI    MBPS\n\n" } , { 0, NULL } };
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
        nspi = cpi * nanoseconds;
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

// Restore screen and exit
	delayBeforeExit();
	clearConsoleBeforeExit();
	return 0;
}

