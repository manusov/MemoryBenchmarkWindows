package mpeshell.taskmonitor;

public class OpResult extends OpStatus
{
private Object object;
private boolean doneExe = false;
private boolean doneReport = false;
public OpResult( boolean b, String s, Object ob )
    {
    super( b, s );
    object = ob;
    }
public Object getObject()              { return object;     }
public void setObject( Object ob )     { object = ob;       }
public boolean getDoneExe()            { return doneExe;    }
public void setDoneExe( boolean b )    { doneExe = b;       }
public boolean getDoneReport()         { return doneReport; }
public void setDoneReport( boolean b ) { doneReport = b;    }
}
