/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Converts monitored data from report to application output,
 * this class used at GUI mode scenario.
 * 
 */

package mpeshell.taskmonitor;

import java.math.BigDecimal;
import java.util.concurrent.CopyOnWriteArrayList;
import javax.swing.DefaultBoundedRangeModel;
import javax.swing.JProgressBar;
import javax.swing.JTextArea;
import mpeshell.SysBenchmarkTableModel;
import mpeshell.MpeGui;
import mpeshell.MpeGuiList;
import mpeshell.MpeGuiList.MeasurementModes;
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
private final MpeGuiList mglst;
private final MpeGui mg;

private final ReportParseKeys rpk;
private boolean detectedUp = false;
private boolean detectedBound = false;

private final CopyOnWriteArrayList<NumericEntry>
    dataArray = new CopyOnWriteArrayList<>();

private final JTextArea mainText;
private final SysBenchmarkTableModel mainTable;
private final JProgressBar mainProgress;
private final DefaultBoundedRangeModel mainProgressModel;
private final ActionDraw openableDraw;
private final ActionStatistics openableStatistics;
private final ActionTextLog openableText;
private final StatisticsTableModel openableTable;
private final DrawModelInterface drawModel;
private final DrawViewInterface drawView;
private final ActionRun taskShell;

private double progressValue = 0.0;
private double progressDelta = 0.0;

private int previous = 0;

public ReportToGuiListener( MpeGuiList x )
    {
    // initializing GUI elements
    mglst = x;
    mg = mglst.getMpeGui();
    rpk = new ReportParseKeys();
    mainText = mg.getTextArea();
    mainTable = mg.getTableModel();
    mainProgress = mg.getProgressBar();
    mainProgressModel = ( DefaultBoundedRangeModel ) mainProgress.getModel();
    openableDraw = mg.getChildDraw();
    openableStatistics = mg.getChildStatistics();
    openableText = mg.getChildTextLog();
    openableTable = openableStatistics.getTableModel();
    drawModel = openableDraw.getController().getModel();
    drawView =  openableDraw.getController().getView();
    taskShell = mg.getTaskShell();
    // initializing progress indicator
    int p1 = taskShell.getProgress1();
    int p2 = taskShell.getProgress2();
    double pCount = mglst.getProgressCount();
    progressValue = p1;
    progressDelta = ( p2 - p1 - 3 ) / pCount; 
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
                        detectedUp = true;  // mark begin of results table
                        }
                    else if ( detectedUp && 
                            ( rpk.detectTableBoundLine( words ) ) )
                        {
                        if ( ! ( detectedBound = ! detectedBound ) )
                            detectedUp = false;  // mark end of results table
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
                            
                            MeasurementModes mm = 
                                mglst.getBandwidthOrLatency();
                            if ( mm == null )
                                point[1] = BigDecimal.ZERO;
                            else switch ( mm ) 
                                {
                                case BANDWIDTH:
                                    point[1] = entry.bigdecs[3];
                                    break;
                                case LATENCY:
                                    point[1] = entry.bigdecs[2];
                                    break;
                                default:
                                    point[1] = BigDecimal.ZERO;
                                    break;
                                }
                            
                            drawModel.updateValue( point );
                            drawModel.rescaleYmax();
                            drawView.getPanel().repaint();
                            
                            // update progress indicator
                            progressValue += progressDelta;
                            taskShell.progressUpdate
                                ( mainProgressModel, mainProgress, 
                                 (int)progressValue );
                            }
                        }
                    }
                }
            previous = n;
            }
        }
    }
    
}
