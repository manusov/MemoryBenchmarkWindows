/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Abstract class for combo box description template.
 * This is template for build combo boxes descriptions objects.
 *
 */

package mpeshell;

import java.awt.Dimension;
import java.awt.event.ActionListener;

public abstract class DescriptCombo implements ActionListener
{
final String GRAY_PREFIX = "<html><font color=gray>";
final MpeGuiList mglst;  // used for access all GUI control
final int id;            // id of this described JComboBox

public DescriptCombo( MpeGuiList x, int y ) 
    {
    mglst = x;
    id = y;
    }

// public methods for support GUI functionality
abstract public String[] getValues(); // get strings for combo items names
abstract public String getText();     // get string for tooltips output
abstract public Dimension getSize();  // get preferred size: X,Y

// public optional supported methods
public int[] getKeys() { return null; }   // get keyb. keys for click this

// this method for simplify visualization and encodings values
// parent class provides dummy handler, for example if items is
// text strings, not a numeric values
public int[] getIntValues()       { return null;  }  // get selections values
public long[] getLongValues()     { return null;  }
public int getDefaultSelection()  { return -1;    }  // get default item, -1=no
public boolean getDefaultEnable() { return true;  }  // get default state, e/d

// methods get name and value for build string: option name = option value 
abstract public String getCmdName();       // get cmd. line option name
abstract public String[] getCmdValues();   // get cmd. option array of values

// support option availability control
final String[] NA = new String[] { "n/a" };
private boolean available = true;

public boolean getAvailable()         
    {
    return available;
    }

public void setAvailable( boolean b )
    {
    available = b;
    }

public boolean getFunctional()
    {
    return available; 
    }

private int previous = getDefaultSelection();
public int getPrevious()         { return previous; }
public void setPrevious( int n ) { previous = n;    }

// helpers methods useable by child classes

final long[] buildRoundNumbersArray( long x, int n )
    {
    long[] array = new long[n];
    for( int i=0; i<n; i++ )
        {
        array[i] = x;
        x = x << 1;
        }
    return array;
    }

String[] printRoundSizesArray( long x, int n, boolean b )
    {
    String[] s = new String[n];
    for( int i=0; i<n; i++ )
        {
        s[i] = PrintHelper.printRoundSize( x, b );
        x = x << 1;
        }
    return s;
    }

final String[] buildSizesOptions( long x, int n )
    {
    return buildSizesOptions( x, n, true );
    }

final String[] buildSizesOptions( long x, int n, boolean b )
    {
    String[] s1 = { "auto" };
    String[] s2 = printRoundSizesArray( x, n, b );
    String[] s = new String[ s1.length + s2.length ];
    int i = 0;
    for ( String s11 : s1 )
        {
        s[i] = s11;
        i++;
        }
    for ( String s21 : s2 ) 
        {
        s[i] = s21;
        i++;
        }
    return s;
    }

final String[] buildNumbersOptions( int[] x )
    {
    int n = x.length;
    String[] s1 = { "no control" };
    String[] s2 = new String[n];
    for( int i=0; i<n; i++ )
        s2[i] = "" + x[i];
    String[] s = new String[ s1.length + s2.length ];
    int i = 0;
    for ( String s11 : s1 )
        {
        s[i] = s11;
        i++;
        }
    for ( String s21 : s2 ) 
        {
        s[i] = s21;
        i++;
        }
    return s;
    }

}
