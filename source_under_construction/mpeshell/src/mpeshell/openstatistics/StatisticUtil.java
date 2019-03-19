/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Static class with statistic library: min, max, median, average for arrays.
 * Plus strings builder for statistics info in the tables.
 * Note some yet unused methods disabled.
 * Note array ordering software method don't replaced to java.util.Arrays.sort
 * because service array with elements indexes required.
 * Update carefully.
 *
 */

package mpeshell.openstatistics;

import java.util.concurrent.CopyOnWriteArrayList;
import mpeshell.taskmonitor.NumericEntry;

public class StatisticUtil 
{

// find minimum value of array
public static double findMin( double[] array )
    {
    return findMin( array, array.length );
    }
    
// method returns minimum of input array, selected part n elements
public static double findMin( double[] array, int n )
    {
    double min = Double.NaN;
    if ( ( array != null ) && ( n > 0 ) && ( n <= array.length ) )
        {
        min = array[0];
        for( int i=0; i<n; i++ )
            {
            if ( min > array[i] ) min = array[i];
            }
        }
    return min;
    }

// find maximum value of array
public static double findMax( double[] array )
    {
    return findMax( array, array.length );
    }

// method returns maximum of input array, selected part n elements
public static double findMax( double[] array, int n )
    {
    double max = Double.NaN;
    if ( ( array != null ) && ( n > 0 ) && ( n <= array.length ) )
        {
        max = array[0];
        for( int i=0; i<n; i++ )
            {
            if ( max < array[i] ) max = array[i];
            }
        }
    return max;
    }

// find average value of array

// method returns average of input array
public static double findAverage( double[] array )
    {
    return findAverage( array, array.length );
    }

// method returns average of input array, selected part n elements
public static double findAverage( double[] array, int n )
    {
    double average = Double.NaN;
    if ( ( array != null ) && ( n > 0 ) && ( n <= array.length ) )
        {
        average = 0.0;
        for( int i=0; i<n; i++ )
            {
            average += array[i];
            }
        average /= n;
        }
    return average;
    }

// find median value of array
public static double findMedian( double[] array )
    {
    return findMedian( array, array.length );
    }

public static double findMedian( double[] array, int n )
    {
    double median = Double.NaN;
    if ( ( array != null ) && ( n == 1 ) && ( n <= array.length ) )
        {  // median for one element array
        median = array[0];
        }
    else if ( ( array != null ) && ( n > 1 ) && ( n <= array.length ) )
        {
        // sorting array
        boolean flag = true;
        while (flag)
            {
            flag = false;
            for( int i=0; i<n-1; i++ )
                {
                if ( array[i] > array[i+1] )
                    {
                    double temp = array[i];
                    array[i] = array[i+1];
                    array[i+1] = temp;
                    flag = true;
                    }
                }
            }
        // get median from sorted array
        int i = n/2;
        if ( n % 2 == 0 )
            {
            median = ( array[i-1] + array[i] ) / 2.0;
            }
        else
            {
            median = array[i];
            }
        }
    return median;
    }

// find median value of array, additionally return indexes

// method returns median representation of input array
public static EntryMedian findMedianIndex( double[] array )
    {
    return findMedianIndex( array, array.length );
    }

// method returns median representation of input array, selected part n elem.
public static EntryMedian findMedianIndex( double[] arrayIn, int n )
    {
    // this copy required because input array modified (sorted) by method
    int m = arrayIn.length;
    double[] array = new double[m];
    System.arraycopy( arrayIn, 0, array, 0, m );
    // operation with copy of array
    int a=-1, b=-1, c=-1;                           // median position indexes
    int[] service = new int[n];
    for(int i=0; i<n; i++)
        {
        service[i] = i;
        }
    double median = Double.NaN;
    double median1 = Double.NaN;
    double median2 = Double.NaN;
    double median3 = Double.NaN;
    if ( ( array != null ) && ( n == 1 ) && ( n <= array.length ) )
        {  // median for one element array
        median = array[0];
        a = b = c = 0;
        median1 = array[a];
        median2 = array[b];
        median3 = array[c];
        }
    else if ( ( array != null ) && ( n > 1 ) && ( n <= array.length ) )
        {

        // sorting array, support values and indexes arrays
        boolean flag = true;
        while (flag)
            {
            flag = false;
            for( int i=0; i<n-1; i++ )
                {
                if ( array[i] > array[i+1] )
                    {
                    double temp1 = array[i];     // values array support
                    array[i] = array[i+1];
                    array[i+1] = temp1;
                    
                    int temp2 = service[i];      // service array support
                    service[i] = service[i+1];
                    service[i+1] = temp2;
                    
                    flag = true;
                    }
                }
            }

        // Can't sorting by array utilites, because
        // service array with elements indexes generation required.
        // Arrays.sort( array, 0, n );
            
        // get median from sorted array
        int i = n/2;
        if ( n % 2 == 0 )
            {
            median = ( array[i-1] + array[i] ) / 2.0;
            a = i-1;
            b = i;
            median1 = array[a];
            median2 = array[b];
            }
        else
            {
            median = array[i];
            if ((i==0)||(i==n))
                {
                a = b = c = i;
                }
            else
                {
                a = i-1;
                b = i+1;
                c = i;
                }
            median1 = array[a];
            median2 = array[b];
            median3 = array[c];
            }
        }
    if ( a>=0 ) a = service[a];
    if ( b>=0 ) b = service[b];
    if ( c>=0 ) c = service[c];
    return new EntryMedian( median, median1, median2, median3, a, b, c );
    }


// method returns statistics representation of input array
public static EntryDetail getStatistic( double[] array )
    {
    return getStatistic( array, array.length );
    }

// method returns statistics representation of input array, 
// selected part n elements
public static EntryDetail getStatistic( double[] array, int n )
    {
    EntryMedian medianEntry = findMedianIndex( array, n );
    double average = findAverage( array, n );
    double min = findMin( array, n );
    double max = findMax( array, n );
    EntryDetail statisticEntry = new EntryDetail
        ( medianEntry.median, 
          medianEntry.median1,
          medianEntry.median2,
          medianEntry.median3, 
          average, min, max,
          medianEntry.medianIndex1,
          medianEntry.medianIndex2,
          medianEntry.medianIndex3 );
    return statisticEntry;
    }

// method accept parameters for ReportListener and builds detail statistics
// info (include text strings) for tables:
// 1) table in the main window, 2) table in the openable statistics window.
public static EntryStatistics buildStatisticsTable
        ( CopyOnWriteArrayList<NumericEntry> dataArray )
    {
    EntryStatistics estat = new EntryStatistics();
    // build 4 arrays: Block Size, CPI, nsPI, MBPS
    int k = dataArray.size();
    estat.blockArray = new double[k];
    estat.cpiArray   = new double[k];
    estat.nspiArray  = new double[k];
    estat.mbpsArray  = new double[k];
    for( int j=0; j<k; j++ )
        {
        estat.blockArray[j] = dataArray.get( j ).doubles[ 0 ];
        estat.cpiArray[j]   = dataArray.get( j ).doubles[ 1 ];
        estat.nspiArray[j]  = dataArray.get( j ).doubles[ 2 ];
        estat.mbpsArray[j]  = dataArray.get( j ).doubles[ 3 ];
    }
    EntryDetail size = StatisticUtil.getStatistic( estat.blockArray );
    EntryDetail cpi  = StatisticUtil.getStatistic( estat.cpiArray   );
    EntryDetail nspi = StatisticUtil.getStatistic( estat.nspiArray  );
    EntryDetail mbps = StatisticUtil.getStatistic( estat.mbpsArray  );
    estat.statTable = new String[4][4];
    estat.statTable[0][0] = String.format( "%.1f K" , size.median / 1024.0 );
    estat.statTable[1][0] = String.format( "%.3f"   , cpi.median );
    estat.statTable[2][0] = String.format( "%.3f"   , nspi.median );
    estat.statTable[3][0] = String.format( "%.3f"   , mbps.median );
    estat.statTable[0][1] = String.format( "%.1f K" , size.average / 1024.0 );
    estat.statTable[1][1] = String.format( "%.3f"   , cpi.average );
    estat.statTable[2][1] = String.format( "%.3f"   , nspi.average );
    estat.statTable[3][1] = String.format( "%.3f"   , mbps.average );
    estat.statTable[0][2] = String.format( "%.1f K" , size.min / 1024.0 );
    estat.statTable[1][2] = String.format( "%.3f"   , cpi.min );
    estat.statTable[2][2] = String.format( "%.3f"   , nspi.min );
    estat.statTable[3][2] = String.format( "%.3f"   , mbps.min );
    estat.statTable[0][3] = String.format( "%.1f K" , size.max / 1024.0 );
    estat.statTable[1][3] = String.format( "%.3f"   , cpi.max );
    estat.statTable[2][3] = String.format( "%.3f"   , nspi.max );
    estat.statTable[3][3] = String.format( "%.3f"   , mbps.max );
    return estat;
    }

}
