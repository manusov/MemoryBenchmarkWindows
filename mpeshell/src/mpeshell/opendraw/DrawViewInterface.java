/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Interface definition for VIEW.
 * VIEW part of MVC ( Model, View, Controller ) pattern implementation
 * for open drawings window with benchmark results as
 * Speed = F ( Block Size) , Latency = F ( Block Size ).
 *
 */

package mpeshell.opendraw;

import javax.swing.JPanel;

public interface DrawViewInterface 
{
public JPanel getPanel();  // get drawings view panel with own paint method
}
