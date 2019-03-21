/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Descriptors of GUI objects, edit logic implementation,
 * also provides command line builder for native application run.
 * This class used by MpeGui,
 * scenario for GUI mode, with global variables interconnect.
 *
 */

package mpeshell;

import java.awt.Dimension;
import java.awt.event.ActionEvent;
import javax.swing.DefaultBoundedRangeModel;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import javax.swing.JProgressBar;
import javax.swing.table.AbstractTableModel;
import mpeshell.MpeGuiList.MeasurementModes;
import mpeshell.service.About;
import mpeshell.service.ActionAbout;
import mpeshell.service.ActionGraph;
import mpeshell.service.ActionLoad;
import mpeshell.service.ActionReport;
import mpeshell.taskmonitor.ActionRun;

public class MpeGuiList 
{
private final MpeGui mg;
public MpeGui getMpeGui() { return mg; }  // provides MpeGui for scenarios
        
public MpeGuiList( MpeGui x )
    {
    mg = x;
    }

// this method implements combo boxes logic, some boxes depends on other.
// id = combo box number, e = combo box click event
protected void changesMonitor( int id, ActionEvent e )
    {
    JComboBox[] c = mg.getCombos();
    DescriptCombo[] dc = getDescriptCombos();
    int n = c.length;
    int m = c[id].getSelectedIndex();
    switch( id )
        {
        case 0:                                         // Application (32/64)
            (( ComboAsm )( dc[4] )).setMode64( m == 1 );
            updateHelper( c[4], dc[4] );
            break;
        case 1:                                         // Action
            boolean b = ( m == 2 );
            for( int i=2; i<n; i++ )
                {
                boolean b1 = c[i].isEnabled();
                int k = dc[i].getPrevious();
                ( dc[i] ).setAvailable( b );
                updateHelper( c[i], dc[i], ( b & dc[i].getFunctional() ) );
                boolean b2 = c[i].isEnabled();
                if (( ! b1 ) && ( b2 ))
                    {
                    if ( k >= 0 )
                        c[i].setSelectedIndex( k );
                    }
                }
            break;
        case 2:                                         // Test object
            break;
        case 3:                                         // Memory
            if ( ( m >= 0 ) && ( m != 5 ) )
                {
                c[11].setSelectedIndex( 0 );
                c[12].setSelectedIndex( 0 );
                c[13].setSelectedIndex( 0 );
                }
            if ( ( m >= 0 ) && ( m == 5 ) )
                {
                if ( c[11].getSelectedIndex() == 0 )
                    c[11].setSelectedIndex( 4 );
                if ( c[12].getSelectedIndex() == 0 )
                    c[12].setSelectedIndex( 12 );
                if ( c[13].getSelectedIndex() == 0 )
                    c[13].setSelectedIndex( 3 );
                }
            break;
        case 4:                                         // Assembler
            break;
        case 5:                                         // NUMA
            break;
        case 6:                                         // Page
            break;
        case 7:                                         // Threads
            break;
        case 8:                                         // Hyper Threading
            break;
        case 9:                                         // Repeats
            if ( m > 0 )
                c[10].setSelectedIndex( 0 );
            break;
        case 10:                                        // Adaptive
            if ( m > 0 )
                c[9].setSelectedIndex( 0 );
            break;
        case 11:                                        // Block start
            if ( m > 0 )
                {
                c[3].setSelectedIndex( 5 );
                }
            break;
        case 12:                                        // Block end
            if ( m > 0 )
                {
                c[3].setSelectedIndex( 5 );
                }
            break;
        case 13:                                        // Block step
            if ( m > 0 )
                {
                c[3].setSelectedIndex( 5 );
                }
            break;
        default:
            break;
        }
    // save states for restore after return from "n/a" state
    if ( c[id].isEnabled() )
        {
        int k = c[id].getSelectedIndex();
        if ( k >= 0 )
            dc[id].setPrevious( k );
        }
    }

// helpers methods

private void updateHelper( JComboBox c, DescriptCombo dc )
    {
    String[] s = dc.getValues();
    int n = s.length;
    c.removeAllItems();
    for( int i=0; i<n; i++ )
        c.addItem( s[i] );
    }

private void updateHelper( JComboBox c, DescriptCombo dc, boolean b )
    {
    c.setEnabled( b );
    updateHelper( c, dc );
    }

// Support progress indicator = f( native mode step count)
private final static int DEFAULT_PROGRESS_COUNT = 16;
public int getProgressCount()
    {
    long start = stepsHelper( 11 );
    long stop = stepsHelper( 12 );
    long step = stepsHelper( 13 );
    if ( ( start > 0 )&&( stop > 0 )&&( step > 0 ) )
        {
        long x = ( stop - start ) / step;
        if ( ( x > 0 )&& ( x < Integer.MAX_VALUE ) )
            return (int)x;
        else
            return DEFAULT_PROGRESS_COUNT;
        }
    else
        return DEFAULT_PROGRESS_COUNT;
    }

private long stepsHelper( int id )
    {
    JComboBox cb = mg.getCombos()[id];
    DescriptCombo dc = getDescriptCombos()[id];
    int index = cb.getSelectedIndex();
    if ( index <= 0 )
        return -1;
    index--;
    return dc.getLongValues()[index];
    }

// Fields for extracts parameters for native application run
private final static String PREFIX = "out=file";
private int optionWidth;
private String optionString;
private UnitsModes blockSizeUnits;
private MeasurementModes bandwidthOrLatency;

public int getOptionWidth()     { return optionWidth;  }  // get mode 32/64
public String getOptionString() { return optionString; }  // get options

public enum UnitsModes { KILOBYTES, MEGABYTES, UNKNOWN };
public enum MeasurementModes { BANDWIDTH, LATENCY, UNKNOWN };

public UnitsModes getBlockSizeUnits()
    { return blockSizeUnits; }                            // get units KB/MB
public MeasurementModes getBandwidthOrLatency() 
    { return bandwidthOrLatency; }                        // get mode bw/lat

// extracts parameters for native application run,
// this parameters = f ( GUI editable components state ).
public void extractParmsFromGui()
    {
    // defaults, width = 32 bit
    optionWidth = 0;
    optionString = null;
    
    // get detectable system information class
    SystemInformation sysinfo = mg.getSysInfo();
        
    // determine strings for native application command line options
    JComboBox[] c = mg.getCombos();
    DescriptCombo[] dc = getDescriptCombos();
    optionWidth = c[0].getSelectedIndex();       // Combo = Application 32/64
    String runmode  = " " + dc[1].getCmdValues()[ c[1].getSelectedIndex() ];
    String memtype  = helperOption( c, dc, 3  );  // Combo = Memory type
    String asminstr = helperOption( c, dc, 4  );  // Combo = Asm.CPU instr.
    String numaopt  = helperOption( c, dc, 5  );  // Combo = NUMA
    String pageopt  = helperOption( c, dc, 6  );  // Combo = Pages
    String threads  = helperOption( c, dc, 7  );  // Combo = Threads
    String htopt    = helperOption( c, dc, 8  );  // Combo = Hyper Threading
    String repeats  = helperOption( c, dc, 9  );  // Combo = Repeats
    String adaptive = helperOption( c, dc, 10 );  // Combo = Adaptive
    
    // determine start, stop, step values for benchmarks measurements
    int k1 = c[11].getSelectedIndex();  // start
    int k2 = c[12].getSelectedIndex();  // end
    int k3 = c[13].getSelectedIndex();  // step
    String start = "";
    String end = "";
    String step = "";
    
    if ( ( k1 > 0 ) && ( k2 > 0 ) && ( k3 > 0 ) )
        {
        memtype = "";
        start = helperOption( c, dc, 11  );  // Combo = start
        end   = helperOption( c, dc, 12  );  // Combo = end
        step  = helperOption( c, dc, 13  );  // Combo = step
        }
    
    // support X-axis: select X scale
    int k4 = c[3].getSelectedIndex();
    int scale;
    switch( k4 )
        {
        case 0:
            scale = sysinfo.getScale1();
            break;
        case 1:
            scale = sysinfo.getScale2();
            break;
        case 2:
            scale = sysinfo.getScale3();
            break;
        case 3:
            scale = sysinfo.getScale4();
            break;
        case 4:
            scale = sysinfo.getScaleDram();
            break;
        case 5:
            long[] arraySizes = dc[12].getLongValues();
            int arrayIndex = k2 - 1;
            if ( ( arrayIndex >= 0 ) && ( arrayIndex < arraySizes.length ) )
                {
                long selectedSize = arraySizes[ arrayIndex ];
                scale = ( int )( selectedSize / 1024 );
                
                if ( scale < 40 )
                    scale = 40;
                else if ( scale < 80 )
                    scale = 80;
                else if ( scale < 200 )
                    scale = 200;
                else if ( scale % 400 == 0 )
                    scale = scale / 400 * 400;
                else
                    scale = scale / 400 * 400 + 400;
                
                }
            else
                {
                scale = 80;
                }
            break;
        default:
            scale = 80;
        }
    
    // support X-axis: select KB or MB units = f ( selected scale )
    // YET LOCKED, CAREFULLY CHANGE BECAUSE PRECISION BUG, VERIFY FOR ALL
    // PARAMETERS: START, END, STEP.
    // if ( scale < 32*1024 )
    //     {
        blockSizeUnits = UnitsModes.KILOBYTES;
    //     }
    // else
    //     {
    //     blockSizeUnits = UnitsModes.MEGABYTES;
    //     scale /= ( 400 * 3 );
    //     }
    mg.getChildDraw().getController().getModel().setModeX( blockSizeUnits );

    // select X-scale
    mg.getChildDraw().getController().getModel().reset();
    mg.getChildDraw().getController().getModel().rescaleXmax( scale );
    
    // support Y-axis: select bandwidth or latency = f ( asm combo box )
    k4 = c[4].getSelectedIndex();
    bandwidthOrLatency = (( ComboAsm ) dc[4] ).askMode( k4 );
    mg.getChildDraw().getController().getModel().setModeY( bandwidthOrLatency );
    
    // build command line options for native application
    optionString = PREFIX + runmode + memtype + asminstr + numaopt + pageopt +
                            threads + htopt + repeats + adaptive +
                            start + end + step;
    }

private String helperOption( JComboBox[] c, DescriptCombo[] dc, int n )
    {
    int index = c[n].getSelectedIndex();
    String s1 = dc[n].getCmdName();
    String s2 = dc[n].getCmdValues()[index];
    String s = "";
    if ( ( s1 != null ) && ( s2 != null ) )
        {
        s = " " + s1 + "=" + s2;
        }
    return s;
    }

// lists of groups of main window GUI elements

public DescriptButton[] getDescriptButtons() { return LIST_BUTTONS; }
private final DescriptButton[] LIST_BUTTONS = new DescriptButton[]
    { new ButtonOpenLog( this ),
      new ButtonOpenStatistics( this ),
      new ButtonOpenDraw( this ),
      new ButtonRun( this ),
      new ButtonAbout( this ),    // start down group
      new ButtonLoad( this ),
      new ButtonGraph( this ),
      new ButtonReport( this ),
      new ButtonDefaults( this ),
      new ButtonClear( this ),
      new ButtonCancel( this ) };

public DescriptLabelConst[] getDescriptLabels() { return LIST_LABELS; }
private final DescriptLabelConst[] LIST_LABELS = new DescriptLabelConst[]
    { new LabelApplication(),  // start left group
      new LabelAction(),
      new LabelTest(),
      new LabelMemory(),
      new LabelAsm(),
      new LabelNuma(),
      new LabelPage(),
      new LabelThreads(),      // start right group
      new LabelHt(),
      new LabelRepeats(),
      new LabelAdaptive(),
      new LabelStart(),
      new LabelEnd(),
      new LabelStep() };

public DescriptCombo[] getDescriptCombos() { return LIST_COMBOS; }
private final DescriptCombo[] LIST_COMBOS = new DescriptCombo[]
    { new ComboApplication( this ),     // #0   // start left group
      new ComboAction( this ),          // #1
      new ComboTest( this ),            // #2
      new ComboMemory( this ),          // #3
      new ComboAsm( this ),             // #4
      new ComboNuma( this ),            // #5
      new ComboPage( this ),            // #6
      new ComboThreads( this ),         // #7   // right group
      new ComboHt( this ),              // #8
      new ComboRepeats( this ),         // #9
      new ComboAdaptive( this ),        // #10
      new ComboStart( this ),           // #11
      new ComboEnd( this ),             // #12
      new ComboStep( this ) };          // #13 
}

