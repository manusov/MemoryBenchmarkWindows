/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Converts monitored data from report to application output,
 * this class used at CONSOLE mode scenario.
 * 
 */

package mpeshell.taskmonitor;

import java.util.concurrent.CopyOnWriteArrayList;

public class ReportToConsoleListener implements WatchDataListener
{
private int previous = 0;
    
@Override public void dataHandler
    ( CopyOnWriteArrayList<String> reportStrings, OpStatus reportStatus )
    {
    if ( reportStrings != null )
        {
        int n = reportStrings.size();
        int m = n - previous;
        if ( m > 0 )
            {
            for( int i=previous; i<n; i++ )
                System.out.println( reportStrings.get( i ) );
            previous = n;
            }
        }
    }
}
