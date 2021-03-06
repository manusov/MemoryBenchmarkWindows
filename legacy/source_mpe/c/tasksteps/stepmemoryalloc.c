/*
 *    Memory allocation for benchmark read-write buffer.
 */

void stepMemoryAlloc( MPE_INPUT_PARAMETERS_BLOCK* ipb, LIST_RELEASE_RESOURCES* xr )
{
    
    DWORD_PTR bufferAlignment = 4096;  // DEBUG, TODO: GET USED PAGE SIZE
    
    
    // Detect maximum block size for positioning source and destination
    SIZE_T maxBlock = ipb->selectBlockStart;
    if ( maxBlock < ipb->selectBlockStop )
    {
        maxBlock = ipb->selectBlockStop;
    }
    // Allocate memory buffer
    xr->bufferTarget = VirtualAlloc( NULL, maxBlock*2+bufferAlignment, MEM_COMMIT, PAGE_READWRITE );
    ipb->baseOriginal = xr->bufferTarget;
    if ( ipb->baseOriginal == NULL )
    {
        exitWithSystemError( "memory allocation" );
    }
    // Interpreting memory allocation results, make alignment
    DWORD_PTR x1 = ( DWORD_PTR )( ipb->baseOriginal );
    DWORD_PTR x2 = x1 % bufferAlignment;
    if ( x2 != 0 )
    {
        x2 = bufferAlignment - x2;
        x1 += x2;
    }
    ipb->baseSrcAligned = ( LPVOID )x1;
    ipb->baseDstAligned = ( LPVOID )( x1 + maxBlock );
}