// buttons descriptors

class ButtonOpenLog extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonOpenLog( MpeGuiList x )  { mglst = x; }
@Override public String getName() { return "Log >"; }
@Override public String getText() { return "open log text window"; }
@Override public Dimension getSize() { return new Dimension( 102, 25 ); }
@Override public void actionPerformed( ActionEvent e )
    { mglst.getMpeGui().getChildTextLog().openWindow(); }
}

class ButtonOpenStatistics extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonOpenStatistics( MpeGuiList x ) { mglst = x; }
@Override public String getName() { return "Statistics >"; }
@Override public String getText() { return "open statistics table window"; }
@Override public Dimension getSize() { return new Dimension( 102, 25 ); }
@Override public void actionPerformed( ActionEvent e )
    { mglst.getMpeGui().getChildStatistics().openWindow(); }
}

class ButtonOpenDraw extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonOpenDraw( MpeGuiList x ) { mglst = x; }
@Override public String getName() { return "Draw >"; }
@Override public String getText() { return "open draw window"; }
@Override public Dimension getSize() { return new Dimension( 102, 25 ); }
@Override public void actionPerformed( ActionEvent e )
    { mglst.getMpeGui().getChildDraw().openWindow(); }
}

class ButtonRun extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonRun( MpeGuiList x ) { mglst = x; }

