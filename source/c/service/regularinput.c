/*
 *   Regular data input support: command line parameters extract and parsing.
 */

void regularInput ( int argc, char** argv, OPTION_ENTRY option_list[] )
{
    int i=0, j=0, k=0, k1=0, k2=0;  // miscellaneous counters and variables
    int recognized = 0;             // result of strings comparision, 0=match 
    OPTION_TYPES t = 0;             // enumeration of parameters types for accept
    char* pAll = NULL;              // pointer to option full string NAME=VALUE
    char* pName = NULL;             // pointer to sub-string NAME
    char* pValue = NULL;            // pointer to sub-string VALUE
    char* pPattern = NULL;          // pointer to compared pattern string
    char** pPatterns = NULL;        // pointer to array of pointers to pat. strings
    DWORD* pDword = NULL;           // pointer to integer (32b) for variable store
    DWORDLONG* pLong = NULL;        // pointer to long (64b) for variable store
    DWORDLONG k64;                  // transit variable for memory block size
    char c = 0;                     // transit storage for char
    char cmdName[SMAX];             // extracted NAME of option string
    char cmdValue[SMAX];            // extracted VALUE of option string
    int MSG_MAX = 80+SMAX;
    CHAR* msgString[MSG_MAX];       // error string about option
    CHAR* pstr = NULL;

    for (i=0; i<argc; i++)
    {
        printf( "\nparm # %d = %s", i+1 , argv[i] );
    }

// Verify command line format and extract values	
    for (i=1; i<argc; i++)  // cycle for command line options
    {
        // initializing for parsing current string
        // because element [0] is application name, starts from [1]
        pAll = argv[i];
        for ( j=0; j<SMAX; j++ )  // clear buffers
            {
            cmdName[j]=0;
            cmdValue[j]=0;
            }
        // check option sub-string length
        k = strlen(pAll);                   // k = length of one option sub-string
        if ( k<SMIN )
        {
            snprintf( pstr, MSG_MAX, "option too short: %s", pAll );
            exitWithInternalError( pstr );
        }
        if ( j>SMAX )
        {
            snprintf( pstr, MSG_MAX, "option too long: %s", pAll );
            exitWithInternalError( pstr );
        }
	
        // Parse command line parameters, setup input parameters, visual
        // extract option name and option value substrings
        pName = cmdName;
        pValue = cmdValue;
        strcpy( pName, pAll );           // store option sub-string to pName
        strtok( pName, "=" );            // pName = pointer to fragment before "="
        pValue = strtok( NULL, "?" );    // pValue = pointer to fragment after "="
        // check option name and option value substrings
        k1 = 0;
        k2 = 0;
        if ( pName  != NULL ) { k1 = strlen( pName );  }
        if ( pValue != NULL ) { k2 = strlen( pValue ); }
        if ( ( k1==0 )||( k2==0 ) )
        {
            snprintf( pstr, MSG_MAX, "invalid option: %s", pAll );
            exitWithInternalError( pstr );
        }

        // detect option by comparision from list, cycle for supported options
        j = 0;
        while ( option_list[j].name != NULL )
        {
            pPattern = option_list[j].name;
            recognized = strcmp ( pName, pPattern );
            if ( recognized==0 )
            {
                // option-type specific handling, run if name match
                t = option_list[j].routine;
                switch(t)
                {
                    case INTPARM:  // support integer parameters
                    {
                        k1 = strlen( pValue );
                        for ( k=0; k<k1; k++ )
                        {
                            if ( isdigit( pValue[k] ) == 0 )
                            {
                                snprintf( pstr, MSG_MAX, "not a number: %s", pValue );
                                exitWithInternalError( pstr );
                            }
                        }
                        k = atoi( pValue );   // convert string to integer
                        pDword = option_list[j].data;
                        *pDword = k;
                        break;
                        }
                    case MEMPARM:  // support memory block size parameters
                    {
                        k1 = 0;
                        k2 = strlen( pValue );
                        c = pValue[k2-1];
                        if ( isdigit(c) != 0 )
                        {
                            k1 = 1;             // no units kilo, mega, giga
                        }
                        else if ( c == 'K' )    // K means kilobytes
                        {
                            k2--;               // last char not a digit K/M/G
                            k1 = 1024;
                        }
                        else if ( c == 'M' )    // M means megabytes
                        {
                            k2--;
                            k1 = 1024*1024;
                        }
                        else if ( c == 'G' )    // G means gigabytes
                        {
                            k2--;
                            k1 = 1024*1024*1024;
                        }
                        for ( k=0; k<k2; k++ )
                        {
                            if ( isdigit( pValue[k] ) == 0 )
                            {
                                k1 = 0;
                            }
                        }
                        if ( k1==0 )
                        {
                            snprintf( pstr, MSG_MAX, "not a block size: %s", pValue );
                            exitWithInternalError( pstr );
                        }
                        k = atoi( pValue );   // convert string to integer
                        k64 = k;
                        k64 *= k1;
                        pLong = option_list[j].data;
                        *pLong = k64;
                        break;
                        }
                    case SELPARM:    // support parameters selected from text names
                    {
                        k = 0;
                        k2 = 0;
                        pPatterns = option_list[j].values;
                        while ( pPatterns[k] != NULL )
                        {
                            pPattern = pPatterns[k];
                            k2 = strcmp ( pValue, pPattern );
                            if ( k2==0 )
                            {
                                pDword = option_list[j].data;
                                *pDword = k;
                                break;
                            }
                        k++;
                        }
                        if ( k2 != 0 )
                        {
                            snprintf( pstr, MSG_MAX, "value invalid: %s", pAll );
                            exitWithInternalError( pstr );
                        }
                        break;
                    }
                    case STRPARM:    // support parameter as text string
                    {
                        // pPatterns = path = pointer to pathBuffer
                        // pValue = pointer to source temp parsing buffer
                        pPatterns = option_list[j].data;
                        strcpy ( *pPatterns, pValue );
                        break;
                    }
                }
            break;
            }
        j++;
        }
        // check option name recognized or not
        if ( recognized != 0 )
        {
            snprintf( pstr, MSG_MAX, "option not recognized: %s", pName );
            exitWithInternalError( pstr );
        }
    }
}

