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
public String[] getColumnsNames()          { return COLUMNS_NAMES; }
public String[][] getRowsValues()          { return rowsValues;    }
public void setRowsValues( String[][] s )  { rowsValues = s;       }
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

/*
public void arrayNotify
    ( double[] sz, double[] cp, double[] ns, double[] mb )
    {
    int n = sz.length;
    EntryDetail size = StatisticUtil.getStatistic( sz );
    EntryDetail cpi  = StatisticUtil.getStatistic( cp );
    EntryDetail nspi = StatisticUtil.getStatistic( ns );
    EntryDetail mbps = StatisticUtil.getStatistic( mb );
    setValueAt( String.format( "%.1f K" , size.median / 1024.0 )  , 0, 1 );
    setValueAt( String.format( "%.3f"   , cpi.median )            , 1, 1 );
    setValueAt( String.format( "%.3f"   , nspi.median )           , 2, 1 );
    setValueAt( String.format( "%.3f"   , mbps.median )           , 3, 1 );
    setValueAt( String.format( "%.1f K" , size.average / 1024.0 ) , 0, 2 );
    setValueAt( String.format( "%.3f"   , cpi.average )           , 1, 2 );
    setValueAt( String.format( "%.3f"   , nspi.average )          , 2, 2 );
    setValueAt( String.format( "%.3f"   , mbps.average )          , 3, 2 );
    setValueAt( String.format( "%.1f K" , size.min / 1024.0 )     , 0, 3 );
    setValueAt( String.format( "%.3f"   , cpi.min )               , 1, 3 );
    setValueAt( String.format( "%.3f"   , nspi.min )              , 2, 3 );
    setValueAt( String.format( "%.3f"   , mbps.min )              , 3, 3 );
    setValueAt( String.format( "%.1f K" , size.max / 1024.0 )     , 0, 4 );
    setValueAt( String.format( "%.3f"   , cpi.max )               , 1, 4 );
    setValueAt( String.format( "%.3f"   , nspi.max )              , 2, 4 );
    setValueAt( String.format( "%.3f"   , mbps.max )              , 3, 4 );
    fireTableRowsUpdated( 0, 3 );
    }
*/

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
