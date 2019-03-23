/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Class for detectable system information.
 *
 */

package mpeshell;

import mpeshell.taskmonitor.SysInfoEntry;

public class SystemInformation 
{

// cache and DRAM control, units = Kilobytes
private final static int DEFAULT_X_SCALE_L1   = 80;
private final static int DEFAULT_X_SCALE_L2   = 800;
private final static int DEFAULT_X_SCALE_L3   = 20000;
private final static int DEFAULT_X_SCALE_L4   = 40000;
private final static int DEFAULT_X_SCALE_DRAM = 80000;

private long l1 = 0;
private long l2 = 0;
private long l3 = 0;
private long l4 = 0;
private long dram = 0;

public long getL1()   { return l1;   }
public long getL2()   { return l2;   }
public long getL3()   { return l3;   }
public long getL4()   { return l4;   }
public long getDram() { return dram; }

private int scale1;
private int scale2;
private int scale3;
private int scale4;
private int scaleDram;

public int getScale1()    { return scale1;    }
public int getScale2()    { return scale2;    }
public int getScale3()    { return scale3;    }
public int getScale4()    { return scale4;    }
public int getScaleDram() { return scaleDram; }

public void setScale1( int n )    { scale1 = n;    }
public void setScale2( int n )    { scale2 = n;    }
public void setScale3( int n )    { scale3 = n;    }
public void setScale4( int n )    { scale4 = n;    }
public void setScaleDram( int n ) { scaleDram = n; }

public SystemInformation()
    {
    reset();
    }

final protected void reset()
    {
    scale1    = DEFAULT_X_SCALE_L1;
    scale2    = DEFAULT_X_SCALE_L2;
    scale3    = DEFAULT_X_SCALE_L3;
    scale4    = DEFAULT_X_SCALE_L4;
    scaleDram = DEFAULT_X_SCALE_DRAM;
    }

protected void init()
    {
    scale1 = cacheScaleHelper( l1, DEFAULT_X_SCALE_L1 );
    scale2 = cacheScaleHelper( l2, DEFAULT_X_SCALE_L2 );
    scale3 = cacheScaleHelper( l3, DEFAULT_X_SCALE_L3 );
    scale4 = cacheScaleHelper( l4, DEFAULT_X_SCALE_L4 );
    scaleDram = DEFAULT_X_SCALE_DRAM;
    }

private int cacheScaleHelper( long value, int def  )
    {
    if ( value <= 0 )
        return def;
    int size = ( int )( value / 1024 );
    int scale = 80;
    while ( scale < size )
        scale *= 2;
    if ( scale / size < 2 )
        scale = scale / 2 + scale;
    return scale;
    }

// this method called when parse report 
// and extract system information parameters
public void acceptEntry( SysInfoEntry entry )
    {
    if ( entry != null )
        {
        switch( entry.infoType )
            {
            case L1:
                l1 = entry.value;
                break;
            case L2:
                l2 = entry.value;
                break;
            case L3:
                l3 = entry.value;
                break;
            case L4:
                l4 = entry.value;
                break;
            case DRAM:
                dram = entry.value;
                break;
            default:
                break;
            }
        }
    }
    
}
