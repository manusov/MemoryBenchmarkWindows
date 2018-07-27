/*
 *   Memory allocation services.
 */

void allocateBuffer( SIZE_T bufferSize, SIZE_T bufferAlignment, SIZE_T blockMax, 
                     LPVOID *bufferBase, LPVOID *bufferAlignedSrc, LPVOID *bufferAlignedDst );

void releaseBuffer( LPVOID bufferBase );
