/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Scenario for Console mode.
 * Example console mode run: " java -jar mpeshell.jar console 32 test=memory ".
 *
 */

package mpeshell;

import mpeshell.service.About;
import mpeshell.taskmonitor.DirectoryMonitor;
import mpeshell.taskmonitor.OpStatus;
import mpeshell.taskmonitor.PlatformDetector;
import mpeshell.taskmonitor.PlatformDetector.PlatformTypes;
import mpeshell.taskmonitor.ReportFileListener;
import mpeshell.taskmonitor.ReportToConsoleListener;
import mpeshell.taskmonitor.TaskMonitor;

public class MpeConsole 
{
private final String[] cmd;
private final static String KEY_32 = "32";
private final static String KEY_64 = "64";
private final static String KEY_OUT = "out=file";
    
public MpeConsole( String[] s )
    {
    cmd = s;
    }
    
public void runBenchmarkConsole()
    {
    // First messages, write title strings
    System.out.printf( "Running console mode...\r\n%s\r\n%s\r\n%s\r\n" , 
    About.getLongName(), About.getVendorName(), About.getWebSite() );
        
    // Parse command line, select 32- or 64-bit native application and
    // build parameters string for native application
    PlatformTypes userSelection = PlatformTypes.UNKNOWN;
    StringBuilder options = new StringBuilder( "" );
    options.append( " " ).append( KEY_OUT );
    for ( String cmd1 : cmd ) 
        {
        if ( ( cmd1 != null ) && ( cmd1.equals( KEY_32 ) ) )
            userSelection = PlatformTypes.WIN32;
        else if ( ( cmd1 != null ) && ( cmd1.equals( KEY_64 ) ) )
            userSelection = PlatformTypes.WIN64;
        else if ( cmd1 != null )
            options.append( " " ).append( cmd1 );
        }
        
    // Initializing platform detector, verify selected native supported
    PlatformDetector platformDetector = new PlatformDetector();
    platformDetector.detect();
    PlatformTypes systemSelection = platformDetector.getSelector();
    System.out.printf( "Initializing platform detector... " );
    System.out.printf( "system = %s, override = %s\r\n" , 
                       systemSelection, userSelection );
    if ( ( systemSelection != PlatformTypes.WIN32 ) &&
         ( systemSelection != PlatformTypes.WIN64 ) )
        return;
    
    if ( ( systemSelection == PlatformTypes.WIN32 ) &&
         ( userSelection == PlatformTypes.WIN64 ) )
        System.out.println( "Warning: run WIN64 application under JRE32." );

    if ( ( systemSelection == PlatformTypes.WIN64 ) &&
         ( userSelection == PlatformTypes.WIN32 ) )
        System.out.println( "Warning: run WIN32 application under JRE64." );
        
    // Initializing task monitor, unpack native binaries to temporary directory
    System.out.printf( "Initializing task monitor... " );
    TaskMonitor taskMonitor = new TaskMonitor();
    OpStatus unpackStatus = taskMonitor.unpackBinaries
        ( platformDetector.getAllNameExt(), 
          platformDetector.getReportNameExtSingle() );
    System.out.println( unpackStatus.getStatusString() );
    if ( ! unpackStatus.getStatusFlag() )
        return;
    
    PlatformTypes resultSelection = userSelection;
    if ( resultSelection == PlatformTypes.UNKNOWN )
        resultSelection = systemSelection;
    
    platformDetector.setSelector( resultSelection );
    taskMonitor.setPlatform( resultSelection );
    taskMonitor.setCmdParms( options.toString() );
    
    // Initializing and start directory monitor
    String reportDir = taskMonitor.getReportDir();
    DirectoryMonitor directoryMonitor = new DirectoryMonitor( reportDir );
    System.out.printf( "Start directory monitor for path: %s\r\n" ,
                        reportDir );
    
    // Create and connect report listeners
    String[] srep = platformDetector.getReportNameExtSeparate();
    ReportFileListener fileListener = 
        new ReportFileListener( reportDir, srep[0], srep[1] );
    ReportToConsoleListener dataListener = new ReportToConsoleListener();
    directoryMonitor.addReportFileListener( fileListener );
    directoryMonitor.addReportDataListener( dataListener );

    OpStatus startDirMonStatus = directoryMonitor.monitorStartBackground();
    System.out.printf( "Directory monitor start status: %s\r\n" ,
                       startDirMonStatus.getStatusString() );
    
    // Run selected ( 32- or 64-bit) native application
    System.out.println( "---------- child task running ----------\r\n" );
    taskMonitor.initOpResult();
    OpStatus taskStatus = taskMonitor.executeTask();
    
    // Remove report listener
    directoryMonitor.removeReportFileListener( fileListener );
    directoryMonitor.removeReportDataListener( dataListener );
    // Stop directory monitor
    OpStatus stopDirMonStatus = directoryMonitor.monitorStop();
    
    System.out.printf( "\r\n---------- child task terminated ----------\r\n" + 
                       "Task shell status: %s\r\n", 
                       taskStatus.getStatusString() );
    
     // Show status for operation: stop directory monitor
    System.out.printf( "Directory monitor stop status: %s\r\n" ,
                       stopDirMonStatus.getStatusString() );
    
    // Delete unpacked native binaries and temporary directory
    System.out.printf( "Delete temporary files... " );
    OpStatus deleteStatus = taskMonitor.deleteBinaries();
    System.out.println( deleteStatus.getStatusString() );
        
    // Last message, exitind
    System.out.println( "Shell done..." );
    }

}
