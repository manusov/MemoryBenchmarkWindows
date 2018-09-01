/*
 *   Operational sequence for call benchmark patterns in the multi-thread mode.
 *   Target called routine entry for benchmark operation.
 */
 
DWORD WINAPI threadEntry( LPVOID threadControl )
{

    while ( TRUE )
    {
        // Thread parameters
        HANDLE handle = ( ( THREAD_CONTROL_ENTRY* )threadControl )->eventHandle;
        DWORD rwMethodSelect = ( ( THREAD_CONTROL_ENTRY* )threadControl )->methodId;
        LPVOID bufferAlignedSrc = ( ( THREAD_CONTROL_ENTRY* )threadControl )->base1;
        LPVOID bufferAlignedDst = ( ( THREAD_CONTROL_ENTRY* )threadControl )->base2;
        SIZE_T instructionsCount = ( ( THREAD_CONTROL_ENTRY* )threadControl )->sizeInstructions;
        SIZE_T repeatsCount = ( ( THREAD_CONTROL_ENTRY* )threadControl )->measurementRepeats;
        DWORDLONG deltaTSC = 0;
        DWORD result = 0;

        // Thread main work
        ( ( ( THREAD_CONTROL_ENTRY* )threadControl )->DLL_PerformanceGate )
        ( rwMethodSelect ,  bufferAlignedSrc , bufferAlignedDst ,
          instructionsCount , repeatsCount , &deltaTSC );

        // Thread coordination
        SetEvent( handle );
        result = 0;
        while ( result == 0 )
        {
            result = WaitForSingleObject( handle, 0 );
        }
    }
} 


 
