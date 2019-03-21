/*
                     MEMORY PERFORMANCE ENGINE FRAMEWORK.
                          Functions loader class.
    This class make dynamical import for Windows API and this application DLL.
*/

#include "FunctionsLoader.h"

// WinAPI and this application DLL Libraries names
const char* FunctionsLoader::sysName = "kernel32";
const char* FunctionsLoader::dllName = NATIVE_LIBRARY_NAME;
// Win API functions names
const char* FunctionsLoader::sysFunctions[] =
{
	"GetLogicalProcessorInformation",
	"GetLogicalProcessorInformationEx",
	"GlobalMemoryStatusEx",
	"GetActiveProcessorCount",
	"GetActiveProcessorGroupCount",
	"GetNumaHighestNodeNumber",
	"GetNumaNodeProcessorMask",
	"GetNumaNodeProcessorMaskEx",
	"VirtualAllocExNuma",
	"SetThreadAffinityMask",
	"SetThreadGroupAffinity",
	"GetLargePageMinimum",
	"GetSystemFirmwareTable",
	NULL
};
// This application DLL functions names
const char* FunctionsLoader::dllFunctions[] =
{
	"GetDllStrings",
	"CheckCpuid",
	"ExecuteCpuid",
	"ExecuteRdtsc",
	"ExecuteXgetbv",
	"MeasureTsc",
	"PerformanceGate",
	NULL
};
// Table up for loaded functions list.
const char* FunctionsLoader::tableUp =
	" module           function                            pointer (hex)\n";
// Control array for print list of loaded functions with libraries names
const PRINT_LOADED_FUNCTION FunctionsLoader::printLoaded[] =
{
	{ sysName, sysFunctions },
	{ dllName, dllFunctions },
	{ NULL, NULL }
};
// WinAPI and this application DLL Libraries handles
HMODULE FunctionsLoader::sysHandle;
HMODULE FunctionsLoader::dllHandle;
// List of functions with status tag
FUNCTIONS_LIST FunctionsLoader::f;
// Status string
#define NS 81
char FunctionsLoader::s[NS];

