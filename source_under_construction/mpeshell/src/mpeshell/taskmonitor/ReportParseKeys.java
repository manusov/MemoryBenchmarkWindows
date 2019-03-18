package mpeshell.taskmonitor;

import java.math.BigDecimal;

public class ReportParseKeys 
{
private final static String[] 
        PATTERN_TABLE_UP = { "#", "size", "CPI", "nsPI", "MBPS" };
private final static String 
        PATTERN_TABLE_BOUND = "-";

public boolean detectBenchmarkTableUp( String[] words )
    {
    boolean b = false;
    int n = words.length;
    if ( n == PATTERN_TABLE_UP.length )
        {
        b = true;
        for( int i=0; i<n; i++ )
            {
            if ( ! words[i].equals( PATTERN_TABLE_UP[i] ) )
                {
                b = false;
                break;
                }
            }
        }
    return b;
    }

public boolean detectTableBoundLine( String[] words )
    {
    return ( words.length > 0 ) && ( words[0].length() > 0 ) &&
           ( words[0].subSequence( 0, 1 ).equals( PATTERN_TABLE_BOUND ) ); 
    }

public NumericEntry detectTableEntry( String[] words )
    {
    int n = words.length;
    if ( n > 1 )
        {
        double[] d = new double[n-1];
        BigDecimal[] bd = new BigDecimal[n-1];
        int k = new Integer( words[0] );
        for( int i=1; i<n; i++ )
            {
            d[i-1]  = new Double( words[i] );
            bd[i-1] = new BigDecimal( words[i] );
            }
        return new NumericEntry( k, d, bd );
        }
    else
        return null;
    }
    
}
