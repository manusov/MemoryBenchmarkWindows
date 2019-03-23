/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Support report text parsing for extract events and parameters,
 * used for this java application monitoring native application output.
 * 
 */

package mpeshell.taskmonitor;

import java.math.BigDecimal;
import mpeshell.taskmonitor.SysInfoEntry.InfoTypes;

public class ReportParseKeys 
{
private final static String[] 
        PATTERN_TABLE_UP = { "#", "size", "CPI", "nsPI", "MBPS" };

// detect string with measurement results table up with parameters names
protected boolean detectBenchmarkTableUp( String[] words )
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

private final static String PATTERN_TABLE_BOUND = "-";

// detect string with table bound line "---...---"
protected boolean detectTableBoundLine( String[] words )
    {
    return ( words.length > 0 ) && ( words[0].length() > 0 ) &&
           ( words[0].subSequence( 0, 1 ).equals( PATTERN_TABLE_BOUND ) ); 
    }

// extract numeric values from result table line
protected NumericEntry detectTableEntry( String[] words )
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

// extract cache or DRAM size, bitmaps or threads count from report line
protected SysInfoEntry detectSysInfoParameter( String[] words )
    {
    InfoTypes type = null;
    long value = -1;
    if ( ( words != null )&&( words.length >= 3 )&&
         ( words[0] != null )&&( words[1] != null )&&( words[2] != null ) )
        {
        if ( cacheHelper( words, "L1" ) )
            {
            type = InfoTypes.L1;
            value = kilobytesHelper( words[2], value );
            }
        if ( cacheHelper( words, "L2" ) )
            {
            type = InfoTypes.L2;
            value = kilobytesHelper( words[2], value );
            }
        if ( cacheHelper( words, "L3" ) )
            {
            type = InfoTypes.L3;
            value = kilobytesHelper( words[2], value );
            }
        if ( cacheHelper( words, "L4" ) )
            {
            type = InfoTypes.L4;
            value = kilobytesHelper( words[2], value );
            }
        if ( dramHelper( words ) )
            {
            if ( words.length >= 5 )
                {
                type = InfoTypes.DRAM;
                value = sizesHelper( words[3], words[4], value );
                }
            }
        }
    if ( type == null )
        return null;
    else
        return new SysInfoEntry( type, value );
    }

private boolean cacheHelper( String[] w, String key )
    {
    return ( ( w[0].equals( key ) ) &&
           ( ( w[1].equals( "data" ) ) || ( w[1].equals( "unified" ) ) ) );
    }

private boolean dramHelper( String[] w )
    {
    return ( ( w[0].equals( "installed" ) ) &&
           ( ( w[1].equals( "memory" ) ) || ( w[1].equals( "=" ) ) ) );
    }

private long kilobytesHelper( String numstr, long value )
    {
    if ( numstr.chars().allMatch( Character::isDigit ) );
        value = Integer.parseInt( numstr );
    return value * 1024;
    }

private long sizesHelper( String numstr, String unitstr, long value )
    {
    double size = 0.0;
    if ( numstr.matches( "[+-]?\\d*(\\.\\d+)?" ) )
        {
        size = Double.parseDouble( numstr );
        }
    double mul = 1.0;
    if ( unitstr.length() > 0 )
        {
        char c = unitstr.charAt( 0 );
        switch ( c )
            {
            case 'K':
                mul = 1024;
                break;
            case 'M':
                mul = 1024*1024;
                break;
            case 'G':
                mul = 1024*1024*1024;
                break;
            default:
                mul = 1.0;
                break;
            }
        }
    return ( long )( size * mul );
    }

}
