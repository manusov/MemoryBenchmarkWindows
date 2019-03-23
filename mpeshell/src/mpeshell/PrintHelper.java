/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Static helper class for print support.
 *
 */

package mpeshell;

public class PrintHelper 
{

private static final int KB = 1024;
private static final int MB = 1024*1024;
private static final int GB = 1024*1024*1024;
protected static String printSize( long n )
    {
    double m = n;
    String s;
    if ( m < KB )
        {
        s = String.format( "%d", n );
        }
    else if ( m < MB )
        {
        if ( n % KB == 0 )
            s = String.format( "%dK", n/KB );
        else
            s = String.format( "%.1fK", m/KB );
        }
    else if ( m < GB )
        {
        if ( n % MB == 0 )
            s = String.format( "%dM", n/MB );
        else
            s = String.format( "%.1fM", m/MB );
        }
    else
        {
        if ( n % GB == 0 )
            s = String.format( "%dG", n/GB );
        else
            s = String.format( "%.1fG", m/GB );
        }
    return s;
    }
    
protected static String printRoundSize( long x )
    {
    return printRoundSize( x, true );
    }

protected static String printRoundSize( long x, boolean b )
    {
    String s = "?";
    if ( x < 0 )
        return s;
    else if ( x < 1024L )
        {  // print as bytes
        int y = ( int )( x );
        s = b ? String.format( "%d Bytes", y ) 
              : String.format( "%d", y );
        }
    else if ( x < 1024*1024L )
        {  // print as kilobytes
        int y = ( int )( x / 1024L );
        s = String.format( "%dK", y );
        }
    else if ( x < 1024*1024*1024L )
        {  // print as megabytes
        int y = ( int )( x / (1024*1024L) );
        s = String.format( "%dM", y );
        }
    else if ( x < 1024*1024*1024*1024L )
        {  // print as gigabytes
        int y = ( int )( x / (1024*1024*1024L) );
        s = String.format( "%dG", y );
        }
    // otherwise return string="?" for big sizes at this method context
    // terabytes not supported yet
    return s;    
    }

    
}