private boolean runStop = true;
public void setRunStop( boolean b ) { runStop = b; }
@Override public String getName() 
    { 
    String s = runStop ? "Run" : "Stop";
    return s;
    }

@Override public String getText() { return "run/stop benchmarks"; }
@Override public Dimension getSize() { return new Dimension( 78, 24 ); }

@Override public void actionPerformed( ActionEvent e )
    {
    if ( runStop )
        {
        mglst.getMpeGui().getChildStatistics().blankTable( 0 );
        mglst.getMpeGui().getChildDraw().getController().reset();
        BenchmarkAction benchmarkAction = new BenchmarkAction();
        Thread benchmarkThread = new Thread ( benchmarkAction );
        benchmarkThread.start();
        }
    else
        {
        mglst.getMpeGui().getTaskShell().interruptBenchmark();
        }
    }
// this nested class running benchmark task at parallel thread
private class BenchmarkAction implements Runnable
    {
    @Override public void run()
        {
        mglst.getMpeGui().disableGuiBeforeRun();
        mglst.extractParmsFromGui();
        mglst.getMpeGui().getTaskShell().runBenchmark();
        mglst.getMpeGui().enableGuiAfterRun();
        }
    }
}

class ButtonAbout extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonAbout( MpeGuiList x ) { mglst = x; }
@Override public String getName() { return "About"; }
@Override public String getText() { return "information about application"; }
@Override public Dimension getSize() { return new Dimension( 80, 24 ); }
@Override public void actionPerformed( ActionEvent e )
    {
    final ActionAbout a = new ActionAbout();
    final JFrame mainFrame = mglst.getMpeGui().getFrame();
    final JDialog dialog = a.createDialog
        ( mainFrame ,
        About.getLongName() , About.getVendorName() );
        dialog.setLocationRelativeTo( mainFrame );
        dialog.setVisible( true );
    }
}

