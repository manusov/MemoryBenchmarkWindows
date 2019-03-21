/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Class for detectable system information.
 *
 */

package mpeshell;

public class SystemInformation 
{

// cache and DRAM control, units = Kilobytes
private final static int DEFAULT_X_SCALE_L1   = 80;
private final static int DEFAULT_X_SCALE_L2   = 600;
private final static int DEFAULT_X_SCALE_L3   = 20000;
private final static int DEFAULT_X_SCALE_L4   = 40000;
private final static int DEFAULT_X_SCALE_DRAM = 100000;

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

protected void reset()
    {
    scale1    = DEFAULT_X_SCALE_L1;
    scale2    = DEFAULT_X_SCALE_L2;
    scale3    = DEFAULT_X_SCALE_L3;
    scale4    = DEFAULT_X_SCALE_L4;
    scaleDram = DEFAULT_X_SCALE_DRAM;
    }




    
}
