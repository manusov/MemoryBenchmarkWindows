/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Handler for open drawings window with benchmark results as
 * Speed = F ( Block Size) , Latency = F ( Block Size ).
 *
 */

package mpeshell.opendraw;

import java.awt.Dimension;
import java.awt.Point;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JFrame;
import javax.swing.JPanel;
import static javax.swing.WindowConstants.DISPOSE_ON_CLOSE;

public class ActionDraw extends JFrame
{
private final JFrame parentFrame;
private final JPanel childPanel;
private final DrawController controller;
private boolean childActive = false;

// get controller for access MVC pattern data
public DrawController getController() { return controller; }

// frame class constructor, f = parent frame
public ActionDraw( JFrame f )
    {
    super( "Benchmark drawings" );
    parentFrame = f;
    controller = new DrawController();
    childPanel = controller.getView().getPanel();
    }

// this point for start drawings, checks if currently active
public void openWindow()
    {
    if ( ! childActive )
        {
        childActive = true;    // prevent open >1 window
        add( childPanel );
        setDefaultCloseOperation( DISPOSE_ON_CLOSE );
        addWindowListener( new ChildWindowListener() );
        Point p = parentFrame.getLocation();
        p.x = p.x + 500;
        p.y = p.y + 320;
        this.setLocation( p );
        setSize( new Dimension ( 740, 540 ) );
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
