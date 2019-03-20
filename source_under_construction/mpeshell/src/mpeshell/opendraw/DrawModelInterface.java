/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Interface definition for MODEL.
 * MODEL part of MVC ( Model, View, Controller ) pattern implementation
 * for open drawings window with benchmark results as
 * Speed = F ( Block Size) , Latency = F ( Block Size ).
 *
 */

package mpeshell.opendraw;

import java.math.BigDecimal;
import mpeshell.MpeGuiList.MeasurementModes;

public interface DrawModelInterface 
{
public BigDecimal[][] getFunction();  // get function array { x, y1, ,,, yn }

public int[] getCurrentIndexes();     // get current indexes per each y-array
public int[] getMaximumIndexes();     // get maximum indexes per each y-array

public void startModel();             // start model, reset defaults
public void stopModel();              // stop model, yet reserved, no actions

public String     getXname();          // get name for X axis units
public String[]   getYnames();         // get names array for Y, per functions
public BigDecimal getXmin();           // get minimum X
public BigDecimal getXmax();           // get maximum X
public BigDecimal getXsmallUnits();    // get small graduation for X axis
public BigDecimal getXbigUnits();      // get big graduation for X axis
public BigDecimal getYmin();           // get minimum Y
public BigDecimal getYmax();           // get maximum Y
public BigDecimal getYsmallUnits();    // get small graduation for Y axis
public BigDecimal getYbigUnits();      // get big graduation for Y axis

public void rescaleXmax( int x );        // set x-scale by input x
public void rescaleYmax();               // set y-scale automatically by values
public void updateValue( BigDecimal[] x );  // add new element to array

public void reset();                         // reset data
public void setMode( MeasurementModes x );   // set Bandwidth or Latency mode

}
