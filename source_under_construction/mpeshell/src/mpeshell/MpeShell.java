/*
 *
 * Memory Performance Engine (MPE) GUI Shell. (C)2019 IC Book Labs.
 *                     Main module.
 *      Start point for Console and GUI scenarios.
 *
 *      UNDER CONSTRUCTION. PROJECT FOR LEARN JAVA.
 *
 */

package mpeshell;

public class MpeShell 
{
private final static String  CONSOLE_KEY = "console";
private static boolean       consoleMode = false;

public static void main( String[] args ) 
    {
    // check command line, select Console or GUI mode
    if ( (args != null ) && ( args.length > 0 ) && ( args[0] != null ) )
        {
        if ( args[0].equals( CONSOLE_KEY ) ) 
            {
            args[0] = null;
            consoleMode = true;
            }
        }
    // run selected scenario: console or GUI    
    if ( consoleMode )
        {  // branch for console mode
        MpeConsole mp = new MpeConsole( args );
        mp.runBenchmarkConsole();
        }
    else
        {  // branch for GUI mode
        MpeGui mg = new MpeGui();
        mg.runBenchmarkGui();
        }
    }

}
