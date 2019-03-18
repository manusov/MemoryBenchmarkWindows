/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * CONTROLLER part of MVC ( Model, View, Controller ) pattern implementation
 * for open drawings window with benchmark results as
 * Speed = F ( Block Size) , Latency = F ( Block Size ).
 *
 */

package mpeshell.opendraw;

public class DrawController implements DrawControllerInterface
{
private final DrawModelInterface model;
private final DrawViewInterface view;

public DrawController()
    {
    model = new DrawModel( this );
    view = new DrawView( this );
    }

@Override public DrawModelInterface getModel()
    {
    return model;
    }

@Override public DrawViewInterface getView()
    {
    return view;
    }

@Override public void reset()
    {
    model.reset();
    view.getPanel().repaint();
    }

}
