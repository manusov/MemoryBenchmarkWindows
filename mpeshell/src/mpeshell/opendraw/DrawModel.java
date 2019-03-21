/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * MODEL part of MVC ( Model, View, Controller ) pattern implementation
 * for open drawings window with benchmark results as
 * Speed = F ( Block Size) , Latency = F ( Block Size ).
 *
 */

package mpeshell.opendraw;

import java.math.BigDecimal;
import mpeshell.MpeGuiList.MeasurementModes;
import mpeshell.MpeGuiList.UnitsModes;

public class DrawModel implements DrawModelInterface
{

private BigDecimal[][] function;

private final double BIG_SCALE_BASE  = 1000.0;
private final double BIG_SCALE_DELTA = 100.0;
private final double SMALL_SCALE_BASE  = 10.0;  // note divisors 10.0 and 50.0
private final double SMALL_SCALE_DELTA = 5.0;

private final int DEFAULT_MAX_COUNT = 100;

private double scaleValue;
private double scaleDelta;
private int maxCount;
private int currentCount;

public DrawModel( DrawControllerInterface x )
    {
    reset();
    }

@Override final public void reset()
    {
    scaleValue = BIG_SCALE_BASE;
    scaleDelta = BIG_SCALE_DELTA;
    maxCount = DEFAULT_MAX_COUNT;
    currentCount = 0;
    rescaleXmax( maxCount );
    }

@Override public BigDecimal[][] getFunction()
    {
    return function;
    }

@Override public int[] getCurrentIndexes()
    {
    // return new int[] { currentCount, currentCount };
    return new int[] { currentCount };
    }

@Override public final int[] getMaximumIndexes()
    {
    // return new int[] { maxCount, maxCount };
    return new int[] { maxCount };
    }

@Override public void startModel() 
    {
    scaleValue = BIG_SCALE_BASE;
    currentCount = 0;
    }

@Override public void stopModel()
    {
    }

private UnitsModes modeX = UnitsModes.KILOBYTES;
@Override public void setModeX( UnitsModes mode )
    {
    modeX = mode;
    }

@Override public String getXname()
    {
    // return "block size, KB";
    // return "iteration";
    String s;
    switch ( modeX )
        {
        case KILOBYTES:
            {
            s = "Block, KB";
            break;
            }
        case MEGABYTES:
            {
            s = "Block, MB";
            break;
            }
        default:
            {
            s = "?";
            break;
            }
        }
    return s;
    }

private MeasurementModes modeY = MeasurementModes.BANDWIDTH;
@Override public void setModeY( MeasurementModes mode )
    {
    modeY = mode;
    }

@Override public String[] getYnames()
    {
    // return new String[] { "MBPS  single-thread", " multi-thread" };
    String[] s;
    switch ( modeY )
        {
        case BANDWIDTH:
            {
            s = new String[] { "Bandwidth, MBPS" };
            break;
            }
        case LATENCY:
            {
            s = new String[] { "Latency, ns" };
            break;
            }
        default:
            {
            s = new String[] { "?" };
            break;
            }
        }
    return s;
    }

@Override public BigDecimal getXmin()
    {
    String s = "1";
    if ( getXmax().intValue() > 10 ) s = "0";
    return new BigDecimal( s );
    }
        
@Override public BigDecimal getXmax()
    {
    return new BigDecimal( maxCount );
    }
        
@Override public BigDecimal getXsmallUnits()
    {
    double x = maxCount / 50.0;
    return new BigDecimal( x );
    }
        
@Override public BigDecimal getXbigUnits()
    {
    double x = maxCount / 10.0;
    return new BigDecimal( x );
    }

@Override public BigDecimal getYmin()
    {
    return new BigDecimal( "0" );
    }
        
@Override public BigDecimal getYmax()
    {
    return new BigDecimal( scaleValue );
    }

@Override public BigDecimal getYsmallUnits()
    {
    return new BigDecimal( scaleValue / 50.0 );
    }

@Override public BigDecimal getYbigUnits()
    {
    return new BigDecimal( scaleValue / 10.0 );
    }

@Override public final void rescaleXmax( int x )
    {
    maxCount = x;
    
    // function = new BigDecimal[3][maxCount];
    function = new BigDecimal[2][maxCount];
    
    BigDecimal a = new BigDecimal(0);
    for( int i=0; i<maxCount; i++ )
        {
        // function[0][i] = function[1][i] = function[2][i] = a;
        function[0][i] = function[1][i] = a;
        }
    }

@Override public void rescaleYmax()
    {
    double max = 0.0;
    int n = getCurrentIndexes().length;
    for( int i=0; i<n; i++ )
        {
        int m = getCurrentIndexes()[i];
        if ( m > 0 )
            {
            if ( i == 0 ) max = ( function[i+1][0] ).doubleValue();
            for ( int j=0; j<m; j++ )
                {
                double temp = ( function[i+1][j] ).doubleValue();
                if ( max < temp ) max = temp;
                }
            }
        
        if ( max < SMALL_SCALE_BASE )
            {
            scaleDelta = SMALL_SCALE_DELTA;
            }
        
        double tempScale = 0;
        while ( max > tempScale )
            {
            tempScale += scaleDelta;
            }
        
        scaleValue = tempScale;
        }
    }

@Override public void updateValue( BigDecimal[] x ) 
    {
    if ( currentCount < maxCount )
        {
        function[0][currentCount] = x[0];
        function[1][currentCount] = x[1];
        // function[2][currentCount] = x[2];
        currentCount++;
        }
    }
    
}
