/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Abstract class for button description template.
 * This is template for build buttons descriptions objects.
 *
 */

package mpeshell;

import java.awt.Color;
import java.awt.Dimension;
import java.awt.Font;
import java.awt.event.ActionListener;

public abstract class DescriptButton implements ActionListener
{
// constants for GUI customization
protected final static Color CLR_CUSTOM_COLOR = Color.RED;
protected final static Color RUN_CUSTOM_COLOR = Color.BLUE;
protected final static Font  CLR_CUSTOM_FONT  = 
            new Font("Verdana", Font.PLAIN, 13);
protected final static Font RUN_CUSTOM_FONT  = CLR_CUSTOM_FONT;
protected final static Font  SMALL_CUSTOM_FONT  = 
            new Font("Verdana", Font.PLAIN, 11);
// public abstract methods
abstract public String getName();     // get string for button name
abstract public String getText();     // get string for tooltips output
abstract public Dimension getSize();  // get preferred size: X,Y
// public optional supported methods
public int[] getKeys() { return null; }   // get keyboard keys for press this
public Color getCustomColor() { return null; }  // get custom color for button
public Font getCustomFont()   { return null; }  // get custom font for button
}
