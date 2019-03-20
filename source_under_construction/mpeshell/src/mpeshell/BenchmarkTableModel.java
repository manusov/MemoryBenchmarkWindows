/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Summary Table Model for memory benchmark, part of main window.
 *
 */

package mpeshell;

import javax.swing.table.AbstractTableModel;
import mpeshell.openstatistics.EntryStatistics;

public class BenchmarkTableModel extends AbstractTableModel
{
// table model class fields
private final String[] COLUMNS_NAMES = 
    { "Parameter" , "Median" , "Average" , "Minimum" , "Maximum" };
private String[][] rowsValues =
    { 
        { "Block size" , "-" , "-" , "-" , "-" } ,
        { "CPI"        , "-" , "-" , "-" , "-" } ,
        { "nsPI"       , "-" , "-" , "-" , "-" } ,
        { "MBPS"       , "-" , "-" , "-" , "-" }
    };
// table model this application-specific public methods
protected String[] getColumnsNames()          { return COLUMNS_NAMES; }
protected String[][] getRowsValues()          { return rowsValues;    }
protected void setRowsValues( String[][] s )  { rowsValues = s;       }
// table model standard required public methods
@Override public int getColumnCount()    { return COLUMNS_NAMES.length; }
@Override public int getRowCount()       { return rowsValues.length;    }
@Override public String getColumnName( int column )
    {
    if ( column < COLUMNS_NAMES.length )   
        return COLUMNS_NAMES[column];
    else
        return "?";
    }
@Override public String getValueAt( int row, int column )
    { 
    if ( ( row < rowsValues.length ) && ( column < COLUMNS_NAMES.length ) )
        return rowsValues[row][column];
    else
        return "";
    }
@Override public void setValueAt( Object x, int row, int column )
    {
    if ( ( row < rowsValues.length ) && ( column < COLUMNS_NAMES.length ) &&
         ( x instanceof String )  )
        rowsValues[row][column] = ( String ) x;
    }
@Override public boolean isCellEditable( int row, int column )
    { return false; }

// update table for each measured value from Report Monitor.
public void measurementNotify( EntryStatistics ests )
    {
    for( int i=0; i<4; i++ )
        {
        for ( int j=0; j<4; j++ )
            {
            setValueAt( ests.statTable[j][i], j, i+1 );
            }
        }
    fireTableRowsUpdated( 0, 3 );
    }

}
