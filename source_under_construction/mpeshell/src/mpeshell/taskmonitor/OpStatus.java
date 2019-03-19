/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Representation of operation results for 
 * TaskMonitor and DirectoryMonitor procedures.
 * 
 */

package mpeshell.taskmonitor;

public class OpStatus 
{
private boolean statusFlag;
private String statusString;
public OpStatus( boolean b, String s )
    {
    statusFlag = b;
    statusString = s;
    }
public boolean getStatusFlag()          { return statusFlag;   }
public void setStatusFlag( boolean b )  { statusFlag = b;      }
public String getStatusString()         { return statusString; }
public void setStatusString( String s ) { statusString = s;    }
}
