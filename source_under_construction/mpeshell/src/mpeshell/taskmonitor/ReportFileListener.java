package mpeshell.taskmonitor;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.nio.file.WatchEvent;
import java.util.concurrent.CopyOnWriteArrayList;

public class ReportFileListener implements WatchFileListener
{
private final String dir;
private final String name;
private final String ext;
private final 
    CopyOnWriteArrayList<String> report = new CopyOnWriteArrayList<>();
private OpStatus status;
    
public ReportFileListener( String d, String n, String e )
    {
    dir = d;
    name = n;
    ext = e;
    }

@Override public void fileHandler( WatchEvent we )
    {
    status = new OpStatus( false, "In progress." );
    String myfile;
    if ( ( ext != null ) || ( ! ext.equals("") ) )
        myfile = name + "." + ext;
    else
        myfile = name;
    String eventContext = we.context().toString();
    if ( eventContext.equals( myfile ) )
        {
        String eventPath = dir + File.separator + eventContext;
        File file = new File( eventPath );
        try ( FileReader fileReader = 
                new FileReader( file ); 
              BufferedReader bufferedReader = 
                new BufferedReader( fileReader ); )
            {
            report.clear();
            String line;
            while ( ( line = bufferedReader.readLine() ) != null )
                report.add( line );
            }
        catch ( Exception e )
            {
            status = 
                new OpStatus( false, "Report error. " + e.getMessage() );
            return;
            }
        status = new OpStatus( true, "OK" );
        }
    }
   
@Override public CopyOnWriteArrayList<String> getReport()
    {
    return report;
    }

@Override public OpStatus getStatus()
    {
    return status;
    }
}
