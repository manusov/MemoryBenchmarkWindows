/*
 *   Memory allocation services.
 */

void allocateBuffer( SIZE_T bufferSize, SIZE_T bufferAlignment, SIZE_T blockMax,
                     LPVOID *bufferBase, LPVOID *bufferAlignedSrc, LPVOID *bufferAlignedDst )
{
    *bufferBase = VirtualAlloc( NULL, bufferSize, MEM_COMMIT, PAGE_READWRITE );
    if ( *bufferBase == NULL )
    {
        exitWithSystemError( "memory allocation" );
    }
    DWORD_PTR x1 = (DWORD_PTR)(*bufferBase);
    DWORD_PTR x2 = x1 % bufferAlignment;
    if ( x2 != 0 )
    {
        x2 = bufferAlignment - x2;
        x1 += x2;
    }
    *bufferAlignedSrc = (LPVOID)x1;
    *bufferAlignedDst = (LPVOID)(x1 + blockMax);
}

void releaseBuffer( LPVOID bufferBase )
{
    if ( bufferBase != NULL )
    {
        BOOL status = VirtualFree( bufferBase, 0, MEM_RELEASE );
        if ( status == 0 )
        {
            exitWithSystemError( "memory allocation" );
        }
    }
}

