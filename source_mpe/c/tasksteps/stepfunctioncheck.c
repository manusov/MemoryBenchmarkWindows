/*
 *   Verify after load DLL function, if error, show error message and exit
 */

void stepFunctionCheck( void *functionPointer, CHAR *functionName, CHAR *dllName )
{
    if ( functionPointer == NULL )
    {
    	int N = 100;
		CHAR p[N+1];
    	snprintf(p, N, "load function=%s from module=%s", functionName, dllName );
		exitWithSystemError( p );	
	}
}

