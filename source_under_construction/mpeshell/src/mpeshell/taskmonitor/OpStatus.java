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
