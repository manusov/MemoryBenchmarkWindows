/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Handler for open statistics table window with benchmark results as
 * Speed = F ( Block Size) or Latency = F ( Block Size ).
 *
 */

package mpeshell.openstatistics;

import java.awt.Dimension;
import java.awt.Point;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTable;
import javax.swing.SpringLayout;
import javax.swing.SwingConstants;
import static javax.swing.WindowConstants.DISPOSE_ON_CLOSE;
import javax.swing.table.DefaultTableCellRenderer;

public class ActionStatistics extends JFrame
{
private final JFrame parentFrame;
private final JPanel tbp;
private final JTable table;
private final StatisticsTableModel stm;
private boolean childActive = false;

public ActionStatistics( JFrame f )
    {
    super( "Benchmark statistics" );
    parentFrame = f;
    SpringLayout sl = new SpringLayout();
    tbp = new JPanel( sl );
    stm = new StatisticsTableModel();
    table = new JTable( stm );
        JScrollPane tbs = new JScrollPane( table );
    DefaultTableCellRenderer tr = new DefaultTableCellRenderer();
    tr.setHorizontalAlignment( SwingConstants.CENTER );
    for ( int i=0; i<table.getColumnCount(); i++ )
        table.getColumnModel().getColumn(i).setCellRenderer( tr );
    tbp.add( tbs );
    // layout setup
    sl.putConstraint( SpringLayout.NORTH, tbs,  1, SpringLayout.NORTH, tbp );
    sl.putConstraint( SpringLayout.SOUTH, tbs, -1, SpringLayout.SOUTH, tbp );
    sl.putConstraint( SpringLayout.WEST,  tbs,  1, SpringLayout.WEST,  tbp );
    sl.putConstraint( SpringLayout.EAST,  tbs, -1, SpringLayout.EAST,  tbp );
    }

// method for blank table and set maximum lines = n,
// n exclude 4 additional Median, Average, Minimum, Maximum
public void blankTable( int n )
    {
    if ( stm != null )
        {
        stm.blank( n );
        DefaultTableCellRenderer tr = new DefaultTableCellRenderer();
        tr.setHorizontalAlignment( SwingConstants.CENTER );
        for ( int i=0; i<table.getColumnCount(); i++ )
            table.getColumnModel().getColumn(i).setCellRenderer( tr );
        }
    }

// method gets table model, used for text report
public StatisticsTableModel getTableModel()
    {
    return stm;
    }

// this point for start drawings, checks if currently active
public void openWindow()
    {
    if ( ! childActive )
        {
        childActive = true;    // prevent open >1 window
        add( tbp );
        setDefaultCloseOperation( DISPOSE_ON_CLOSE );
        addWindowListener( new ChildWindowListener() );
        Point p = parentFrame.getLocation();
        p.x = p.x + 400;
        p.y = p.y + 220;
        this.setLocation( p );
        setSize( new Dimension ( 510, 520 ) );
        setVisible( true );
        setResizable( true );
        }
    }

// this point for stop drawings
private class ChildWindowListener extends WindowAdapter 
    {
    @Override public void windowClosing( WindowEvent e )
        {
        childActive = false;  // enable re-open this window if it closed
        }
    }
    
}
