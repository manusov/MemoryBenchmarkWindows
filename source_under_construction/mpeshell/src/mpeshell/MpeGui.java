/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Scenario for GUI mode, with global variables interconnect.
 *
 */

package mpeshell;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.DefaultBoundedRangeModel;
import javax.swing.JButton;
import javax.swing.JComboBox;
import javax.swing.JDialog;
import javax.swing.JFrame;
import static javax.swing.JFrame.EXIT_ON_CLOSE;
import javax.swing.JLabel;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JProgressBar;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.JTextArea;
import javax.swing.SpringLayout;
import javax.swing.SwingConstants;
import javax.swing.table.DefaultTableCellRenderer;
import javax.swing.text.DefaultCaret;
import mpeshell.opendraw.ActionDraw;
import mpeshell.openlog.ActionTextLog;
import mpeshell.openstatistics.ActionStatistics;
import mpeshell.service.About;
import mpeshell.taskmonitor.ActionRun;
import mpeshell.taskmonitor.OpStatus;

public class MpeGui 
{
private JFrame frame = null;
private JPanel p = null;
private final Dimension SIZE_WINDOW = new Dimension( 590, 580 );
private ActionDraw childDraw = null;
private ActionStatistics childStatistics = null;
private ActionTextLog childTextLog = null;
private ActionRun taskShell = null;

public JFrame getFrame()                     { return frame;           }
public ActionDraw getChildDraw()             { return childDraw;       }
public ActionStatistics getChildStatistics() { return childStatistics; }
public ActionTextLog getChildTextLog()       { return childTextLog;    }
public ActionRun getTaskShell()              { return taskShell;       }

private JTable table = null;
private BenchmarkTableModel tableModel = null;
private JTextArea textArea = null;
private DefaultBoundedRangeModel progressModel = null;
private JProgressBar pb = null;

public JTable getTable()                    { return table;      }
public BenchmarkTableModel getTableModel()  { return tableModel; }
public JTextArea getTextArea()              { return textArea;   }
public DefaultBoundedRangeModel
        getProgressModel() { return progressModel; }
public JProgressBar
        getProgressBar() { return pb; }

private JButton[]   b = null;
private JLabel[]    a = null;
private JComboBox[] c = null;   // must be numerated [0-13] as fixed positions
public JComboBox[] getCombos()  { return c; }

private MpeGuiList mglst;
private CloseSessionListener csl;

public void runBenchmarkGui()
    {
    // initializing static GUI options
    JFrame.setDefaultLookAndFeelDecorated( true );
    JDialog.setDefaultLookAndFeelDecorated( true );
    
    // initializing main application GUI task-specific description
    mglst = new MpeGuiList( this );
    
    // initializing application frame, panel and layout manager
    frame = new JFrame( About.getShortName() );
    SpringLayout sl = new SpringLayout();        // components layout
    p = new JPanel( sl );                        // panel with layout

    // table
    tableModel = new BenchmarkTableModel();
    table = new JTable( tableModel );
    JScrollPane tbs = new JScrollPane( table );
    DefaultTableCellRenderer tr = new DefaultTableCellRenderer();
    tr.setHorizontalAlignment( SwingConstants.CENTER );
    for ( int i=0; i<table.getColumnCount(); i++ )
        table.getColumnModel().getColumn(i).setCellRenderer( tr );
    p.add( tbs );

    // text view
    textArea = new JTextArea();
    Font font = new Font( "monospaced", Font.PLAIN, 12 );
    textArea.setFont( font );
    textArea.setEditable( false );
    DefaultCaret caret = ( DefaultCaret )( textArea.getCaret() );
    caret.setUpdatePolicy( DefaultCaret.ALWAYS_UPDATE );
    String runstr = "starting...";
    textArea.append( runstr );
    JScrollPane txs = new JScrollPane( textArea );
    p.add( txs );
    
    // buttons
    DescriptButton[] descbut = mglst.getDescriptButtons();
    int n = descbut.length;
    b = new JButton[n];
    for( int i=0; i<n; i++ )
        {
        b[i] = new JButton( descbut[i].getName() );
        b[i].setPreferredSize( descbut[i].getSize() );
        b[i].setToolTipText( descbut[i].getText() );
        b[i].addActionListener( descbut[i] );
        p.add( b[i] );
        }

    // labels
    DescriptLabelConst[] desclab = mglst.getDescriptLabels();
    n = desclab.length;
    a = new JLabel[n];
    for( int i=0; i<n; i++ )
        {
        a[i] = new JLabel( desclab[i].getName() );
        a[i].setPreferredSize( desclab[i].getSize() );
        a[i].setToolTipText( desclab[i].getText() );
        p.add( a[i] );
        }
    
    // combos
    DescriptCombo[] desccom = mglst.getDescriptCombos();
    n = desccom.length;
    c = new JComboBox[n];
    for( int i=0; i<n; i++ )
        {
        c[i] = new JComboBox();
        c[i].setPreferredSize( desccom[i].getSize() );
        c[i].setToolTipText( desccom[i].getText() );
        String[] s = desccom[i].getValues();
        int m = s.length;
        for( int j=0; j<m; j++ )
            c[i].addItem( s[j] );
        int k1 = desccom[i].getDefaultSelection();
        if ( k1 >= 0 )
            c[i].setSelectedIndex( k1 );
        boolean k2 = desccom[i].getDefaultEnable();
        if ( ! k2 )
            c[i].setEnabled( false );
        c[i].addActionListener( desccom[i] );
        p.add( c[i] );
        }

    // progress bar
    progressModel = new DefaultBoundedRangeModel( 0, 0, 0, 100 );
    pb = new JProgressBar( progressModel );
    pb.setPreferredSize( new Dimension ( 189, 21 ) );
    pb.setStringPainted( true );
    pb.setString( "Please run..." );
    p.add( pb );

    // start layout setup, results table
    sl.putConstraint( SpringLayout.NORTH, tbs,  1,   SpringLayout.NORTH, p );
    sl.putConstraint( SpringLayout.SOUTH, tbs, -460, SpringLayout.SOUTH, p );
    sl.putConstraint( SpringLayout.WEST,  tbs,  1,   SpringLayout.WEST,  p );
    sl.putConstraint( SpringLayout.EAST,  tbs, -110, SpringLayout.EAST,  p );
    
    // text view
    sl.putConstraint( SpringLayout.NORTH, txs,  5,   SpringLayout.SOUTH, tbs );
    sl.putConstraint( SpringLayout.SOUTH, txs, -300, SpringLayout.SOUTH, p   );
    sl.putConstraint( SpringLayout.WEST,  txs,  2,   SpringLayout.WEST,  p   );
    sl.putConstraint( SpringLayout.EAST,  txs, -2,   SpringLayout.EAST,  p   );
    
    // progress bar
    sl.putConstraint( SpringLayout.SOUTH,  pb, -50,  SpringLayout.SOUTH, p );
    sl.putConstraint( SpringLayout.WEST,   pb, 155,  SpringLayout.WEST,  p );

    // up right buttons, buttons for open additional windows
    sl.putConstraint( SpringLayout.NORTH, b[0],  3, SpringLayout.NORTH, p    );
    sl.putConstraint( SpringLayout.EAST,  b[0], -3, SpringLayout.EAST,  p    );
    sl.putConstraint( SpringLayout.NORTH, b[1],  3, SpringLayout.SOUTH, b[0] );
    sl.putConstraint( SpringLayout.WEST,  b[1],  0, SpringLayout.WEST,  b[0] );
    sl.putConstraint( SpringLayout.NORTH, b[2],  3, SpringLayout.SOUTH, b[1] );
    sl.putConstraint( SpringLayout.WEST,  b[2],  0, SpringLayout.WEST,  b[1] );
    
    // run button
    sl.putConstraint( SpringLayout.NORTH, b[3], 0, SpringLayout.NORTH, pb );
    sl.putConstraint( SpringLayout.WEST,  b[3], 5, SpringLayout.EAST,  pb );
    
    // down right buttons, buttons for service actions
    sl.putConstraint( SpringLayout.SOUTH, b[10], -2, SpringLayout.SOUTH, p    );
    sl.putConstraint( SpringLayout.EAST,  b[10], -2, SpringLayout.EAST,  p    );
    sl.putConstraint( SpringLayout.SOUTH, b[9],  -2, SpringLayout.SOUTH, p    );
    sl.putConstraint( SpringLayout.EAST,  b[9],  -2, SpringLayout.WEST, b[10] );
    sl.putConstraint( SpringLayout.SOUTH, b[8],  -2, SpringLayout.SOUTH, p    );
    sl.putConstraint( SpringLayout.EAST,  b[8],  -2, SpringLayout.WEST, b[9]  );
    sl.putConstraint( SpringLayout.SOUTH, b[7],  -2, SpringLayout.SOUTH, p    );
    sl.putConstraint( SpringLayout.EAST,  b[7],  -2, SpringLayout.WEST, b[8]  );
    sl.putConstraint( SpringLayout.SOUTH, b[6],  -2, SpringLayout.SOUTH, p    );
    sl.putConstraint( SpringLayout.EAST,  b[6],  -2, SpringLayout.WEST, b[7]  );
    sl.putConstraint( SpringLayout.SOUTH, b[5],  -2, SpringLayout.SOUTH, p    );
    sl.putConstraint( SpringLayout.EAST,  b[5],  -2, SpringLayout.WEST, b[6]  );
    sl.putConstraint( SpringLayout.SOUTH, b[4],  -2, SpringLayout.SOUTH, p    );
    sl.putConstraint( SpringLayout.EAST,  b[4],  -2, SpringLayout.WEST, b[5]  );

    // labels for left combo boxes
    sl.putConstraint( SpringLayout.NORTH, a[0], 24, SpringLayout.SOUTH, txs  );
    sl.putConstraint( SpringLayout.WEST,  a[0], 7,  SpringLayout.WEST,  p    );
    sl.putConstraint( SpringLayout.NORTH, a[1], 4,  SpringLayout.SOUTH, a[0] );
    sl.putConstraint( SpringLayout.WEST,  a[1], 0,  SpringLayout.WEST,  a[0] );
    sl.putConstraint( SpringLayout.NORTH, a[2], 4,  SpringLayout.SOUTH, a[1] );
    sl.putConstraint( SpringLayout.WEST,  a[2], 0,  SpringLayout.WEST,  a[1] );
    sl.putConstraint( SpringLayout.NORTH, a[3], 4,  SpringLayout.SOUTH, a[2] );
    sl.putConstraint( SpringLayout.WEST,  a[3], 0,  SpringLayout.WEST,  a[2] );
    sl.putConstraint( SpringLayout.NORTH, a[4], 4,  SpringLayout.SOUTH, a[3] );
    sl.putConstraint( SpringLayout.WEST,  a[4], 0,  SpringLayout.WEST,  a[3] );
    sl.putConstraint( SpringLayout.NORTH, a[5], 4,  SpringLayout.SOUTH, a[4] );
    sl.putConstraint( SpringLayout.WEST,  a[5], 0,  SpringLayout.WEST,  a[4] );
    sl.putConstraint( SpringLayout.NORTH, a[6], 4,  SpringLayout.SOUTH, a[5] );
    sl.putConstraint( SpringLayout.WEST,  a[6], 0,  SpringLayout.WEST,  a[5] );
    
    // left combo boxes
    sl.putConstraint( SpringLayout.NORTH, c[0], 24, SpringLayout.SOUTH, txs  );
    sl.putConstraint( SpringLayout.WEST,  c[0],  1, SpringLayout.EAST,  a[0] );
    sl.putConstraint( SpringLayout.NORTH, c[1],  4, SpringLayout.SOUTH, c[0] );
    sl.putConstraint( SpringLayout.WEST,  c[1],  0, SpringLayout.WEST,  c[0] );
    sl.putConstraint( SpringLayout.NORTH, c[2],  4, SpringLayout.SOUTH, c[1] );
    sl.putConstraint( SpringLayout.WEST,  c[2],  0, SpringLayout.WEST,  c[1] );
    sl.putConstraint( SpringLayout.NORTH, c[3],  4, SpringLayout.SOUTH, c[2] );
    sl.putConstraint( SpringLayout.WEST,  c[3],  0, SpringLayout.WEST,  c[2] );
    sl.putConstraint( SpringLayout.NORTH, c[4],  4, SpringLayout.SOUTH, c[3] );
    sl.putConstraint( SpringLayout.WEST,  c[4],  0, SpringLayout.WEST,  c[3] );
    sl.putConstraint( SpringLayout.NORTH, c[5],  4, SpringLayout.SOUTH, c[4] );
    sl.putConstraint( SpringLayout.WEST,  c[5],  0, SpringLayout.WEST,  c[4] );
    sl.putConstraint( SpringLayout.NORTH, c[6],  4, SpringLayout.SOUTH, c[5] );
    sl.putConstraint( SpringLayout.WEST,  c[6],  0, SpringLayout.WEST,  c[5] );
    
    // labels for right combo boxes
    sl.putConstraint( SpringLayout.NORTH, a[7], 24, SpringLayout.SOUTH, txs   );
    sl.putConstraint( SpringLayout.WEST,  a[7], 22, SpringLayout.EAST,  c[0]  );
    sl.putConstraint( SpringLayout.NORTH, a[8],  4, SpringLayout.SOUTH, a[7]  );
    sl.putConstraint( SpringLayout.WEST,  a[8],  0, SpringLayout.WEST,  a[7]  );
    sl.putConstraint( SpringLayout.NORTH, a[9],  4, SpringLayout.SOUTH, a[8]  );
    sl.putConstraint( SpringLayout.WEST,  a[9],  0, SpringLayout.WEST,  a[8]  );
    sl.putConstraint( SpringLayout.NORTH, a[10], 4, SpringLayout.SOUTH, a[9]  );
    sl.putConstraint( SpringLayout.WEST,  a[10], 0, SpringLayout.WEST,  a[9]  );
    sl.putConstraint( SpringLayout.NORTH, a[11], 4, SpringLayout.SOUTH, a[10] );
    sl.putConstraint( SpringLayout.WEST,  a[11], 0, SpringLayout.WEST,  a[10] );
    sl.putConstraint( SpringLayout.NORTH, a[12], 4, SpringLayout.SOUTH, a[11] );
    sl.putConstraint( SpringLayout.WEST,  a[12], 0, SpringLayout.WEST,  a[11] );
    sl.putConstraint( SpringLayout.NORTH, a[13], 4, SpringLayout.SOUTH, a[12] );
    sl.putConstraint( SpringLayout.WEST,  a[13], 0, SpringLayout.WEST,  a[12] );
    
    // right combo boxes
    sl.putConstraint( SpringLayout.NORTH, c[7], 24, SpringLayout.SOUTH, txs   );
    sl.putConstraint( SpringLayout.WEST,  c[7],  1, SpringLayout.EAST,  a[7]  );
    sl.putConstraint( SpringLayout.NORTH, c[8],  4, SpringLayout.SOUTH, c[7]  );
    sl.putConstraint( SpringLayout.WEST,  c[8],  0, SpringLayout.WEST,  c[7]  );
    sl.putConstraint( SpringLayout.NORTH, c[9],  4, SpringLayout.SOUTH, c[8]  );
    sl.putConstraint( SpringLayout.WEST,  c[9],  0, SpringLayout.WEST,  c[8]  );
    sl.putConstraint( SpringLayout.NORTH, c[10], 4, SpringLayout.SOUTH, c[9]  );
    sl.putConstraint( SpringLayout.WEST,  c[10], 0, SpringLayout.WEST,  c[9]  );
    sl.putConstraint( SpringLayout.NORTH, c[11], 4, SpringLayout.SOUTH, c[10] );
    sl.putConstraint( SpringLayout.WEST,  c[11], 0, SpringLayout.WEST,  c[10] );
    sl.putConstraint( SpringLayout.NORTH, c[12], 4, SpringLayout.SOUTH, c[11] );
    sl.putConstraint( SpringLayout.WEST,  c[12], 0, SpringLayout.WEST,  c[11] );
    sl.putConstraint( SpringLayout.NORTH, c[13], 4, SpringLayout.SOUTH, c[12] );
    sl.putConstraint( SpringLayout.WEST,  c[13], 0, SpringLayout.WEST,  c[12] );
    
    // initializing child windows and native application runner
    childDraw = new ActionDraw( frame );
    childStatistics = new ActionStatistics( frame );
    childTextLog = new ActionTextLog( frame );
    taskShell = new ActionRun( mglst );
    
    // check status after constructor unpacking all binaries for all session
    OpStatus ops = taskShell.getInitStatus();
    if ( ! ops.getStatusFlag() )
        {
        JOptionPane.showMessageDialog
            ( frame, ops.getStatusString(), About.getShortName(), 
              JOptionPane.ERROR_MESSAGE ); 
        }
    
    // add listener for closeSession, required for delete unpacked binaries
    csl = new CloseSessionListener();
    frame.addWindowListener( csl );
    
    // copy "starting..." to openable report window
    String logcopy = getTextArea().getText();
    getChildTextLog().write( logcopy );
    
    // visual main application frame
    frame.add( p );
    frame.setDefaultCloseOperation( EXIT_ON_CLOSE );
    frame.setLocationRelativeTo( null );
    frame.setSize( SIZE_WINDOW );
    frame.setResizable( false );
    frame.setVisible( true );
    }

// handler for close session when exit by System.exit(n);
// this software emulation required because JVM calls windowClosing
// handler one when window closing.
public void invokeCloseSession()
    {
    csl.windowClosing( null );
    }

// handler for close session when window closed
private class CloseSessionListener extends WindowAdapter 
    {
    @Override public void windowClosing( WindowEvent e )
        {
        OpStatus ops = taskShell.closeSession();  // delete unpacked binaries
        if ( ! ops.getStatusFlag() )
            {
            JOptionPane.showMessageDialog
                ( frame, ops.getStatusString(), About.getShortName(), 
                  JOptionPane.ERROR_MESSAGE ); 
            }
        }
    }

}
