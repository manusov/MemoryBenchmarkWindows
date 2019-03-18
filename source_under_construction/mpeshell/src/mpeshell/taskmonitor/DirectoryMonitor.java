package mpeshell.taskmonitor;

import java.io.IOException;
import java.nio.file.*;
import java.util.ArrayList;
import java.util.concurrent.CopyOnWriteArrayList;

public class DirectoryMonitor 
{
private final String dir;
private boolean alive;
private Thread thread;
private OpStatus monitorStatus;

private final ArrayList<WatchFileListener> flst = new ArrayList<>();
private final ArrayList<WatchDataListener> dlst = new ArrayList<>();

public DirectoryMonitor( String d )
    {
    dir = d;
    }

public OpStatus monitorStart()
    {
    alive = true;
    WatchService watchService;
    try {
        watchService = FileSystems.getDefault().newWatchService();
        Path path = Paths.get( dir );
        path.register( watchService, 
                       StandardWatchEventKinds.ENTRY_CREATE, 
                       StandardWatchEventKinds.ENTRY_MODIFY );
        }
    catch ( IOException e )
        {
        return new OpStatus( false, "Init failed. " + e.getMessage() );
        }
    WatchKey key;
        int extraWait = 10;
        while( extraWait > 0 )
        {

        if ( ! alive ) 
            {
            extraWait--;
            try 
                {
                Thread.sleep( 20 ); 
                }
            catch ( InterruptedException e )
                {
                return new OpStatus
                    ( false, "Wait interrupted. " + e.getMessage() );   
                }
            }
        
        key = watchService.poll();
        if ( key != null )
            {
            for ( WatchEvent<?> event : key.pollEvents() ) 
                {
                int fcount = flst.size();
                for( int i=0; i<fcount; i++ )
                    {  // cycle for all file listeners
                    flst.get( i ).fileHandler( event );
                    int dcount = dlst.size();
                    for( int j=0; j<dcount; j++ )
                        {  // cycle for all data listeners
                        CopyOnWriteArrayList<String> reportStrings =
                            flst.get( i ).getReport();
                        OpStatus reportStatus =
                            flst.get( i ).getStatus();
                        dlst.get( j ).dataHandler
                            ( reportStrings, reportStatus );
                        }
                    }
                }
            key.reset();
            }
        }
    return new OpStatus( true, "OK" );
    }

public OpStatus monitorStop()
    {
    // wait extra
    try { Thread.sleep( 100 ); }
    catch ( InterruptedException e ) 
        {
        return new OpStatus
            ( false, "Termination extra wait failed. " + e.getMessage() );   
        }
    // wait with checking
    alive = false;
    int timeout = 500;
    while ( ( thread.isAlive() ) && ( timeout > 0 ) )
        {
        timeout--;
        try { Thread.sleep( 20 ); }
        catch ( InterruptedException e ) 
            {
            return new OpStatus
                ( false, "Termination failed. " + e.getMessage() );   
            }
        }
        // wait extra
    try { Thread.sleep( 100 ); }
    catch ( InterruptedException e ) 
        {
        return new OpStatus
            ( false, "Termination extra wait failed. " + e.getMessage() );   
        }
    if ( timeout > 0 )
        return new OpStatus( true, "OK" );
    else
        return new OpStatus( false, "Termination timeout." );
    }

public OpStatus monitorStartBackground()
    {
    Runnable r = () -> 
        {
        monitorStatus = null;
        monitorStatus = monitorStart();
        };
    thread = new Thread( r );
    thread.start();
    return new OpStatus( true, "OK" );
    }

public OpStatus getMonitorStatus()
    {   
    if ( monitorStatus != null )
        return monitorStatus;
    else
        return new OpStatus( false, "N/A" );
    }

public void addReportFileListener( WatchFileListener wfl )
    {
    flst.add( wfl );
    }

public void removeReportFileListener( WatchFileListener wfl )
    {
    flst.remove( wfl );
    }

public void addReportDataListener( WatchDataListener wdl )
    {
    dlst.add( wdl );
    }

public void removeReportDataListener( WatchDataListener wdl )
    {
    dlst.remove( wdl );
    }

}
