/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Handler for "Report" button, save text report to file.
 *
 */

package mpeshell.service;

import java.io.File;
import java.io.FileWriter;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.filechooser.FileNameExtensionFilter;
import javax.swing.table.AbstractTableModel;
import mpeshell.MpeGui;

public class ActionReport 
{
    
private final MpeGui mg;
public ActionReport( MpeGui x )
    {
    mg = x;
    }

public void saveReportDialogue()
    {
    // get window context
    JFrame parentWin = mg.getFrame();
    String logText = mg.getTextArea().getText();
    AbstractTableModel briefTable = 
        mg.getTableModel();
    AbstractTableModel detailTable = 
        mg.getChildStatistics().getTableModel();
    // initializing file operations context
    final String DEFAULT_FILE_NAME = "report.txt";
    JFileChooser chooser = new JFileChooser();
    chooser.setDialogTitle( "Report - select directory" );
    FileNameExtensionFilter filter = 
        new FileNameExtensionFilter ( "Text files" , "txt" );
    chooser.setFileFilter( filter );
    chooser.setFileSelectionMode( JFileChooser.FILES_ONLY );
    chooser.setSelectedFile( new File( DEFAULT_FILE_NAME ) );
    // (re)start dialogue
    boolean inDialogue = true;
    while( inDialogue )
        {
        int select = chooser.showSaveDialog( parentWin );
        // save file
        if( select == JFileChooser.APPROVE_OPTION )
            {
            String filePath = chooser.getSelectedFile().getPath();
            int option = JOptionPane.YES_OPTION;
            // check file exist and warning message
            File file = new File( filePath );
            if( file.exists() == true )
                {
                option = JOptionPane.showConfirmDialog
                    ( null, 
                    "File exist: " + filePath + "\noverwrite?" , "REPORT" ,
                    JOptionPane.YES_NO_CANCEL_OPTION ,
                    JOptionPane.WARNING_MESSAGE );  // or QUESTION_MESSAGE
                }
            // Select operation by user selection
            if ( ( option == JOptionPane.NO_OPTION  ) |
                 ( option == JOptionPane.CLOSED_OPTION ) )
                { 
                continue; 
                }
            if ( option == JOptionPane.CANCEL_OPTION ) 
                { 
                inDialogue = false;
                continue; 
                }
            
            // continue prepare for save file.
            StringBuilder data = new StringBuilder ( "" );
            // add product and copyright information to report
            data.append( About.getLongName() );
            data.append( "\r\n" );
            data.append( About.getWebSite() );
            data.append( "\r\n" );
            data.append( "report file." );
            data.append( "\r\n\r\n" );

            // brief report, text same as openable full report
            data.append( "TEXT LOG.\r\n" );
            if ( logText != null )
                {
                int n = logText.length();
                boolean b = false;
                for( int i=0; i<n; i++ )
                    {
                    // sequences 0Ah/0Dh, 0Dh/0Ah, 0Dh, 0Ah
                    // must be replaced to sequence 0Dh/0Ah
                    char c = logText.charAt( i );
                    if ( ( c == '\r' ) || ( c == '\n' ) )
                        {
                        if ( b )
                            {
                            b = false;
                            }
                        else
                            {
                            data.append( "\r\n" );
                            b = true;
                            }
                        }
                    else
                        {
                        data.append( c );
                        b = false;
                        }
                    }
                }
            data.append( "\r\n\r\n" );
            
            // brief table
            data.append( "BRIEF STATISTICS.\r\n" );
            saveTable( briefTable, data );
            
            // openable full table
            data.append( "DETAIL STATISTICS.\r\n" );
            saveTable( detailTable, data );

            // save text report
            String fileData = data.toString();
            int status = 0;
            try ( FileWriter writer = new FileWriter( filePath, false ) )
                {
                writer.write( fileData );
                writer.flush(); 
                }
            catch( Exception ex )
                {
                status = 1; 
                }
            // message box after report saved
            if ( status == 0 )  
                {
                JOptionPane.showMessageDialog
                ( parentWin, "Report saved: " + filePath, "REPORT",
                JOptionPane.WARNING_MESSAGE ); 
                }
            else
                {
                JOptionPane.showMessageDialog
                ( parentWin, "Write report failed", "ERROR",
                JOptionPane.ERROR_MESSAGE ); 
                }
            inDialogue = false;
            }  
        else
            { 
            inDialogue = false; 
            }
        }
        
    }

// helper method for save table
private void saveTable( AbstractTableModel table, StringBuilder data )
    {
    if ( table != null )
        {
        int columns = table.getColumnCount();
        int rows = table.getRowCount();
        int colSize;
        int[] maxColSizes = new int[columns];
        int maxColSize = 13;
        // Get column names lengths
        for ( int i=0; i<columns; i++ )
            {
            maxColSizes[i] = table.getColumnName(i).length(); 
            }
        // Get column maximum lengths
        for ( int j=0; j<rows; j++ )
            {
            for ( int i=0; i<columns; i++ )
                {
                colSize = (( String )( table.getValueAt( j, i ) )).length();
                if ( colSize > maxColSizes[i] )
                    {
                    maxColSizes[i] = colSize; 
                    }
                }
            }
        for ( int i=0; i<maxColSizes.length; i++ ) 
            {
            maxColSize += maxColSizes[i];
            }
        // Write table up
        for ( int i=0; i<columns; i++ )
            {
            data.append( " " );
            data.append( table.getColumnName( i ) );
            colSize = maxColSizes[i] - table.getColumnName( i ).length() + 1;
            for ( int k=0; k<colSize; k++ )
                {
                data.append( " " );
                }
            }
        // Write horizontal line
        data.append("\r\n" );
        for ( int i=0; i<maxColSize; i++ )
            {
            data.append( "-" );
            }
        data.append("\r\n" );
    // Write table content
    for (int j=0; j<rows; j++)         // this cycle for rows
        {
        for (int i=0; i<columns; i++)  // this cycle for columns
            {
            data.append( " " );
            data.append( table.getValueAt( j, i ) );
            colSize = maxColSizes[i] - 
                ( ( String )table.getValueAt( j, i )).length() + 1;
            for ( int k=0; k<colSize; k++ )
                {
                data.append( " " );
                }
            }
            data.append( "\r\n" );
        }
        // Write horizontal line
        for ( int i=0; i<maxColSize; i++ )
            {
            data.append( "-" );
            }
        data.append( "\r\n\r\n" );
        }
    }

}
