package tileset_editor;

import javax.swing.*;
import java.awt.*;
import java.awt.image.*;
import java.util.*;
import javax.imageio.*;
import java.io.*;

/**
 * This component shows the tileset image and allow the user to select the tiles.
 */
public class TilesetImageView extends JComponent implements Observer, ImageObserver {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * The tileset's image.
     */
    private Image tilesetImage;

    /**
     * Constructor.
     */
    public TilesetImageView() {
	super();
    }

    /**
     * Returns the tileset's image size.
     * If no image is loaded, returns a default size.
     * @return the image size or a default size.
     */
    public Dimension getPreferredSize() {
	int width, height;

	if (tilesetImage == null) {
	    width = 500;
	    height = 500;
	}
	else {
	    width = tilesetImage.getWidth(this);
	    height = tilesetImage.getHeight(this);
	}

	return new Dimension(width, height);
    }

    /**
     * Sets the observed tileset.
     */
    public void setTileset(Tileset tileset) {
	this.tileset = tileset;
	tileset.addObserver(this);

	// load the tileset's image
	reloadImage();
    }

    /**
     * Reads the image from the file.
     */
    public void reloadImage() {
	try {
	    tilesetImage = ImageIO.read(new File(tileset.getImagePath()));
	}
	catch (IOException e) {
// 	    JOptionPane.showMessageDialog(this,
// 					  "Cannot read the tileset image file: " + tileset.getImagePath(),
// 					  "Error",
// 					  JOptionPane.ERROR_MESSAGE);
	    tilesetImage = null;
	}

	update(tileset, null);
    }

    /**
     * This function is called when the tileset changes.
     */
    public void update(Observable o, Object obj) {
	repaint();
    }

    /**
     * This function is called when some requested information about the image comes.
     * @return true
     */
    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height) {
	return true;
    }

    /**
     * This function is called to display the component.
     * @param g the graphic context
     */
    public void paint(Graphics g) {
	if (tilesetImage != null) {
	    g.drawImage(tilesetImage, 0, 0, this);
	}
	else if (tileset != null) {
	    // display an error message
	    g.drawString("Unable to load image '" + tileset.getImagePath() + "'.", 10, 20);
	}
    }
}
