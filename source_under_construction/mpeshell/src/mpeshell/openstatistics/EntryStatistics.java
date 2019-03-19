/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Class for return detail info about benchmarks measurement session.
 * Used for communication with table models.
 *
 */

package mpeshell.openstatistics;

import java.util.concurrent.CopyOnWriteArrayList;
import mpeshell.taskmonitor.NumericEntry;

public class EntryStatistics 
{
// data
public CopyOnWriteArrayList<NumericEntry>
    dataArray = new CopyOnWriteArrayList<>();
// statistics
public double[] blockArray;       // data blocks sizes
public double[] cpiArray;         // CPI = Clocks per Instruction
public double[] nspiArray;        // NSPI = Nanoseconds per Instruction
public double[] mbpsArray;        // MBPS = Megabytes per Second
public EntryDetail blockEntry;    // Entries for current point
public EntryDetail cpiEntry;
public EntryDetail nspiEntry;
public EntryDetail mbpsEntry;
public String[][] statTable;      // strings for statistics in the GUI tables
}
