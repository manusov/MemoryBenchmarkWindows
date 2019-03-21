/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Interface definition for CONTROLLER.
 * CONTROLLER part of MVC ( Model, View, Controller ) pattern implementation
 * for open drawings window with benchmark results as
 * Speed = F ( Block Size) , Latency = F ( Block Size ).
 *
 */

package mpeshell.opendraw;

public interface DrawControllerInterface 
{
public DrawModelInterface getModel(); // get connected model = function
public DrawViewInterface getView();   // get connected view = panel
public void reset();
}
