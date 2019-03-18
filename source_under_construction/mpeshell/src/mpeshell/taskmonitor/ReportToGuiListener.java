package mpeshell.taskmonitor;

import java.math.BigDecimal;
import java.util.concurrent.CopyOnWriteArrayList;
import javax.swing.JProgressBar;
import javax.swing.JTextArea;
import mpeshell.BenchmarkTableModel;
import mpeshell.MpeGui;
import mpeshell.opendraw.ActionDraw;
import mpeshell.opendraw.DrawModelInterface;
import mpeshell.opendraw.DrawViewInterface;
import mpeshell.openlog.ActionTextLog;
import mpeshell.openstatistics.ActionStatistics;
import mpeshell.openstatistics.EntryStatistics;
import mpeshell.openstatistics.StatisticUtil;
import mpeshell.openstatistics.StatisticsTableModel;

public class ReportToGuiListener implements WatchDataListener
{
private final MpeGui mg;
private final ReportParseKeys rpk;
private boolean detectedUp = false;
private boolean detectedBound = false;

private final CopyOnWriteArrayList<NumericEntry>
    dataArray = new CopyOnWriteArrayList<>();

private final JTextArea mainText;
private final BenchmarkTableModel mainTable;
private final JProgressBar mainProgress;
private final ActionDraw openableDraw;
private final ActionStatistics openableStatistics;
private final ActionTextLog openableText;
private final StatisticsTableModel openableTable;
private final DrawModelInterface drawModel;
private final DrawViewInterface drawView;

private int previous = 0;

public ReportToGuiListener( MpeGui x )
    {
    mg = x;
    rpk = new ReportParseKeys();
    mainText = mg.getTextArea();
    mainTable = mg.getTableModel();
    mainProgress = mg.getProgressBar();
    openableDraw = mg.getChildDraw();
    openableStatistics = mg.getChildStatistics();
    openableText = mg.getChildTextLog();
    openableTable = openableStatistics.getTableModel();
    drawModel = openableDraw.getController().getModel();
    drawView =  openableDraw.getController().getView();
    }
    
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
                {
                // update text logs (main and openable windows), simple text
                String s = "\r\n" + reportStrings.get( i );
                mainText.append( s );
                openableText.write( s );
                // Get and parse array of words
                String[] words = reportStrings.get( i ).trim().split( "\\s+" );
                if ( words != null )
                    {
                    if ( rpk.detectBenchmarkTableUp( words ) )
                        {
                        detectedUp = true;
                        }
                    else if ( detectedUp && 
                            ( rpk.detectTableBoundLine( words ) ) )
                        {
                        if ( ! ( detectedBound = ! detectedBound ) )
                            detectedUp = false;
                        }
                    else if ( detectedUp )
                        {
                        NumericEntry entry = rpk.detectTableEntry( words );
                        if ( ( entry != null )             && 
                             ( entry.doubles != null )     && 
                             ( entry.bigdecs != null )     &&
                             ( entry.doubles.length >= 4 ) &&
                             ( entry.bigdecs.length >= 4 )  )
                            {
                            dataArray.add( entry );
                            EntryStatistics ests = 
                                StatisticUtil.buildStatisticsTable( dataArray );
                            ests.dataArray = dataArray;
                            
                            // update statistic table at main window
                            mainTable.measurementNotify( ests );
                            // update statistic table, openable window
                            openableTable.measurementNotify( ests );
                            
                            // update drawings Y=F(X), openable window
                            BigDecimal[] point = new BigDecimal[2];
                            point[0] = entry.bigdecs[0].
                                divideToIntegralValue( new BigDecimal (1024) );
                            point[1] = entry.bigdecs[3];
                            drawModel.updateValue( point );
                            drawModel.rescaleYmax();
                            drawView.getPanel().repaint();
                            }
                        }
                    }
                }
            previous = n;
            }
        }
    }
    
}
