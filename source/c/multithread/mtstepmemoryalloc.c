/*
 *   Operational sequence for call benchmark patterns in the multi-thread mode.
 *   Allocate memory resources for performance buffer.
 */

void mtStepMemoryAlloc( MT_DATA* mtd, MPE_INPUT_PARAMETERS_BLOCK* ipb, 
                        LIST_RELEASE_RESOURCES* xr )
{

    DWORD_PTR bufferAlignment = 4096;  // DEBUG, TODO: GET USED PAGE SIZE
    
    
    // Detect maximum block size for positioning source and destination
    SIZE_T maxBlock = ipb->selectBlockStart;
    if ( maxBlock < ipb->selectBlockStop )
    {
        maxBlock = ipb->selectBlockStop;
    }
    // Multiply by number of threads
    int n = mtd->threadsCount;
    maxBlock *= n;
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
    // This only for first block, because multi-thread mode
    ipb->baseSrcAligned = ( LPVOID )x1;
    ipb->baseDstAligned = ( LPVOID )( x1 + maxBlock );
    
    // Update threads management list
    int i = 0;
    THREAD_CONTROL_ENTRY* ePointer = mtd->threadsControl;
    for( i=0; i<n; i++ )
    {
        ePointer->base1 += x1;
        ePointer->base2 += x1;
        ePointer++;
    }
}

