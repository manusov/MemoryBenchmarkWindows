/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Scenario for benchmark session ("Run" button handler):
 * run native application, intercept text report file updates.
 * 
 */

package mpeshell.taskmonitor;

import javax.swing.DefaultBoundedRangeModel;
import javax.swing.JProgressBar;
import javax.swing.JTextArea;
import mpeshell.MpeGuiList;
import mpeshell.opendraw.DrawController;
import mpeshell.opendraw.DrawModelInterface;
import mpeshell.opendraw.DrawViewInterface;
import mpeshell.openlog.ActionTextLog;
import mpeshell.taskmonitor.PlatformDetector.PlatformTypes;

public class ActionRun 
{
final MpeGuiList mglst;
final PlatformDetector platformDetector;
final TaskMonitor taskMonitor;
final DirectoryMonitor directoryMonitor;
final String tempDir;
private final String reportName;
private final String[][] binariesList;
private final OpStatus initStatus;
    
// constructor with native binaries unpacking, include "openSession" step
public ActionRun( MpeGuiList x )
    {
    mglst = x;
    platformDetector = new PlatformDetector();
    platformDetector.detect();
    taskMonitor = new TaskMonitor();
    reportName = platformDetector.getReportNameExtSingle();
    binariesList = platformDetector.getAllNameExt();
    // unpack all binaries for all session
    initStatus = taskMonitor.unpackBinaries( binariesList, reportName );
    // this depend on unpack results
    tempDir = taskMonitor.getReportDir();
    directoryMonitor = new DirectoryMonitor( tempDir );
    // add temporary directory name to init status string
    String s1 = ( tempDir == null ) ? "N/A" : tempDir;
    String s2 = initStatus.getStatusString() + "\r\nTempDir = " + s1;
    initStatus.setStatusString( s2 );
    }

public OpStatus getInitStatus()
    {
    return initStatus;
    }

public OpStatus closeSession()
    {
    OpStatus ops = taskMonitor.deleteBinaries();
    String s1 = ( tempDir == null ) ? "N/A" : tempDir;
    String s2 = ops.getStatusString() + "\r\nTempDir = " + s1;
    ops.setStatusString( s2 );
    return ops;
    }

private final int PROGRESS_1 = 5;
private final int PROGRESS_2 = 95;
protected int getProgress1() { return PROGRESS_1; }
protected int getProgress2() { return PROGRESS_2; }

// entry point for run benchmark scenario, get option by mglst,
// note executable binaries (EXE, DLL) must be already unpacked
public void runBenchmark()
    {
    // Initializing progress indicator, update it for 0 percents
    JProgressBar progressBar = mglst.getMpeGui().getProgressBar();
    DefaultBoundedRangeModel progressModel = ( DefaultBoundedRangeModel )
        progressBar.getModel();
    progressUpdate( progressModel, progressBar, 0 );

    // Open session for drawings window and logging
    DrawController drawController = 
        mglst.getMpeGui().getChildDraw().getController();
    DrawModelInterface drawModel = drawController.getModel();
    DrawViewInterface drawView = drawController.getView();
    // drawController.reset();
    // drawModel.rescaleXmax( 80 );    // TODO. Parametrize, debug constant 80.
    drawModel.startModel();
    JTextArea log1 = mglst.getMpeGui().getTextArea();
    ActionTextLog log2 = mglst.getMpeGui().getChildTextLog();
        
    // Select 32/64-bit native application mode
    PlatformTypes pt;
    switch( mglst.getOptionWidth() )
        {
        case 0:
            pt = PlatformTypes.WIN32;
            break;
        case 1:
            pt = PlatformTypes.WIN64;
            break;
        default:
            pt = PlatformTypes.WIN32;
            break;
        }
    platformDetector.setSelector( pt );
    taskMonitor.setPlatform( pt );

    // Get command line parameters
    String cmdParms = mglst.getOptionString();
    taskMonitor.setCmdParms( cmdParms );
    
    // Write to log: Platform, tempDir, Options.
    String logRunning = 
        "\r\n\r\nRunning binary executable with parameters:";
    String logPlatform = "\r\n Platform : " + pt;
    String logTempDir  = "\r\n TempDir  : " + tempDir;
    String logOptions  = "\r\n Options  : " + cmdParms + "\r\n";
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
    
    // Update progress indicator: 5 percents
    progressUpdate( progressModel, progressBar, PROGRESS_1 );  // start work

    // Start native application by start task monitor
    taskMonitor.initOpResult();
    OpStatus taskStatus = taskMonitor.executeTask();  // this is execution time
    
    // Update progress indicator: 95 percents
    progressUpdate( progressModel, progressBar, PROGRESS_2 );  // end work

    // Remove report listeners for file monitoring and GUI update
    directoryMonitor.removeReportFileListener( fileListener );
    directoryMonitor.removeReportDataListener( dataListener );
    
    // Stop directory monitor
    OpStatus stopDirMonStatus = directoryMonitor.monitorStop();

    // Close session for drawings window
    drawModel.stopModel();
    
    // re-initializing GUI (combo boxes) by system information,
    // extracted from loaded report
    mglst.getMpeGui().updateGuiBySysInfo();
    
    // Done, update progress indicator: 100 percents
    progressUpdate( progressModel, progressBar, 100 );
    }

public void interruptBenchmark()
    {
    taskMonitor.interruptTask();
    }

public void progressUpdate( DefaultBoundedRangeModel model, JProgressBar bar, 
                               int percentage )
    {
    model.setValue( percentage );
    bar.setString( model.getValue() + "%" );
    }

}
