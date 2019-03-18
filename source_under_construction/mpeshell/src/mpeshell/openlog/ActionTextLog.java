/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Handler for open text log window with benchmark results as
 * Speed = F ( Block Size) , Latency = F ( Block Size ).
 *
 */

package mpeshell.openlog;

import java.awt.Dimension;
import java.awt.Font;
import java.awt.Point;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JFrame;
import javax.swing.JPanel;
import javax.swing.JScrollPane;
import javax.swing.JTextArea;
import javax.swing.SpringLayout;
import static javax.swing.WindowConstants.DISPOSE_ON_CLOSE;
import javax.swing.text.DefaultCaret;

public class ActionTextLog extends JFrame
{
private final JFrame parentFrame;
private final JPanel txp;
private final JTextArea tx;
private boolean childActive = false;
    
public ActionTextLog( JFrame f )
    {
    super( "Benchmark log" );
    parentFrame = f;
    SpringLayout sl = new SpringLayout();
    txp = new JPanel( sl );
    tx = new JTextArea();
    Font font = new Font( "monospaced", Font.PLAIN, 12 );
    tx.setFont( font );
    tx.setEditable( false );
    DefaultCaret caret = ( DefaultCaret )( tx.getCaret() );
    caret.setUpdatePolicy( DefaultCaret.ALWAYS_UPDATE );
    JScrollPane txs = new JScrollPane( tx );
    txp.add( txs );
    // layout setup
    sl.putConstraint( SpringLayout.NORTH, txs,  1, SpringLayout.NORTH, txp );
    sl.putConstraint( SpringLayout.SOUTH, txs, -1, SpringLayout.SOUTH, txp );
    sl.putConstraint( SpringLayout.WEST,  txs,  1, SpringLayout.WEST,  txp );
    sl.putConstraint( SpringLayout.EAST,  txs, -1, SpringLayout.EAST,  txp );
    }
    
// this point for start drawings, checks if currently active
public void openWindow()
    {
    if ( ! childActive )
        {
        childActive = true;    // prevent open >1 window
        add( txp );
        setDefaultCloseOperation( DISPOSE_ON_CLOSE );
        addWindowListener( new ChildWindowListener() );
        Point p = parentFrame.getLocation();
        p.x = p.x + 300;
        p.y = p.y + 120;
        this.setLocation( p );
        setSize( new Dimension ( 610, 520 ) );
        setVisible( true );
        setResizable( true );
        }
    }

// method for write string, append to existed
public void write( String s )
    {
    if ( tx != null )
        tx.append( s );
    }

// method for write string, overwrite existed
public void overWrite( String s )
    {
    if ( tx != null )
        tx.setText( s );
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
