/*
 *   Header for target system support routines.
 */

// Data definitions
typedef BOOL (WINAPI *LPFN_GLPI)(
    PSYSTEM_LOGICAL_PROCESSOR_INFORMATION, 
    PDWORD);
typedef enum {
	FUNCTION_NO_ERRORS,       // 0 means no errors, can continue
	FUNCTION_API_ERROR,       // 1 means required call GetLastError()
	FUNCTION_INTERNAL_ERROR   // >=2 required external decoding of status
} FUNCTION_RETURN;
// Routines definitions
DWORD getLPI( PSYSTEM_LOGICAL_PROCESSOR_INFORMATION *ptrPtr, DWORD *ptrSize );

