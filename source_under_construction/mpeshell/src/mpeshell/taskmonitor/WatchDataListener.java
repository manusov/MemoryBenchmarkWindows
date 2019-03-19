/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Interface definition for Strings Array to Parsed Data converter-listener.
 * 
 */

package mpeshell.taskmonitor;

import java.util.concurrent.CopyOnWriteArrayList;

public interface WatchDataListener 
{
public void dataHandler
        ( CopyOnWriteArrayList<String> reportStrings, OpStatus reportStatus );
}