class ButtonLoad extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonLoad( MpeGuiList x ) { mglst = x; }
@Override public String getName() { return "Load"; }
@Override public String getText() { return "load previous results"; }
@Override public Dimension getSize() { return new Dimension( 80, 24 ); }
@Override public void actionPerformed( ActionEvent e )
    {
    final ActionLoad a = new ActionLoad( mglst );
    a.loadReportDialogue();
    }
}

class ButtonGraph extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonGraph( MpeGuiList x ) { mglst = x; }
@Override public String getName() { return "Graph"; }
@Override public String getText() { return "save graphics image"; }
@Override public Dimension getSize() { return new Dimension( 80, 24 ); }
@Override public void actionPerformed( ActionEvent e )
    {
    final ActionGraph a = new ActionGraph( mglst.getMpeGui() );
    a.saveGraphicsDialogue();
    }
}

class ButtonReport extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonReport( MpeGuiList x ) { mglst = x; }
@Override public String getName() { return "Report"; }
@Override public String getText() { return "save text report"; }
@Override public Dimension getSize() { return new Dimension( 80, 24 ); }
@Override public void actionPerformed( ActionEvent e )
    {
    final ActionReport a = new ActionReport( mglst.getMpeGui() );
    a.saveReportDialogue();
    }
}

class ButtonDefaults extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonDefaults( MpeGuiList x ) { mglst = x; }
@Override public String getName() { return "Defaults"; }
@Override public String getText() { return "restore options defaults"; }
@Override public Dimension getSize() { return new Dimension( 83, 24 ); }
@Override public void actionPerformed( ActionEvent e )
    {
    JComboBox[] c = mglst.getMpeGui().getCombos();
    DescriptCombo[] dc = mglst.getDescriptCombos();
    for( int i=0; i<c.length; i++ )
        {
        int k = dc[i].getDefaultSelection();
        if ( k >= 0 )
            c[i].setSelectedIndex( k );
        else
            c[i].setSelectedIndex( 0 );
        c[i].setEnabled( dc[i].getDefaultEnable() );
        }
    }
}

class ButtonClear extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonClear( MpeGuiList x ) { mglst = x; }
@Override public String getName() { return "Clear"; }
@Override public String getText() { return "clear logs and statistics"; }
@Override public Dimension getSize() { return new Dimension( 80, 24 ); }
@Override public void actionPerformed( ActionEvent e )
    {
    String s1 = "log cleared by user.";
    mglst.getMpeGui().getTextArea().setText( s1 );
    mglst.getMpeGui().getChildTextLog().overWrite( s1 );
    AbstractTableModel atm = mglst.getMpeGui().getTableModel();
    int rc = atm.getRowCount();
    int cc = atm.getColumnCount();
    for( int i=0; i<rc; i++ )
        {
        for( int j=1; j<cc; j++ )
            {
            atm.setValueAt( "-", i, j );
            }
        }
    atm.fireTableDataChanged();
    mglst.getMpeGui().getChildStatistics().blankTable( 0 );
    mglst.getMpeGui().getChildDraw().getController().reset();
    // set progress indicator to 0 percents
    JProgressBar progressBar = mglst.getMpeGui().getProgressBar();
    DefaultBoundedRangeModel progressModel = ( DefaultBoundedRangeModel )
        progressBar.getModel();
    ActionRun ar = mglst.getMpeGui().getTaskShell();
    ar.progressUpdate( progressModel, progressBar, 0 );
    }
}

class ButtonCancel extends DescriptButton {
private final MpeGuiList mglst;
protected ButtonCancel( MpeGuiList x ) { mglst = x; }
@Override public String getName() { return "Cancel"; }
@Override public String getText() { return "exit application"; }
@Override public Dimension getSize() { return new Dimension( 81, 24 ); }
@Override public void actionPerformed( ActionEvent e )
    {
    mglst.getMpeGui().invokeCloseSession();
    System.exit( 0 );
    }
}

// labels (constant text) descriptors

