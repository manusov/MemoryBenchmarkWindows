package mpeshell.taskmonitor;

import java.util.Properties;

public class PlatformDetector 
{
private final static String[] OS_PATTERNS   = { "win" , "linux" };
private final static String[] CPU_PATTERNS  = { "x86" , "amd64" , "i386" };
private final static String[] PROP_PATTERNS = { "os.name" , "os.arch" };
private final static String[] WIN_32_APP    = { "mpe32" , "exe" , "dll" };
private final static String[] WIN_64_APP    = { "mpe64" , "exe" , "dll" };
private final static String[][] NATIVE_APPLICATIONS =
    { WIN_32_APP, WIN_64_APP };
private final static String REPORT_NAME = "output.txt";

public enum PlatformTypes { WIN32, WIN64, LINUX32, LINUX64, UNKNOWN }
private PlatformTypes selector = PlatformTypes.UNKNOWN;

public PlatformTypes getSelector()
    {
    return selector; 
    }

public void setSelector( PlatformTypes n ) 
    {
    selector = n;
    }

public String[] getNameExt()
    {
    String[] s = null;
    if ( ( selector == PlatformTypes.WIN32 ) ||
         ( selector == PlatformTypes.WIN64 ) )
        {
        s = NATIVE_APPLICATIONS[ selector.ordinal() ];
        }
    return s;
    }

public String[][] getAllNameExt()
    {
    return NATIVE_APPLICATIONS;
    }

public String getReportName()
    {
    return REPORT_NAME;
    }

public void detect()
    {
    // initializing variables, xO = OS index, xC = CPU index
    selector = PlatformTypes.UNKNOWN;
    int xO = -1;
    int xC = -1;
    int nP = PROP_PATTERNS.length;
    int nO = OS_PATTERNS.length;
    int nC = CPU_PATTERNS.length;
    Properties p = System.getProperties();
    String[] arguments = new String[nP];
    String s1, s2;
    int n1, n2;
    // get system info, built array of strings
    for ( int i=0; i<nP; i++ )
        {
        s1 = p.getProperty( PROP_PATTERNS[i] );
        s1 = s1.trim();
        s1 = s1.toUpperCase();
        arguments[i] = s1;
        }
    // detect OS type
    for ( int i=0; i<nO; i++ )
        {
        s1 = arguments[0];
        n1 = s1.length();
        s2 = OS_PATTERNS[i];
        s2 = s2.trim();
        s2 = s2.toUpperCase();
        n2 = s2.length();
        if ( n1 >= n2 )
            {
            s1 = s1.substring( 0, n2 );
            if ( s1.equals( s2 ) ) { xO = i; }
            }
        }
    // detect CPU architecture
    for ( int i=0; i<nC; i++ )
        {
        s1 = arguments[1];
        n1 = s1.length();
        s2 = CPU_PATTERNS[i];
        s2 = s2.trim();
        s2 = s2.toUpperCase();
        n2 = s2.length();
        if ( n1 >= n2 )
            {
            s1 = s1.substring( 0, n2 );
            if ( s1.equals( s2 ) ) { xC = i; }
            }
        }
    // analysing and return
    if ( ( xO==0 ) && ( xC==0 ) ) { selector = PlatformTypes.WIN32;   }
    if ( ( xO==0 ) && ( xC==1 ) ) { selector = PlatformTypes.WIN64;   }
    if ( ( xO==1 ) && ( xC==0 ) ) { selector = PlatformTypes.LINUX32; }
    if ( ( xO==1 ) && ( xC==2 ) ) { selector = PlatformTypes.LINUX32; }
    if ( ( xO==1 ) && ( xC==1 ) ) { selector = PlatformTypes.LINUX64; }
    }
    
}
