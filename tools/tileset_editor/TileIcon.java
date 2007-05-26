package tileset_editor;

import java.awt.*;
import java.awt.image.*;
import javax.swing.*;
import java.util.*;

/**
 * This component shows a tile's image in 16*16 pixels.
 */
public class TileIcon extends JComponent implements Observer, ImageObserver {

    /**
     * The tile.
     */
    private Tile tile;

    /**
     * The tileset, needed to get its image.
     */
    private Tileset tileset;

    /**
     * Constructor.
     */
    public TileIcon(Tile tile, Tileset tileset) {
	this.tile = tile;
	this.tileset = tileset;
	tile.addObserver(this);
	tileset.addObserver(this);
    }

    /**
     * Returns the preferred size: 16*16.
     * @return 16*16
     */
    public Dimension getPreferredSize() {
	return new Dimension(16, 16);
    }

    /**
     * Draws the tile's image.
     * @param g graphic contewt
     */
    public void paint(Graphics g) {
	Image tilesetImage = tileset.getImage();

	if (tilesetImage != null) {
	    Rectangle positionInTileset = tile.getPositionInTileset();
	    g.drawImage(tilesetImage, 0, 0, 16, 16, positionInTileset.x, positionInTileset.y, 
			positionInTileset.x + positionInTileset.width,
			positionInTileset.y + positionInTileset.height,
			this);
	}
    }

    /**
     * This function is called when some requested information about the image comes.
     * @return true
     */
    public boolean imageUpdate(Image img, int infoflags, int x, int y, int width, int height) {
	return true;
    }

    /**
     * This function is called when the tile is changed.
     */
    public void update(Observable o, Object params) {
	repaint();
    }
}