class LabelApplication extends DescriptLabelConst {
@Override public String getName() { return "Application"; }
@Override public String getText() 
    { return "native application 32 or 64 bit"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelAction extends DescriptLabelConst {
@Override public String getName() { return "Action"; }
@Override public String getText() 
    { return "select help, sysinfo or benchmarks"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelTest extends DescriptLabelConst {
@Override public String getName() { return "Test object"; }
@Override public String getText() 
    { return "select memory or storage test"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelMemory extends DescriptLabelConst {
@Override public String getName() { return "Memory"; }
@Override public String getText() 
    { return "select memory. L1-L4, DRAM"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelAsm extends DescriptLabelConst {
@Override public String getName() { return "Assembler"; }
@Override public String getText() 
    { return "select CPU instruction set"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelNuma extends DescriptLabelConst {
@Override public String getName() { return "NUMA"; }
@Override public String getText() 
    { return "set NUMA optimization option"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelPage extends DescriptLabelConst {
@Override public String getName() { return "Page"; }
@Override public String getText() 
    { return "set paging option, normal or large"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelThreads extends DescriptLabelConst {
@Override public String getName() { return "Threads"; }
@Override public String getText() 
    { return "set number of threads for benchmark"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelHt extends DescriptLabelConst {
@Override public String getName() { return "HT"; }
@Override public String getText() 
    { return "set hyper-threading option"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelRepeats extends DescriptLabelConst {
@Override public String getName() { return "Repeats"; }
@Override public String getText() 
    { return "set benchmarks measurement repeats"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelAdaptive extends DescriptLabelConst {
@Override public String getName() { return "Adaptive"; }
@Override public String getText() 
    { return "set adaptive measurement repeats"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelStart extends DescriptLabelConst {
@Override public String getName() { return "Block start"; }
@Override public String getText() 
    { return "set start block size"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelEnd extends DescriptLabelConst {
@Override public String getName() { return "Block end"; }
@Override public String getText() 
    { return "set end block size"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

class LabelStep extends DescriptLabelConst {
@Override public String getName() { return "Block step"; }
@Override public String getText() 
    { return "set block size step"; }
@Override public Dimension getSize() { return new Dimension( 72, 21 ); }
}

// combo boxes descriptors

// combo descriptor for select native application. This selection must make
// additional updates: assembler option, operations different OP32/OP64
class ComboApplication extends DescriptCombo {
protected ComboApplication( MpeGuiList x ) { super( x, 0 ); }
@Override public String[] getValues()
    { 
    return new String[] 
        // { "Windows ia32", "Windows x64", "Linux ia32" , "Linux x64" };
        { "Windows ia32", "Windows x64" }; 
    }
@Override public String getText()      
    { return "native application 32 or 64 bit"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 245, 21 ); }
@Override public int getDefaultSelection() { return 1; }
@Override public String getCmdName()     { return null; }
@Override public String[] getCmdValues() { return null; }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select operation type: User help, System Information,
// Benchmarks. This selection must make additional updates: all options exclude
// "Application" and "Action" make gray and n/a.
class ComboAction extends DescriptCombo {
private final static String[] MODES =
    { "help=full", "info=all", "test=memory" };  // as irregular cases
protected ComboAction( MpeGuiList x ) { super( x, 1 ); }
@Override public String[] getValues()
    { 
    return new String[] 
        { "Help", "System information", "Benchmarks" }; 
    }
@Override public String getText()      
    { return "select help, sysinfo or benchmarks"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 245, 21 ); }
@Override public int getDefaultSelection() { return 2; }
@Override public String getCmdName() { return null;  }      // irregular cases
@Override public String[] getCmdValues() { return MODES; }  // irregular cases
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select tested storage object: memory or mass storage.
// This selection is under construction. Yet "memory" only.
class ComboTest extends DescriptCombo {
protected ComboTest( MpeGuiList x ) { super( x, 2 ); }
@Override public String[] getValues()
    { return getAvailable() ? new String[] { "Memory", "Storage" } : NA; }
@Override public String getText()      
    { return "select memory or storage test"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 245, 21 ); }
@Override public boolean getDefaultEnable() { return false; }
@Override public String getCmdName() { return null;  }     // yet reserved
@Override public String[] getCmdValues() { return null; }  // yet reserved
@Override public boolean getFunctional() { return false; }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select tested memory object, cache levels or DRAM.
// This selection must make additional updates: "assembler" and threads
// count options.
class ComboMemory extends DescriptCombo {
private boolean itemsAvailable = true;
protected boolean getItemsAvailable()          { return itemsAvailable;    }
protected void setItemsAvailable( boolean b )  { itemsAvailable = b;       }
private final static String MEMORY = "memory";
private final static String[] MEMTYPES = 
    { "l1", "l2", "l3", "l4", "dram", "custom"  };
protected ComboMemory( MpeGuiList x ) { super( x, 3 ); }
@Override public String[] getValues()
    { 
    return getAvailable() ? 
        new String[] 
        { "L1 cache", "L2 cache", "L3 cache", "L4 cache", "DRAM", 
          "Custom block" }
        : NA;
    }
@Override public String getText()      
    { 
    return "select memory. L1-L4, DRAM"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 245, 21 ); }
@Override public String getCmdName() { return MEMORY;  }
@Override public String[] getCmdValues() { return MEMTYPES; }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select assembler method. This selection without
// additional updates.
class ComboAsm extends DescriptCombo {
protected ComboAsm( MpeGuiList x ) { super( x, 4 ); }
private static final String[] ASM_NAMES_32 =
    { 
    "auto",                                // ID = -1
    "Read MOV32",                          // ID = 0
    "Write MOV32",
    "Copy MOV32",
    "Modify NOT32",
    "Write strings 32",
    "Copy strings 32",
    "Read MMX64",
    "Write MMX64",
    "Copy MMX64",
    "Read SSE128",
    "Write SSE128",
    "Copy SSE128",
    "Read AVX256",
    "Write AVX256",
    "Copy AVX256",
    "Read AVX512",
    "Write AVX512",
    "Copy AVX512",
    "Dot product FMA256",
    "Dot product FMA512",                  // ID = 19
    "Write SSE128 NT",                     // ID = 20
    "Copy SSE128 NT write",
    "Write AVX256 NT",
    "Copy AVX256 NT write",
    "Write AVX512 NT",
    "Copy AVX512 NT write",                // ID = 25
    "Read SSE128 NT",                      // ID = 26
    "Copy SSE128 NT read+write",
    "Read AVX256 NT",
    "Copy AVX256 NT read+write",
    "Read AVX512 NT",
    "Copy AVX512 NT read+write",           // ID = 31
    "Read SSE128 NT prefetch",             // ID = 32
    "Copy SSE128 NT prefetch NT write",    // ID = 33
    "Read AVX256 NT prefetch",             // ID = 34
    "Latency LCM",                         // ID = 35
    "Latency RDRAND"                       // ID = 36
    }; 
private static final String[] ASM_NAMES_64 =
    {
    ASM_NAMES_32[0],                       // ID = -1
    "Read MOV64",                          // ID = 0
    "Write MOV64",
    "Copy MOV64",
    "Modify NOT64",
    "Write strings 64",
    "Copy strings 64",
    ASM_NAMES_32[7],
    ASM_NAMES_32[8],
    ASM_NAMES_32[9],
    ASM_NAMES_32[10],
    ASM_NAMES_32[11],
    ASM_NAMES_32[12],
    ASM_NAMES_32[13],
    ASM_NAMES_32[14],
    ASM_NAMES_32[15],
    ASM_NAMES_32[16],
    ASM_NAMES_32[17],
    ASM_NAMES_32[18],
    ASM_NAMES_32[19],
    ASM_NAMES_32[20],
    ASM_NAMES_32[21],
    ASM_NAMES_32[22],
    ASM_NAMES_32[23],
    ASM_NAMES_32[24],
    ASM_NAMES_32[25],
    ASM_NAMES_32[26],
    ASM_NAMES_32[27],
    ASM_NAMES_32[28],
    ASM_NAMES_32[29],
    ASM_NAMES_32[30],
    ASM_NAMES_32[31],
    ASM_NAMES_32[32],
    ASM_NAMES_32[33],
    ASM_NAMES_32[34],
    ASM_NAMES_32[35],
    ASM_NAMES_32[36],    // ID = 35
    ASM_NAMES_32[37]     // ID = 36
    };
private final static String ASM = "asm";
private final static String[] ASM_32 =
    { null,
      "readmov32", "writemov32", "copymov32", "modifynot32",
      "writestring32", "copystring32", 
      "readmmx64", "writemmx64", "copymmx64",
      "readsse128", "writesse128", "copysse128",
      "readavx256", "writeavx256", "copyavx256",
      "readavx512", "writeavx512", "copyavx512",
      "dotfma256",  "dotfma512",
      "ntwwritesse128", "ntwcopysse128",
      "ntwwriteavx256", "ntwcopyavx256",
      "ntwwriteavx512", "ntwcopyavx512",
      "ntrwreadsse128", "ntrwcopysse128",
      "ntrwreadavx256", "ntrwcopyavx256",
      "ntrwreadavx512", "ntrwcopyavx512",
      "ntprwreadsse128", "ntprwcopysse128",
      "ntprwreadavx256",
      "latencylcm", "latencyrdrand" };
private final static String[] ASM_64 =
    { null,
      "readmov64", "writemov64", "copymov64", "modifynot64",
      "writestring64", "copystring64",
      ASM_32[7],   ASM_32[8],   ASM_32[9],
      ASM_32[10],  ASM_32[11],  ASM_32[12],
      ASM_32[13],  ASM_32[14],  ASM_32[15],
      ASM_32[16],  ASM_32[17],  ASM_32[18],
      ASM_32[19],  ASM_32[20],
      ASM_32[21],  ASM_32[22],
      ASM_32[23],  ASM_32[24],
      ASM_32[25],  ASM_32[26],
      ASM_32[27],  ASM_32[28],
      ASM_32[29],  ASM_32[30],
      ASM_32[31],  ASM_32[32],
      ASM_32[33],  ASM_32[34],
      ASM_32[35],
      ASM_32[36],  ASM_32[37] };

protected MeasurementModes askMode( int num )
    {
    MeasurementModes x = MeasurementModes.UNKNOWN;
    if ( num <= 35 )
        x = MeasurementModes.BANDWIDTH;
    else if ( num <= 37 )
        x = MeasurementModes.LATENCY;
    return x;
    }

private boolean mode64 = true;
private long bitmapAsm = -1;
protected boolean getMode64()         { return mode64;    }
protected void setMode64( boolean b ) { mode64 = b;       }
protected long getBitmapAsm()         { return bitmapAsm; }
protected void setBitmapAsm( long x ) { bitmapAsm = x;    }
@Override public String[] getValues()
    { 
    String[] s;
    s = mode64 ? ASM_NAMES_64 : ASM_NAMES_32;
    int n = s.length;
    long bitmask = 1;
    for( int i=0; i<n; i++ )
        {
        if ( ( bitmapAsm & bitmask ) == 0 )
            s[i] = GRAY_PREFIX + s[i];
        bitmask = bitmask << 1;
        }
    return getAvailable() ? s : NA;
    }
@Override public String getText()      
    { return "select CPU instruction set"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 245, 21 ); }
@Override public String getCmdName() { return ASM;  }
@Override public String[] getCmdValues() { return mode64 ? ASM_64 : ASM_32; }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select NUMA optimization mode.
class ComboNuma extends DescriptCombo {
protected ComboNuma( MpeGuiList x ) { super( x, 5 ); }
private final static String NUMA = "numa";
private final static String[] NUMA_NAMES = { "unaware", "local", "remote" };
private int bitmapNuma = -1;
protected long getBitmapNuma()        { return bitmapNuma; }
protected void setBitmapNuma( int x ) { bitmapNuma = x;    }
@Override public String[] getValues()
    { 
    String[] s = NUMA_NAMES;
    int n = s.length;
    int bitmask = 1;
    for( int i=0; i<n; i++ )
        {
        if ( ( bitmapNuma & bitmask ) == 0 )
            s[i] = GRAY_PREFIX + s[i];
        }
    return getAvailable() ? s : NA;
    }
@Override public String getText()      
    { return "set NUMA optimization option"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 245, 21 ); }
@Override public String getCmdName() { return NUMA;  }
@Override public String[] getCmdValues() { return NUMA_NAMES; }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select memory page size at memory allocation OS API.
class ComboPage extends DescriptCombo {
protected ComboPage( MpeGuiList x ) { super( x, 6 ); }
private final static String PAGE = "page";
private final static String[] PAGE_NAMES = { "default", "large" };
private int bitmapPage = -1;
protected long getBitmapPage()        { return bitmapPage; }
protected void setBitmapPage( int x ) { bitmapPage = x;    }
@Override public String[] getValues()
    {
    String[] s = PAGE_NAMES;
    int n = s.length;
    int bitmask = 1;
    for( int i=0; i<n; i++ )
        {
        if ( ( bitmapPage & bitmask ) == 0 )
            s[i] = GRAY_PREFIX + s[i];
        }
    return getAvailable() ? s : NA;
    }
@Override public String getText()      
    { return "set paging option, normal or large"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 245, 21 ); }
@Override public String getCmdName() { return PAGE;  }
@Override public String[] getCmdValues() { return PAGE_NAMES; }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select number of threads.
class ComboThreads extends DescriptCombo {
protected ComboThreads( MpeGuiList x ) 
    {
    super( x, 7 ); 
    textValues = buildNumbersOptions( numericValues );
    }
private final static String THREADS = "threads";
private int[] numericValues = { 1, 2, 4, 8, 64 };
private String[] textValues;
@Override public int[] getIntValues() { return numericValues; }
protected void setNumericValues( int[] x )   
    {
    numericValues = x;
    textValues = buildNumbersOptions( numericValues );
    }
@Override public String[] getValues()  
    { return getAvailable() ? textValues : NA; }
@Override public String getText()      
    { return "set number of threads for benchmark"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 152, 21 ); }
@Override public String getCmdName() { return THREADS;  }
@Override public String[] getCmdValues() 
    { 
    String[] s1 = getValues();
    String[] s2 = new String[ s1.length ];
    System.arraycopy( s1, 0, s2, 0, s1.length );
    s2[0] = null;
    return s2;
    }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select Hyper-Threading or SMT option
class ComboHt extends DescriptCombo {
protected ComboHt( MpeGuiList x ) { super( x, 8 ); }
private final static String HT = "ht";
private final static String[] HT_NAMES = 
    { "no control", "disabled", "enabled" };
private final static String[] HT_CMDS  = 
    { null, "off", "on" };
private int bitmapHt = -1;
protected long getBitmapHt()        { return bitmapHt; }
protected void setBitmapHt( int x ) { bitmapHt = x;    }
@Override public String[] getValues()
    {
    String[] s = HT_NAMES;
    int n = s.length;
    int bitmask = 1;
    for( int i=0; i<n; i++ )
        {
        if ( ( bitmapHt & bitmask ) == 0 )
            s[i] = GRAY_PREFIX + s[i];
        }
    return getAvailable() ? s : NA;
    }
@Override public String getText()      
    { return "set hyper-threading option"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 152, 21 ); }
@Override public String getCmdName() { return HT;  }
@Override public String[] getCmdValues() { return HT_CMDS; }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select measurement repeats count at non-adaptive mode.
class ComboRepeats extends DescriptCombo {
protected ComboRepeats( MpeGuiList x ) 
    {
    super( x, 9 ); 
    textValues = buildNumbersOptions( numericValues );
    }
private final static String REPEATS = "repeats";
private int[] numericValues = { 2000000, 500000, 10000, 1000, 200, 20 };
private String[] textValues;
@Override public int[] getIntValues() { return numericValues; }
protected void setNumericValues( int[] x )    
    {
    numericValues = x;    
    textValues = buildNumbersOptions( numericValues );
    }
@Override public String[] getValues()
    { return getAvailable() ? textValues : NA; }
@Override public String getText()      
    { return "set benchmarks measurement repeats"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 152, 21 ); }
@Override public String getCmdName() { return REPEATS;  }
@Override public String[] getCmdValues() 
    {
    String[] s1 = getValues();
    String[] s2 = new String[ s1.length ];
    System.arraycopy( s1, 0, s2, 0, s1.length );
    s2[0] = null;
    return s2;
    }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select adaptive measurement repeats, total size at
// formula: 
// total size = ( one measurement iteration size ) * ( iterations count )
class ComboAdaptive extends DescriptCombo {
protected ComboAdaptive( MpeGuiList x ) { super( x, 10 ); }
private final static String ADAPTIVE = "adaptive";
private String[] textValues = { "no control", "512M", "256M", "64M" };
protected void setTextValues( String[] s ) { textValues = s;    }
@Override public String[] getValues()   
    { return getAvailable() ? textValues : NA; }
@Override public String getText()      
    { return "set adaptive measurement repeats"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 152, 21 ); }
@Override public String getCmdName() { return ADAPTIVE;  }
@Override public String[] getCmdValues() 
    {
    String[] s1 = getValues();
    String[] s2 = new String[ s1.length ];
    System.arraycopy( s1, 0, s2, 0, s1.length );
    s2[0] = null;
    return s2;
    }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select block size START value at 
// function speed = f( block size ).
class ComboStart extends DescriptCombo {
protected ComboStart( MpeGuiList x ) 
    { 
    super( x, 11 ); 
    textValuesCombo = buildSizesOptions( 512, 22 );
    textValuesCmd = buildSizesOptions( 512, 22, false );
    longValues = buildRoundNumbersArray( 512, 22 );
    }
private final static String START = "start";
private final String[] textValuesCombo;
private final String[] textValuesCmd;

private final long[] longValues;
@Override public long[] getLongValues() { return longValues; }

@Override public String[] getValues()   
    { return getAvailable() ? textValuesCombo : NA; }
@Override public String getText()      
    { return "set start block size"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 152, 21 ); }
@Override public String getCmdName() { return START;  }
@Override public String[] getCmdValues() { return textValuesCmd; }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select block size END value at 
// function speed = f( block size ).
class ComboEnd extends DescriptCombo {
protected ComboEnd( MpeGuiList x ) 
    {
    super( x, 12 ); 
    textValuesCombo  = buildSizesOptions( 512, 22 );
    textValuesCmd = buildSizesOptions( 512, 22, false );
    longValues = buildRoundNumbersArray( 512, 22 );
    }
private final static String END = "end";
private final String[] textValuesCombo;
private final String[] textValuesCmd;

private final long[] longValues;
@Override public long[] getLongValues() { return longValues; }

@Override public String[] getValues()
    { return getAvailable() ? textValuesCombo : NA; }
@Override public String getText()      
    { return "set end block size"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 152, 21 ); }
@Override public String getCmdName() { return END;  }
@Override public String[] getCmdValues() { return textValuesCmd; }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}

// combo descriptor for select block size STEP value at 
// function speed = f( block size ).
class ComboStep extends DescriptCombo {
protected ComboStep( MpeGuiList x ) 
    {
    super( x, 13 ); 
    textValuesCombo = buildSizesOptions( 512, 22 );
    textValuesCmd = buildSizesOptions( 512, 22, false );
    longValues = buildRoundNumbersArray( 512, 22 );
    }
private final static String STEP = "step";
private final String[] textValuesCombo;
private final String[] textValuesCmd;

private final long[] longValues;
@Override public long[] getLongValues() { return longValues; }

@Override public String[] getValues()
    { return getAvailable() ? textValuesCombo : NA; }
@Override public String getText()      
    { return "set block size step"; }
@Override public int[] getKeys() { return null; }
@Override public Dimension getSize() { return new Dimension( 152, 21 ); }
@Override public String getCmdName() { return STEP;  }
@Override public String[] getCmdValues() { return textValuesCmd; }
@Override public void actionPerformed( ActionEvent e )
    { mglst.changesMonitor( id, e ); }
}
