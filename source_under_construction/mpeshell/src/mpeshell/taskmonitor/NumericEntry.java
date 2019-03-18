package mpeshell.taskmonitor;

import java.math.BigDecimal;

public class NumericEntry 
{
public int num;
public double[] doubles;
public BigDecimal[] bigdecs;
public NumericEntry( int n, double[] d, BigDecimal[] b )
    {
    num = n;
    doubles = d;
    bigdecs = b;
    }
}
