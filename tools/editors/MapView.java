package editors;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

/**
 * This component shows the map image and allow the user to modify it.
 */
public class MapView extends JComponent implements Observer {

    /**
     * The current map.
     */
    private Map map;

    /**
     * Constructor.
     */
    public MapView() {
	super();

	Configuration.getInstance().addObserver(this);

	setPreferredSize(new Dimension(500, 500));
    }

    /**
     * Sets the observed map.
     */
    public void setMap(Map map) {
	if (this.map != null) {
	    this.map.deleteObserver(this);
	}

	this.map = map;
	map.addObserver(this);

	update(map, null);
    }

    /**
     * Returns the size of the map.
     * @return the size of the map, or a default size
     * if no map is loaded.
     */
    public Dimension getPreferredSize() {
	int width, height;

	if (map == null) {
	    width = 640;
	    height = 480;
	}
	else {
	    width = map.getWidth() * 2;
	    height = map.getHeight() * 2;
	}
	
	return new Dimension(width, height);
    }
    
    /**
     * This function is called when the map or the configuration changes.
     */
    public void update(Observable o, Object obj) {

	if (o instanceof Map) {
	    // the map has changed
	    
	    // redraw the image
	    repaint();
	}
	else if (o instanceof Configuration && map != null) {
	    // TODO
	}
    }

    /**
     * This function is called to display the component.
     * @param g the graphic context
     */
    public void paint(Graphics g) {
	if (map == null) {
	    return;
	}

	// background color
	g.setColor(map.getBackgroundColor());
	g.fillRect(0, 0, map.getWidth() * 2, map.getHeight() * 2);

	Tileset tileset = map.getTileset();
	if (tileset != null) {

	    Image tilesetImage = tileset.getDoubleImage();
	    if (tilesetImage != null) {

		// the tiles
		for (int layer = 0; layer < 3; layer++) {
		    TileOnMapVector tiles = map.getTiles(layer);
		    for (int i = 0; i < tiles.size(); i++) {
			
			TileOnMap tileOnMap = tiles.get(i);
			Tile tile = tileset.getTile(tileOnMap.getIndex());
			Rectangle positionInTileset = tile.getPositionInTileset();
			Point positionInMap = tileOnMap.getPositionInMap();
			
			int width = positionInTileset.width;
			int height = positionInTileset.height;
			
			int sx1 = positionInTileset.x * 2;
			int sx2 = sx1 + width * 2;
			int sy1 = positionInTileset.y * 2;
			int sy2 = sy1 + height * 2;
			
			int dx1 = positionInMap.x * 2;
			int dx2 = sx1 + width * tileOnMap.getRepeatX() * 2;
			int dy1 = positionInMap.y * 2;
			int dy2 = sy1 + height * tileOnMap.getRepeatY() * 2;
			
			g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, tileset);
		    } // for
		} // for

		// selected tile
		
	    }
	}
    }
}
