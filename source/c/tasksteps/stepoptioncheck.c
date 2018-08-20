/*
 *   Verify default value settings for options under construction.
 *   If mismatch, show error message and exit.
 */

void stepOptionCheck( DWORD currentSetting, DWORD defaultSetting, CHAR *optionName )
{
    if ( currentSetting != defaultSetting )
    {
        int N = 100;
        CHAR p[N+1];
        snprintf( p, N, "cannot change default \"%s\" option in the engineering release", optionName );
        exitWithInternalError( p );
    }
}


