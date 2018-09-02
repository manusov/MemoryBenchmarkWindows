/*
 *    Get System Memory info by WinAPI.
 *    Physically installed and free memory.
 */

// Data for memory status store
MEMORYSTATUSEX memstatus;
BOOL status;

void stepMemory( MPE_MEMORY_DATA* xm, LIST_RELEASE_RESOURCES* xr )
{
	// Get system memory information
    memstatus.dwLength = sizeof(memstatus);
	status = GlobalMemoryStatusEx(&memstatus);
	if ( status == 0)
	{
		helperRelease( xr );
		exitWithSystemError( "get system memory information" );
	}
    // Update output
    DWORDLONG x1, x2;
    x1 = memstatus.ullTotalPhys;
    x2 = memstatus.ullAvailPhys;
    xm->pysicalMemory = x1;
    xm->freeMemory = x2;
    // Show results
    x1 /= 1024;
    x2 /= 1024;
    int y1;
    int y2;
    CHAR* s1 = "\nSystem memory: ";
    CHAR* s2 = "total physical=";
    CHAR* s3 = ", available physical=";
    
    if ( ( x1 > 1024*1024*1024 ) || ( x2 > 1024*1024*1024 ) )
    {
        y1 = x1;
        y2 = x2;
        printf( "%s%s%dK%s%dK", s1, s2, y1, s3, y2 );
    }
    else
    {
        y1 = x1 / 1024;
        y2 = x2 / 1024;
        printf( "%s%s%dM%s%dM", s1, s2, y1, s3, y2 );
    }
    
}

