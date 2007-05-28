package tileset_editor;

import java.awt.*;
import javax.swing.*;
import java.util.*;

/**
 * This component shows a tile's icon.
 */
public class TileIcon extends JComponent implements Observer {

    private static final Color bgColorSelected = new Color(128, 128, 255);
    private static final Color bgColorUnselected = Color.WHITE;

    /**
     * The tile.
     */
    private Tile tile;

    /**
     * The tileset, needed to get its image.
     */
    private Tileset tileset;

    /**
     * Coordinates of the tile as displayed by the icon.
     */
    private int dx1, dx2, dy1, dy2;

    /**
     * Coordinates of the tile's icon in the tileset double image.
     */
    private int sx1, sx2, sy1, sy2;

    /**
     * Constructor.
     */
    public TileIcon(Tile tile, Tileset tileset) {
	this.tile = tile;
	this.tileset = tileset;
	tile.addObserver(this);
	tileset.addObserver(this);

	update(tile, null);
	update(tileset, null);
    }

    /**
     * Returns the preferred size: 48*48.
     * @return 48*48
     */
    public Dimension getPreferredSize() {
	return new Dimension(48, 48);
    }

    /**
     * Draws the tile's image.
     * @param g graphic context
     */
    public void paint(Graphics g) {
	Image tilesetImage = tileset.getDoubleImage();

	if (tilesetImage != null) {
	    Color bgColor = (tileset.getSelectedTile() == tile) ? bgColorSelected : bgColorUnselected;
	    g.setColor(bgColor);
	    g.fillRect(0, 0, 47, 47);
	    g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, tileset);
	}
    }

    /**
     * This function is called when the tile is changed.
     */
    public void update(Observable o, Object params) {

	if (o instanceof Tile) { // the tile has changed

	    Rectangle positionInTileset = tile.getPositionInTileset();
	    int width = positionInTileset.width;
	    int height = positionInTileset.height;

	    // coordinates of the tile in the tileset
	    sx1 = positionInTileset.x * 2;
	    sx2 = sx1 + width * 2;
	    sy1 = positionInTileset.y * 2;
	    sy2 = sy1 + height * 2;

	    // coordinates of the tile in the icon

	    // check the tile dimensions (we need to draw the tile 
	    // into a 16*16 square)
	    if (width > 16 || height > 16) {

		// the image will be rescaled to fit in the 16*16 square
		// but we need to keep the same ratio height-width
		double ratio = ((double) height) / width;
		if (height > width) {
		    height = 16;
		    width = (int) (height / ratio);
		}
		else {
		    width = 16;
		    height = (int) (width * ratio);
		}
	    }

	    // now the dimensions fit in the 32*32 square
	    dx1 = 24 - width;
	    dx2 = 23 + width;
	    dy1 = 24 - height;
	    dy2 = 23 + height;
	}

	repaint();
    }
}
