/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Interface definition for Report File to Strings Array converter-listener.
 * 
 */

package mpeshell.taskmonitor;

import java.nio.file.WatchEvent;
import java.util.concurrent.CopyOnWriteArrayList;

public interface WatchFileListener 
{
public void fileHandler( WatchEvent we );
public CopyOnWriteArrayList<String> getReport();
public OpStatus getStatus();
}
