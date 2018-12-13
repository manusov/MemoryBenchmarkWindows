#include "SystemLibrary.h"

// Native dll and functions names, text constants
const char* SystemLibrary::dllName = NATIVE_LIBRARY_NAME;
const char* SystemLibrary::fname1 = "GetDllStrings";
const char* SystemLibrary::fname2 = "CheckCpuid";
const char* SystemLibrary::fname3 = "ExecuteCpuid";
const char* SystemLibrary::fname4 = "ExecuteRdtsc";
const char* SystemLibrary::fname5 = "ExecuteXgetbv";
const char* SystemLibrary::fname6 = "MeasureTsc";
const char* SystemLibrary::fname7 = "PerformanceGate";

// Loaded native library handle
HMODULE SystemLibrary::dllHandle = NULL;

// Structure with pointers to loaded native library functions
SYSTEM_FUNCTIONS_LIST SystemLibrary::f;

// Status string
#define NS 81
char SystemLibrary::s[NS];

// Helper method for verify dll functions load
BOOL SystemLibrary::functionCheck( void *functionPointer, const char *functionName, const char *dllName, DWORD &errorCode )
{
    errorCode = 0;
	if ( functionPointer == NULL )
    {
    	errorCode = GetLastError( );
		snprintf( s, NS, "load function=%s from module=%s", functionName, dllName );
		return FALSE;
	}
	return TRUE;
}

// Class constructor, blank status string
SystemLibrary::SystemLibrary( )
{
    snprintf( s, NS, "no data" );
}

// Class destructor, unload DLL
SystemLibrary::~SystemLibrary( )
{
    if ( dllHandle != NULL )
    {
        BOOL status;
        status = FreeLibrary( dllHandle );
        if ( !status )
        {
            snprintf( s, NS, "unload module=%s", dllName );
        }
    }
}

// Method loads native library and functions
DWORD SystemLibrary::loadSystemLibrary( )
{
    BOOL status;
    DWORD error;
    // Load DLL
    snprintf( s, NS, "Load %s...", dllName );
    dllHandle = LoadLibrary( dllName );
	if( dllHandle == NULL )
    {
		error = GetLastError( );
        snprintf( s, NS, "load module=%s", dllName );
        return error;
    }
    // Load function: GetDllStrings
    f.DLL_GetDllStrings = ( void (__stdcall *) ( char** , char** , char** ) )
    GetProcAddress( dllHandle, fname1 );
    status = functionCheck( ( void* )f.DLL_GetDllStrings, fname1, dllName, error );
    if ( !status ) return error;
    // Load function: CheckCpuid
    f.DLL_CheckCpuid = ( BOOL (__stdcall *) ( void ) )
    GetProcAddress( dllHandle, fname2 );
    status = functionCheck( ( void* )f.DLL_CheckCpuid, fname2, dllName, error );
    if ( !status ) return error;
    // Load function: ExecuteCpuid
    f.DLL_ExecuteCpuid = ( void (__stdcall *) ( DWORD, DWORD, DWORD*, DWORD*, DWORD*, DWORD* ) )
    GetProcAddress( dllHandle, fname3 );
    status = functionCheck( ( void* )f.DLL_ExecuteCpuid, fname3, dllName, error );
    if ( !status ) return error;
    // Load function: ExecuteRdtsc
    f.DLL_ExecuteRdtsc = ( void (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( dllHandle, fname4 );
	status = functionCheck( ( void* )f.DLL_ExecuteRdtsc, fname4, dllName, error );
	if ( !status ) return error;
    // Load function: ExecuteXgetbv
    f.DLL_ExecuteXgetbv = ( void (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( dllHandle, fname5 );
	status = functionCheck( ( void* )f.DLL_ExecuteXgetbv, fname5, dllName, error );
	if ( !status ) return error;
    // Load function: MeasureTsc
    f.DLL_MeasureTsc = ( BOOL (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( dllHandle, fname6 );
	status = functionCheck( ( void* )f.DLL_MeasureTsc, fname6, dllName, error );
	if ( !status ) return error;
    // Load function: PerformanceGate
    f.DLL_PerformanceGate = ( BOOL (__stdcall *) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* ) )
	GetProcAddress( dllHandle, fname7 );
	status = functionCheck( ( void* )f.DLL_PerformanceGate, fname7, dllName, error );
	if ( !status ) return error;
    // Return
    return 0;
}

// Method returns native library entry points list, valid if no errors
SYSTEM_FUNCTIONS_LIST* SystemLibrary::getSystemFunctionsList( )
{
    return &f;
}

// Method returns status string, valid if error returned
char* SystemLibrary::getStatusString( )
{
    return s;
}

