package mpeshell.taskmonitor;

import java.util.concurrent.CopyOnWriteArrayList;

public interface WatchDataListener 
{
public void dataHandler
        ( CopyOnWriteArrayList<String> reportStrings, OpStatus reportStatus );
}
