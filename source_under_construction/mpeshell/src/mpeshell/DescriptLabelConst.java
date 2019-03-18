/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Abstract class for text label description template.
 * This variant ( 1 of 2) for non changeable texts,
 * see also DescriptLabel.java (at VIEW package, note ported from JavaDraw)
 * This is template for build text labels descriptions objects.
 *
 */

package mpeshell;

import java.awt.Dimension;

public abstract class DescriptLabelConst 
{
abstract public String getName();     // get label string
abstract public String getText();     // get string for tooltips output
abstract public Dimension getSize();  // get preferred size: X,Y
}
