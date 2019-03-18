package mpeshell.taskmonitor;

import java.nio.file.WatchEvent;
import java.util.concurrent.CopyOnWriteArrayList;

public interface WatchFileListener 
{
public void fileHandler( WatchEvent we );
public CopyOnWriteArrayList<String> getReport();
public OpStatus getStatus();
}
