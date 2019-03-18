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
