/*
 *   Get Logical Processor Information, use OS API function
 */

DWORD getLPI( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION *ptrPtr, DWORD *ptrSize )
{
	// Local variables
	HANDLE hglpi = NULL;
	LPFN_GLPI glpi = NULL;
    BOOL done = FALSE;
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION tempPtr = NULL;
    DWORD tempSize = 0;
    // Load WinAPI function
	hglpi = GetModuleHandle( "kernel32" );
	if ( hglpi == NULL ) return FUNCTION_API_ERROR;
	glpi = (LPFN_GLPI) GetProcAddress( hglpi, "GetLogicalProcessorInformation" );
	if ( glpi == NULL ) return FUNCTION_API_ERROR;
// Get information, note required memory allocation for buffer
	while (!done)
    {
        DWORD rc = glpi( tempPtr, &tempSize);
        if ( rc == FALSE ) 
        {
            if (GetLastError() == ERROR_INSUFFICIENT_BUFFER) 
            {
                if ( tempPtr ) 
                    free( tempPtr );

                tempPtr = (PSYSTEM_LOGICAL_PROCESSOR_INFORMATION)malloc( tempSize );

                if ( tempPtr == NULL ) 
                {
                	return FUNCTION_API_ERROR;
                }
            } 
            else 
            {
            	return FUNCTION_API_ERROR;
            }
        } 
        else
        {
            done = TRUE;
        }
    }
    *ptrPtr = tempPtr;
    *ptrSize = tempSize;
	return FUNCTION_NO_ERRORS;    
}

