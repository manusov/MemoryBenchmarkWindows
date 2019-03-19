/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Representation of measurement result. Note:
 * Block size for function speed=f(size), size units = bytes
 * CPI = Clocks per Instruction
 * NSPI = Nanoseconds per Instruction
 * MBPS = Megabytes per Second
 * 
 */

package mpeshell.taskmonitor;

import java.math.BigDecimal;

public class NumericEntry 
{
public int num;                   // measurement iteration number
public double[] doubles;          // array[4] = block size, cpi, nspi, mbps
public BigDecimal[] bigdecs;      // same array as BigDecimal
public NumericEntry( int n, double[] d, BigDecimal[] b )
    {
    num = n;
    doubles = d;
    bigdecs = b;
    }
}
