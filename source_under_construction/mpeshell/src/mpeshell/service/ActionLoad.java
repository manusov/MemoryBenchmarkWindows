/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Handler for "Load" button, load and re-interpreting previously saved
 * this application text report file or console MPE32/MPE64 application report.
 *
 */

package mpeshell.service;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileReader;
import java.util.concurrent.CopyOnWriteArrayList;
import javax.swing.DefaultBoundedRangeModel;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JProgressBar;
import javax.swing.filechooser.FileNameExtensionFilter;
import mpeshell.MpeGuiList;
import mpeshell.taskmonitor.ActionRun;
import mpeshell.taskmonitor.OpStatus;
import mpeshell.taskmonitor.ReportToGuiListener;

public class ActionLoad 
{

private final MpeGuiList mglst;
public ActionLoad( MpeGuiList x )
    {
    mglst = x;
    }

public void loadReportDialogue()
    {
    // get window context
    JFrame parentWin = mglst.getMpeGui().getFrame();
    // initializing file operations context
    final String DEFAULT_FILE_NAME = "report.txt";
    JFileChooser chooser = new JFileChooser();
    chooser.setDialogTitle( "Load report - select directory" );
    FileNameExtensionFilter filter = 
        new FileNameExtensionFilter ( "Text files" , "txt" );
    chooser.setFileFilter( filter );
    chooser.setFileSelectionMode( JFileChooser.FILES_ONLY );
    chooser.setSelectedFile( new File( DEFAULT_FILE_NAME ) );
    int select = chooser.showOpenDialog( parentWin );
    // load file, build list of strings
    boolean loaded = false;
    CopyOnWriteArrayList<String> report = null;
    if( select == JFileChooser.APPROVE_OPTION )
        {
        String filePath = chooser.getSelectedFile().getPath();
        report = new CopyOnWriteArrayList<>();
                File file = new File( filePath );
        try ( FileReader fileReader = 
                new FileReader( file ); 
              BufferedReader bufferedReader = 
                new BufferedReader( fileReader ); )
            {
            report.clear();
            String line;
            while ( ( line = bufferedReader.readLine() ) != null )
                report.add( line );
            loaded = true;
            }
        catch ( Exception e )
            {
            loaded = false;
            }
        }
    // interpreting list of strings, if loaded successfully
    if ( ( loaded ) && ( report != null ) )
        {
        ReportToGuiListener listener = new ReportToGuiListener( mglst );
        OpStatus status = new OpStatus( true, "OK" );
        listener.dataHandler( report, status );
        // set progress indicator to 100 percents
        JProgressBar progressBar = mglst.getMpeGui().getProgressBar();
        DefaultBoundedRangeModel progressModel = ( DefaultBoundedRangeModel )
            progressBar.getModel();
        ActionRun ar = mglst.getMpeGui().getTaskShell();
        ar.progressUpdate( progressModel, progressBar, 100 );
        // message box about report loaded successfully
        JOptionPane.showMessageDialog
            ( parentWin, "Report loaded successfully", "LOAD REPORT",
            JOptionPane.WARNING_MESSAGE ); 
        }
    else
        {  // error message
        JOptionPane.showMessageDialog
            ( parentWin, "Load report failed", "ERROR",
            JOptionPane.ERROR_MESSAGE ); 
        }
        
    }

}
