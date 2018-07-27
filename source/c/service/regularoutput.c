/*
 *   Regular data output support: write to console and/or report file.
 */
 
void regularOutput ( PRINT_ENTRY print_list[] )
{
	// correct this: declarations irregular
	PRINT_TYPES m = 0;       // enumeration of parameters types for print
	DWORDLONG n = 0;         // value for block size
	DWORDLONG* np = NULL;    // pointer to block size value
	double d = 0.0;          // transit variable
	double* dp = NULL;       // transit pointer to double
	DWORD*  kp = NULL;       // pointer to integer
	char* cp = NULL;         // pointer to char(s)
	char** ccp = NULL;       // pointer to array of pointers to strings
	size_t* sizep = 0;       // pointer to block size variable
	size_t size = 0;         // block size variable
	// correct this: declarations irregular
	int i=0, j=0, k=0;
	while ( print_list[i].name != NULL )
    {
    	k = PRINT_NAME - printf( "\n%s", print_list[i].name );
    	for ( j=0; j<k; j++ )
        {
        	printf( " " );
        }
    	printf( "= " );
    	m = print_list[i].routine;
    	switch(m)
        {
        	case INTEGER:  // integer parameter
            {
            	kp = print_list[i].data;
            	k = *kp;
            	printf( "%d", k );
            	break;
            }
        	case MEMSIZE:  // memory block size parameter
            {
            	sizep = print_list[i].data;
            	size = *sizep;
            	printMemorySize( size );
            	break;
            }
        	case SELECTOR:  // pool of text names parameter
            {
            	kp = print_list[i].data;
            	k = *kp;
            	ccp = print_list[i].values;
            	printSelectedString( k, ccp );
            	break;
            }
        	case POINTER:  // memory pointer parameter
            {
            	ccp = print_list[i].data;
            	cp = *ccp;
            	printf( "%ph", cp );
            	break;
            }
        	case HEX64:  // 64-bit hex number parameter
            {
            	np = print_list[i].data;
            	n = *np;
            	printf( "0x%08llX", n );
            	break;
            }
        	case MHZ:  // frequency in MHz parameter
            {
            	dp = print_list[i].data;
            	d = *dp;  // with convert from unsigned long long to double
            	d /= 1000000.0;
            	printf( "%.1f MHz", d );
            	break;
            }
        	case STRNG:  // parameter as text string
            {
            	ccp = print_list[i].data;
            	cp = *ccp;
            	printf( "%s", *cp );
            	break;
            }
        }
    i++;
    }
    printf( "\n" );
}