// Class constructor, blank status string, load libraries and functions
FunctionsLoader::FunctionsLoader( )
{
	// Blank status string
    snprintf( s, NS, "no data" );
    // Initializing variables
	BOOL status;
    f.loadStatus = 0;
    // Load Win API system DLL
    status = libraryLoad( sysHandle, sysName, f.loadStatus );
    if ( !status ) return;
    // This functions is OPTIONAL, no return if error, but function pointer = NULL if not function loaded
    // Load function: GetLogicalProcessorInformation
	f.API_GetLogicalProcessorInformation = ( BOOL( __attribute__((__stdcall__)) * )( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, PDWORD ) )
	GetProcAddress( sysHandle, sysFunctions[0] ); 
	// Load function: GetLogicalProcessorInformationEx
	f.API_GetLogicalProcessorInformationEx = ( BOOL( __attribute__((__stdcall__)) * )( LOGICAL_PROCESSOR_RELATIONSHIP, PSYSTEM_LOGICAL_PROCESSOR_INFORMATION_EX, PDWORD ) )
	GetProcAddress( sysHandle, sysFunctions[1] ); 
 	// Load function: GlobalMemoryStatusEx
 	f.API_GlobalMemoryStatusEx = ( BOOL( __attribute__((__stdcall__)) * ) ( LPMEMORYSTATUSEX ) )
 	GetProcAddress( sysHandle, sysFunctions[2] ); 
	// Load function: GetActiveProcessorCount
	f.API_GetActiveProcessorCount = ( DWORD( __attribute__((__stdcall__)) * )( WORD ) )
	GetProcAddress( sysHandle, sysFunctions[3] ); 
	// Load function: GetActiveProcessorGroupCount
	f.API_GetActiveProcessorGroupCount = ( WORD( __attribute__((__stdcall__)) * )( ) )
	GetProcAddress( sysHandle, sysFunctions[4] ); 
	// Load function: GetNumaHighestNodeNumber
	f.API_GetNumaHighestNodeNumber = ( BOOL( __attribute__((__stdcall__)) * )( PULONG ) )
	GetProcAddress( sysHandle, sysFunctions[5] ); 
	// Load function: GetNumaNodeProcessorMask
	f.API_GetNumaNodeProcessorMask = ( BOOL( __attribute__((__stdcall__)) * )( UCHAR, PULONGLONG ) )
	GetProcAddress( sysHandle, sysFunctions[6] ); 
	// Load function: GetNumaNodeProcessorMaskEx
	f.API_GetNumaNodeProcessorMaskEx = ( BOOL( __attribute__((__stdcall__)) * )( USHORT, PGROUP_AFFINITY ) )
	GetProcAddress( sysHandle, sysFunctions[7] ); 
	// Load function: VirtualAllocExNuma
	f.API_VirtualAllocExNuma = ( LPVOID( __attribute__((__stdcall__)) * )( HANDLE, LPVOID, SIZE_T, DWORD, DWORD, DWORD ) )
	GetProcAddress( sysHandle, sysFunctions[8] ); 
	// Load function: SetThreadAffinityMask
	f.API_SetThreadAffinityMask = ( DWORD_PTR( __attribute__((__stdcall__)) * )( HANDLE, DWORD_PTR ) )
	GetProcAddress( sysHandle, sysFunctions[9] ); 
	// Load function: SetThreadGroupAffinity
	f.API_SetThreadGroupAffinity = ( BOOL( __attribute__((__stdcall__)) * )( HANDLE, const GROUP_AFFINITY*, PGROUP_AFFINITY ) )
	GetProcAddress( sysHandle, sysFunctions[10] ); 
	// Load function: GetLargePageMinimum
	f.API_GetLargePageMinimum = ( SIZE_T( __attribute__((__stdcall__)) * )( ) )
	GetProcAddress( sysHandle, sysFunctions[11] ); 
	// Load function: GetSystemFirmwareTable
	f.API_GetSystemFirmwareTable = ( UINT( __attribute__((__stdcall__)) * )( DWORD, DWORD, PVOID, DWORD ) )
    GetProcAddress( sysHandle, sysFunctions[12] ); 
    // Load this application DLL
    f.loadStatus = 0;
    status = libraryLoad( dllHandle, dllName, f.loadStatus );
    if ( !status ) return;
    // This functions is MANDATORY, return if error
	// Load function: GetDllStrings
	f.DLL_GetDllStrings = ( void (__stdcall *) ( char** , char** , char** ) )
	GetProcAddress( dllHandle, dllFunctions[0] );
	status = functionCheck( ( void* )f.DLL_GetDllStrings, dllFunctions[0], dllName, f.loadStatus );
	if ( !status ) return;
    // Load function: CheckCpuid
    f.DLL_CheckCpuid = ( BOOL (__stdcall *) ( void ) )
    GetProcAddress( dllHandle, dllFunctions[1] );
    status = functionCheck( ( void* )f.DLL_CheckCpuid, dllFunctions[1], dllName, f.loadStatus );
    if ( !status ) return;
    // Load function: ExecuteCpuid
    f.DLL_ExecuteCpuid = ( void (__stdcall *) ( DWORD, DWORD, DWORD*, DWORD*, DWORD*, DWORD* ) )
    GetProcAddress( dllHandle, dllFunctions[2] );
    status = functionCheck( ( void* )f.DLL_ExecuteCpuid, dllFunctions[2], dllName, f.loadStatus );
    if ( !status ) return;
    // Load function: ExecuteRdtsc
    f.DLL_ExecuteRdtsc = ( void (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( dllHandle, dllFunctions[3] );
	status = functionCheck( ( void* )f.DLL_ExecuteRdtsc, dllFunctions[3], dllName, f.loadStatus );
	if ( !status ) return;
    // Load function: ExecuteXgetbv
    f.DLL_ExecuteXgetbv = ( void (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( dllHandle, dllFunctions[4] );
	status = functionCheck( ( void* )f.DLL_ExecuteXgetbv, dllFunctions[4], dllName, f.loadStatus );
	if ( !status ) return;
    // Load function: MeasureTsc
    f.DLL_MeasureTsc = ( BOOL (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( dllHandle, dllFunctions[5] );
	status = functionCheck( ( void* )f.DLL_MeasureTsc, dllFunctions[5], dllName, f.loadStatus );
	if ( !status ) return;
    // Load function: PerformanceGate
    f.DLL_PerformanceGate = ( BOOL (__stdcall *) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* ) )
	GetProcAddress( dllHandle, dllFunctions[6] );
	status = functionCheck( ( void* )f.DLL_PerformanceGate, dllFunctions[6], dllName, f.loadStatus );
	if ( !status ) return;
    // Return with status tag = 0, means no errors
    f.loadStatus = 0;
    return;
}
// Helper routine for class constructor, load dll
BOOL FunctionsLoader::libraryLoad( HMODULE &moduleHandle, const char* moduleName, DWORD &error )
{
    snprintf( s, NS, "Load %s...", moduleName );
    moduleHandle = LoadLibrary( moduleName );
	if( moduleHandle == NULL )
    {
		error = GetLastError( );
        snprintf( s, NS, "load module=%s", moduleName );
        return FALSE;
    }
	return TRUE;
}
// Helper routine for class constructor, verify dll functions load
BOOL FunctionsLoader::functionCheck( void *functionPointer, const char *functionName, const char *libName, DWORD &errorCode )
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

// Class destructor, unload DLL
FunctionsLoader::~FunctionsLoader( )
{
	libraryUnload( sysHandle, sysName );
	libraryUnload( dllHandle, dllName );
}
// Helper routine for class destructor, unload dll
void FunctionsLoader::libraryUnload( HMODULE moduleHandle, const char* moduleName )
{
	BOOL status;
    if ( moduleHandle != NULL )
    {
        status = FreeLibrary( moduleHandle );
        if ( !status )
        {
            snprintf( s, NS, "unload module=%s", moduleName );
        }
    }
}

// Function returns pointer to functions list
FUNCTIONS_LIST* FunctionsLoader::getFunctionsList( )
{
	return &f;
}

// Build text block with functions list information
#define TABLE_WIDTH 71
#define COLUMN_1 17
#define COLUMN_2 36
void FunctionsLoader::getFunctionsText( LPSTR &dst, size_t &max )
{
	AppLib::printString( dst, max, tableUp );
	AppLib::printLine( dst, max, TABLE_WIDTH );
	
	const PRINT_LOADED_FUNCTION* libraries = printLoaded;
	LPCSTR* functions = NULL;
	LPBYTE binaries = ( LPBYTE )( &f ) + sizeof ( LPVOID );
	LPVOID* x = NULL;
	LPVOID y = NULL;

	while ( libraries->libList != NULL )
	{
		functions = libraries->libList;
		while ( *functions != NULL )
		{
			AppLib::printString( dst, max, " " );
			AppLib::printCell( dst, libraries->libName, max, COLUMN_1 );
			AppLib::printCell( dst, *functions, max, COLUMN_2 );
			x = ( LPVOID* ) binaries;
			y = *x;
			AppLib::printPointer( dst, max, y, FALSE );
			AppLib::printCrLf( dst, max );
			binaries += sizeof ( LPVOID );
			functions++;
		}
		libraries++;	
	}
	
	AppLib::printLine( dst, max, TABLE_WIDTH );
	return;
}

// Method returns status string, valid if error returned
char* FunctionsLoader::getStatusString( )
{
    return s;
}

