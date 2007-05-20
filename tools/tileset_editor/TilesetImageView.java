package tileset_editor;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
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
     * The rectangle drawn by the user in order to create a new tile.
     */
    private Rectangle newTileArea;

    /**
     * Constructor.
     */
    public TilesetImageView() {
	super();
	addMouseListener(new TilesetImageMouseListener());
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

	if (tilesetImage != null) { // the image is loaded

	    // draw the image
	    g.drawImage(tilesetImage, 0, 0, this);

	    // determine the selected area
	    int selectedTileIndex = tileset.getSelectedTileIndex();
	    Rectangle selectedRectangle = null;
	    if (selectedTileIndex == tileset.getNbTiles()) {
		// the selected tile doesn't exist yet
		selectedRectangle = newTileArea;		
	    }
	    else if (selectedTileIndex > -1) {
		// an existing tile is selected
		selectedRectangle = tileset.getTile(selectedTileIndex).getPositionInTileset();
	    }
	    
	    // draw the selected rectangle
	    if (selectedRectangle != null) {
		int x1 = selectedRectangle.x;
		int x2 = selectedRectangle.x + selectedRectangle.width - 1;
		int y1 = selectedRectangle.y;
		int y2 = selectedRectangle.y + selectedRectangle.height - 1;

		g.setColor(Color.MAGENTA);
		g.drawLine(x1, y1, x2, y1);
		g.drawLine(x2, y1, x2, y2);
		g.drawLine(x2, y2, x1, y2);
		g.drawLine(x1, y2, x1, y1);
	    }

	}
	else if (tileset != null) {

	    // a tileset is open but the image doesn't exist: display an error message
	    g.drawString("Unable to load image '" + tileset.getImagePath() + "'.", 10, 20);
	}
    }

    /**
     * The mouse listener associated to the tileset image.
     */
    private class TilesetImageMouseListener extends MouseAdapter {
	
	/**
	 * This function is called when the user clicks on the tileset image.
	 * Only left clicks are considered.
	 * If no tile was selected, then the tile clicked (if any) is selected.
	 * If a tile was selected, it becomes unselected.
	 * @param mouseEvent information about the click
	 */
	public void mouseClicked(MouseEvent mouseEvent) {

	    // only consider left clicks
	    if (tilesetImage != null && mouseEvent.getButton() == MouseEvent.BUTTON1) {

		int x = mouseEvent.getX();
		int y = mouseEvent.getY();

		if (tileset.getSelectedTileIndex() == -1) {
		    // no tile was selected, search a tile at the click location
		    
		    boolean found = false;
		    for (int i = 0; i < tileset.getNbTiles() && !found; i++) {

			Rectangle tileRectangle = tileset.getTile(i).getPositionInTileset();
			if (tileRectangle.isPointInside(x, y)) {
			    tileset.setSelectedTileIndex(i);
			    found = true;
			}
		    }
		}
		else {
		    // a tile was selected: unselect it
		    tileset.setSelectedTileIndex(-1);
		    newTileArea = null;
		}
	    }
	}
    }
}
