/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Representation of Cache or DRAM size, extracted from 
 * native application text report.
 *
 */

package mpeshell.taskmonitor;

public class SysInfoEntry 
{
public enum InfoTypes
    { L1, L2, L3, L4, DRAM, CPUBITMAP, HTBITMAP, THREADSCOUNT, UNKNOWN }
public InfoTypes infoType;
public long value;
public SysInfoEntry( InfoTypes type, long x )
    {
    infoType = type;
    value = x;
    }
}
