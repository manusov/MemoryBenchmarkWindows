package mpeshell.openstatistics;

import java.util.concurrent.CopyOnWriteArrayList;
import mpeshell.taskmonitor.NumericEntry;

public class EntryStatistics 
{
// data
public CopyOnWriteArrayList<NumericEntry>
    dataArray = new CopyOnWriteArrayList<>();
// statistics
public double[] blockArray;
public double[] cpiArray;
public double[] nspiArray;
public double[] mbpsArray;
public EntryDetail blockEntry;
public EntryDetail cpiEntry;
public EntryDetail nspiEntry;
public EntryDetail mbpsEntry;
public String[][] statTable;
}
