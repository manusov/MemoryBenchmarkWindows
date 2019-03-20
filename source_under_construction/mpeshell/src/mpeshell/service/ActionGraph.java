/*
 *
 * Memory Performance Engine (MPE) Shell. (C)2019 IC Book Labs.
 * Handler for "Graph" button, save graphics image to file.
 *
 */

package mpeshell.service;

import java.awt.Graphics2D;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;
import javax.imageio.ImageIO;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.filechooser.FileNameExtensionFilter;
import mpeshell.MpeGui;

public class ActionGraph 
{

private final MpeGui mg;
public ActionGraph( MpeGui x )
    {
    mg = x;
    }

public void saveGraphicsDialogue()
    {
    // get window context
    JFrame parentWin = mg.getFrame();
    // get objects for save
    JPanel displayWin = 
        mg.getChildDraw().getController().getView().getPanel();
    // generate graphics image from drawings GUI panel
    int w = displayWin.getWidth();
    int h = displayWin.getHeight();
    // check for run save image when drawings window opened
    if ( ( w <= 0 ) || ( h <= 0 ) )
        {
        JOptionPane.showMessageDialog
            ( parentWin,
              "Drawings window must\r\nbe opened for write image\r\n" +
              "first press \" Draw > \"",
              "ERROR",
              JOptionPane.ERROR_MESSAGE ); 
        return;
        }
    
    // initializing file operations context
    final String DEFAULT_FILE_NAME = "picture.png";
    JFileChooser chooser = new JFileChooser();
    chooser.setDialogTitle( "Save graphics - select directory" );
    FileNameExtensionFilter filter = 
        new FileNameExtensionFilter ( "Picture files" , "png" );
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
                    "File exist: " + filePath + "\noverwrite?" , "IMAGE" ,
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
            
            BufferedImage image = 
                new BufferedImage( w, h, BufferedImage.TYPE_INT_RGB );
            Graphics2D g = image.createGraphics();
            g.setClip( 0, 0, w, h );
            displayWin.paint( g );
            g.drawImage( image, w, h, displayWin );
            g.dispose();
            // save graphics image to file
            int status = 0;
            try
                {
                ImageIO.write( image, "png", new File( filePath ) );
                }
            catch( IOException ex )
                {
                status = 1; 
                }
            // message box after report saved
            if ( status == 0 )  
                {
                JOptionPane.showMessageDialog
                ( parentWin, "Image saved: " + filePath, "IMAGE",
                JOptionPane.WARNING_MESSAGE ); 
                }
            else
                {
                JOptionPane.showMessageDialog
                ( parentWin, "Write image failed", "ERROR",
                JOptionPane.ERROR_MESSAGE ); 
                }
            inDialogue = false;
            }  
        else
            { 
            inDialogue = false; 
            }
        }   // End of save dialogue cycle
    }
}
