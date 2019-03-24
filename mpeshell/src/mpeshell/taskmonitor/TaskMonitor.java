/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * 
 * 
 */

package mpeshell.taskmonitor;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.net.URL;
import java.nio.file.Files;
import java.nio.file.Path;
import mpeshell.MpeShell;
import mpeshell.service.About;
import mpeshell.taskmonitor.PlatformDetector.PlatformTypes;

public class TaskMonitor 
{
private final int BLOCK_SIZE = 16384;
private String estr;
private File dir   = null;
private File exe32 = null;
private File lib32 = null;
private File exe64 = null;
private File lib64 = null;
private File txt   = null;
private Process process = null;

// initializing background process status
private OpResult opresult = null;
public void initOpResult()
    {
    opresult = new OpResult( false, null, null );
    }

// get background process status
public OpResult getTaskMonitorResults()
    {
    return opresult;
    }

// set command line options string for native application run
private String cmdParms = null;
public void setCmdParms( String s )
    {
    cmdParms = s;
    }

// set platform/OS type for select native application to run
private PlatformTypes platform = null;
public void setPlatform( PlatformTypes p )
    {
    platform = p;
    }

// get directory path after create temporary directory and unpack binaries
private String reportDir = null;
public String getReportDir()
    {
    return reportDir;
    }

// unpack native binaries, called ONCE when RUN Java application session
public OpStatus unpackBinaries( String[][] binlist, String reportname )
    {
    estr = "";
    reportDir = null;
    Path dirpath;
    try { 
        dirpath = Files.createTempDirectory( null );  
        }
    catch ( IOException e ) 
        {
        dirpath = null;
        estr = " " + e.getMessage();
        }
    if ( dirpath == null )
        {
        return new OpStatus
            ( false, "Error creating temp directory." + estr );
        }
    dir = dirpath.toFile();
    reportDir = dirpath.toString();
    String dirstr = reportDir + "\\";
    exe32 = unpackFileHelper( dirstr, binlist[0][0], binlist[0][1] );
    if ( exe32 == null )
        return new OpStatus( false, "Error unpack EXE32." + estr );
    lib32 = unpackFileHelper( dirstr, binlist[0][0], binlist[0][2] );
    if ( lib32 == null )
        return new OpStatus( false, "Error unpack LIB32." + estr );
    exe64 = unpackFileHelper( dirstr, binlist[1][0], binlist[1][1] );
    if ( exe64 == null )
        return new OpStatus( false, "Error unpack EXE64." + estr );
    lib64 = unpackFileHelper( dirstr, binlist[1][0], binlist[1][2] );
    if ( lib64 == null )
        return new OpStatus( false, "Error unpack LIB64." + estr );
    txt = new File( dirstr + reportname );
    if ( txt == null )
        return new OpStatus( false, "Error reserve report file." + estr );
    else
        return new OpStatus( true, "OK" );
    }

// helper for unpack file
private File unpackFileHelper( String tmppath, String name, String ext )
    {
    estr = "";
    File file;
    String nmex = name + "." + ext;
    try {
        URL resource = MpeShell.class.getResource
                ( About.getResourcePath() + nmex );
            try ( InputStream input = resource.openStream() ) 
            {
                file = new File( tmppath + nmex );
                try ( FileOutputStream output = new FileOutputStream( file ) ) 
                    {
                    byte[] buffer = new byte[BLOCK_SIZE];
                    for( int i = input.read( buffer ); i != -1;
                        i = input.read( buffer ) )
                        {
                        output.write( buffer, 0, i );
                        }
                    }
                }
            }
        catch ( IOException e )
            {
            file = null;
            estr = " " + e.getMessage();
            }
    return file;
    }

// delete unpacked binaries, called ONCE when CLOSE Java application session
public OpStatus deleteBinaries()
    {
    boolean b = false;    
    int n = 50;
    while ( ( !b ) && ( n > 0 ) )
        {
        try {
            Thread.sleep(1);
            }
        catch ( InterruptedException e )
            {
            return new OpStatus
                ( false, "Delete interrupted. " + e.getMessage() );
            }
        boolean b1 = true, b2 = true, b3 = true,
                b4 = true, b5 = true, b6 = true;
        if ( exe32 != null ) { b1 = exe32.delete(); exe32 = null; }
        if ( lib32 != null ) { b2 = lib32.delete(); lib32 = null; }
        if ( exe64 != null ) { b3 = exe64.delete(); exe64 = null; }
        if ( lib64 != null ) { b4 = lib64.delete(); lib64 = null; }
        if ( txt   != null ) { b4 = txt.delete();   txt = null;   }
        if ( dir   != null ) { b6 = dir.delete();   dir = null;   }
        n--;
        b = b1 & b2 & b3 & b4 & b5 & b6;
        }
    String s = "OK";
    if ( !b )
        s = "Delete unpacked native application failed.";
    return new OpStatus( b, s );
    }

// delete text report
public OpStatus deleteReport()
    {
    boolean b = true;
    if ( txt != null ) { b = txt.delete(); txt = null;  }
    String s = "OK";
    if ( !b )
        s = "Delete text report failed.";
    return new OpStatus( b, s );
    }

// execute child task: native benchmark application with dynamical libraries.
public OpStatus executeTask()
    {
    File exe;
    switch ( platform )
    {
        case WIN32:
            exe = exe32;
            break;
        case WIN64:
            exe = exe64;
            break;
        default:
            return new OpStatus( false, "This platform is not supported." );
    }
    if ( exe == null )
        return new OpStatus( false, "Platform detection error." );
    
    String pathtext = exe.getAbsolutePath();
    String runtext = pathtext + " " + cmdParms;
    try {
        process = Runtime.getRuntime().exec( runtext );
        }
    catch ( IOException e ) 
        {
        process = null;
        estr = " " + e.getMessage();
        }
    if ( process == null )
        return new OpStatus( false, "Build process failed." + estr );
    
    while ( ! process.isAlive() )
        {
        try {
            Thread.sleep(1);
            }
        catch ( InterruptedException e )
            {
            return new OpStatus
                ( false, "Process wait start interrupted. " + e.getMessage() );
            }
        }
    
    long t1 = System.nanoTime();
    while ( process.isAlive() )
        {
        try {
            Thread.sleep( 20 );  // OLD = Thread.sleep( 1 );
            }
        catch ( InterruptedException e )
            {
            OpStatus ops = new OpStatus
                ( false, "Process wait stop interrupted. " + e.getMessage() );
            opresult.setStatusString( ops.getStatusString() );
            return ops;
            }
        }
    long t2 = System.nanoTime();
    double dt = ( t2 - t1 ) / 1_000_000_000.0;
    
    try {
        Thread.sleep( 100 );
        }
    catch ( InterruptedException e )
        {
        OpStatus ops = new OpStatus
            ( false, "Extra wait interrupted. " + e.getMessage() );
        opresult.setStatusString( ops.getStatusString() );
        return ops;
        }

    String stime = String.format
        ( "OK, process elapsed time = %.3f seconds", dt );
    opresult.setStatusString( stime );
    opresult.setDoneExe( true );
    return new OpStatus( true, stime );
    }

// interrupt runned native benchmark application
public void interruptTask()
    {
    process.destroy();
    }

}
