package mpeshell.taskmonitor;

import mpeshell.MpeGuiList;
import mpeshell.opendraw.DrawController;
import mpeshell.opendraw.DrawModelInterface;
import mpeshell.opendraw.DrawViewInterface;
import mpeshell.taskmonitor.PlatformDetector.PlatformTypes;

public class ActionRun 
{
private final MpeGuiList mglst;
private final PlatformDetector platformDetector;
private final TaskMonitor taskMonitor;
private final DirectoryMonitor directoryMonitor;
private final String tempDir;
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
    reportName = platformDetector.getReportName();
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


// entry point for run benchmark scenario, get option by mglst,
// note executable binaries (EXE, DLL) must be unpacked
public void runBenchmark()
    {
    // open session for drawings window
    DrawController drawController = 
        mglst.getMpeGui().getChildDraw().getController();
    DrawModelInterface drawModel = drawController.getModel();
    DrawViewInterface drawView = drawController.getView();
    drawController.reset();
    drawModel.rescaleXmax( 80 );    // TODO. Parametrize, debug constant 80.
    drawModel.startModel();
        
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
    taskMonitor.setCmdParms( mglst.getOptionString() );

    // Connect report listener for file monitoring and GUI update
    String[] reps = reportName.split( "\\." );
    String repName, repExt;
    if ( reps.length > 1 )
        {
        repName = reps[0];
        repExt = reps[1];
        }
    else
        {
        repName = reps[0];
        repExt = null;
        }
    ReportFileListener fileListener = 
        new ReportFileListener( tempDir, repName, repExt );
    ReportToGuiListener dataListener = 
        new ReportToGuiListener( mglst.getMpeGui() );
    directoryMonitor.addReportFileListener( fileListener );
    directoryMonitor.addReportDataListener( dataListener );

    // Start directory monitor at background
    OpStatus startDirMonStatus = directoryMonitor.monitorStartBackground();

    // Start native application by start task monitor
    taskMonitor.initOpResult();
    OpStatus taskStatus = taskMonitor.executeTask();

    // Remove report listeners for file monitoring and GUI update
    directoryMonitor.removeReportFileListener( fileListener );
    directoryMonitor.removeReportDataListener( dataListener );
    
    // Stop directory monitor
    OpStatus stopDirMonStatus = directoryMonitor.monitorStop();

    // close session for drawings window
    drawModel.stopModel();
    
    }
    
}
