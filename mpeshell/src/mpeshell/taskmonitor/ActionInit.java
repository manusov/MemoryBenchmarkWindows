/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Scenario for system information (Initialization handler):
 * run native application, intercept text report file updates.
 * 
 */

package mpeshell.taskmonitor;

import javax.swing.JTextArea;
import mpeshell.MpeGuiList;
import mpeshell.openlog.ActionTextLog;

public class ActionInit extends ActionRun
{
private final static String CMD_PARMS = "out=file info=all";
    
public ActionInit( MpeGuiList x )
    {
    super( x );
    }

// entry point for run benchmark scenario, get option by mglst,
// note executable binaries (EXE, DLL) must be already unpacked
public void runSysInfo()
    {
    // Select 32/64-bit native application mode
    PlatformDetector.PlatformTypes pt;
    switch( mglst.getOptionWidth() )
        {
        case 0:
            pt = PlatformDetector.PlatformTypes.WIN32;
            break;
        case 1:
            pt = PlatformDetector.PlatformTypes.WIN64;
            break;
        default:
            pt = PlatformDetector.PlatformTypes.WIN32;
            break;
        }
    platformDetector.setSelector( pt );
    taskMonitor.setPlatform( pt );
    
    // setup command line parameters for system information mode
    taskMonitor.setCmdParms( CMD_PARMS );
    
    JTextArea log1 = mglst.getMpeGui().getTextArea();
    ActionTextLog log2 = mglst.getMpeGui().getChildTextLog();

    // Write to log: Platform, tempDir, Options.
    String logRunning = 
        "\r\n\r\nRunning binary executable with parameters:";
    String logPlatform = "\r\n Platform : " + pt;
    String logTempDir  = "\r\n TempDir  : " + tempDir;
    String logOptions  = "\r\n Options  : " + CMD_PARMS + "\r\n";
    log1.append( logRunning );
    log2.write( logRunning );
    log1.append( logPlatform );
    log2.write( logPlatform );
    log1.append( logTempDir );
    log2.write( logTempDir );
    log1.append( logOptions );
    log2.write( logOptions );

    // Connect report listener for file monitoring and GUI update
    String[] reps = platformDetector.getReportNameExtSeparate();
    ReportFileListener fileListener = 
        new ReportFileListener( tempDir, reps[0], reps[1] );
    ReportToGuiListener dataListener = 
        new ReportToGuiListener( mglst );
    directoryMonitor.addReportFileListener( fileListener );
    directoryMonitor.addReportDataListener( dataListener );

    // Start directory monitor at background
    OpStatus startDirMonStatus = directoryMonitor.monitorStartBackground();

    // Start native application by start task monitor
    taskMonitor.initOpResult();
    OpStatus taskStatus = taskMonitor.executeTask();  // this is execution time

    // Remove report listeners for file monitoring and GUI update
    directoryMonitor.removeReportFileListener( fileListener );
    directoryMonitor.removeReportDataListener( dataListener );
    
    // Stop directory monitor
    OpStatus stopDirMonStatus = directoryMonitor.monitorStop();

    // write status information to log
    String statusString = "Load system information...";
    statusString = statusString + "\r\n" +  
        taskStatus.getStatusString();
    statusString = statusString + "\r\n" + 
        stopDirMonStatus.getStatusString();
    log1.setText( statusString );
    log2.overWrite( statusString );
    
    // re-initializing GUI (combo boxes) by system information,
    // extracted from loaded report
    mglst.getMpeGui().updateGuiBySysInfo();
    }

}
