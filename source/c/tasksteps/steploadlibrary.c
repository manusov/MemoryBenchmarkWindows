/*
 *   Load native functions library, load functions, show library name strings.
 *   Note load results verification at next steps.
 */

// DLL and Functions names strings: for load and for error reporting
CHAR* dllName = "mpe_w_64.dll";
CHAR* fname1 = "GetDllStrings";
CHAR* fname2 = "CheckCpuid";
CHAR* fname3 = "ExecuteCpuid";
CHAR* fname4 = "ExecuteXgetbv";
CHAR* fname5 = "MeasureTsc";
CHAR* fname6 = "PerformanceGate";

void stepLoadLibrary( LIST_DLL_FUNCTIONS* xf, LIST_RELEASE_RESOURCES* xr )
{
    // Load DLL
    printf( "Load %s...", dllName );
    xr->dllName = dllName;
    xr->nativeLibrary = LoadLibrary( dllName );
	if( xr->nativeLibrary == NULL )
    {
        int N = 100;
        CHAR p[N+1];
        snprintf( p, N, "load module=%s", dllName );
        exitWithSystemError( p );	
    }
    // Load function: GetDllStrings
    xf->name1 = fname1;
    xf->DLL_GetDllStrings = ( void (__stdcall *) ( char** , char** , char** ) )
    GetProcAddress( xr->nativeLibrary, fname1 );
    // Load function: CheckCpuid
    xf->name2 = fname2;
    xf->DLL_CheckCpuid = ( BOOL (__stdcall *) ( void ) )
    GetProcAddress( xr->nativeLibrary, fname2 );
    // Load function: ExecuteCpuid
    xf->name3 = fname3;
    xf->DLL_ExecuteCpuid = ( void (__stdcall *) ( DWORD, DWORD, DWORD*, DWORD*, DWORD*, DWORD* ) )
    GetProcAddress( xr->nativeLibrary, fname3 );
    // Load function: ExecuteXgetbv
    xf->name4 = fname4;
    xf->DLL_ExecuteXgetbv = ( void (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( xr->nativeLibrary, fname4 );
    // Load function: MeasureTsc
    xf->name5 = fname5;
    xf->DLL_MeasureTsc = ( BOOL (__stdcall *) ( DWORDLONG* ) )
	GetProcAddress( xr->nativeLibrary, fname5 );
    // Load function: PerformanceGate
    xf->name6 = fname6;
    xf->DLL_PerformanceGate = ( BOOL (__stdcall *) ( DWORD, byte* , byte* , size_t , size_t , DWORDLONG* ) )
	GetProcAddress( xr->nativeLibrary, fname6 );
	// Print DLL strings
	if ( ( xf->DLL_GetDllStrings ) != NULL )
	{
        char *dllProduct, *dllVersion, *dllVendor;
        xf->DLL_GetDllStrings( &dllProduct, &dllVersion, &dllVendor );
        printf( "\n%s %s %s", dllProduct, dllVersion, dllVendor );
    }
}

