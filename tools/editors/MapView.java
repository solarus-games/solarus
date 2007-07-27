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
     * X of the tile image displayd under the pointer in the map image.
     * -1 indicates that no tile image is displayed under the pointer
     */
    private int cursorX = -1;
    
    /**
     * Y of the tile image displayd under the pointer in the map image.
     * -1 indicates that no tile image is displayed under the pointer
     */
    private int cursorY = -1;

    /**
     * Constructor.
     */
    public MapView() {
	super();

	Configuration.getInstance().addObserver(this);

	addMouseListener(new MapMouseListener());
	addMouseMotionListener(new MapMouseMotionListener());
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
     * Returns true if the image exists.
     * @return true if the map image exists
     */
    public boolean isImageLoaded() {
	return map != null && map.getTileset() != null && map.getTileset().getDoubleImage() != null;
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
			int dx2 = dx1 + width * tileOnMap.getRepeatX() * 2;
			int dy1 = positionInMap.y * 2;
			int dy2 = dy1 + height * tileOnMap.getRepeatY() * 2;
			
			g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, tileset);
		    } // for
		} // for

		// selected tile in the tileset
		Tile selectedTileInTileset = tileset.getSelectedTile();
		if (selectedTileInTileset != null && cursorX != -1 && cursorY != -1) {
		    Rectangle positionInTileset = selectedTileInTileset.getPositionInTileset();

		    int width = positionInTileset.width;
		    int height = positionInTileset.height;
			
		    int sx1 = positionInTileset.x * 2;
		    int sx2 = sx1 + width * 2;
		    int sy1 = positionInTileset.y * 2;
		    int sy2 = sy1 + height * 2;
			
		    int dx1 = cursorX;
		    int dx2 = dx1 + width * 2;
		    int dy1 = cursorY;
		    int dy2 = dy1 + height * 2;
			
		    g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, tileset);
		}
	    }
	}
    }

    /**
     * The mouse listener associated to the map image.
     */
    private class MapMouseListener extends MouseAdapter {

	/**
	 * This method is called when the mouse exits the map view.
	 * If a tile is selected in the tileset, it is still displayed at
	 * the last cursor location on the map view, to we have to remove it.
	 */
	public void mouseExited(MouseEvent mouseEvent) {
	    if (cursorX != -1 || cursorY != -1) {
		cursorX = -1;
		cursorY = -1;
		repaint();
	    }
	}

	/**
	 * This method is called when the mouse is clicked on the map view.
	 * If a tile is selected in the tileset, an instance of this tile is added to the map
	 * at the cursor location.
	 */
	public void mouseClicked(MouseEvent mouseEvent) {

	    // place a new tile
	    if (isImageLoaded() && cursorX != -1 && cursorY != -1) {
		Tileset tileset = map.getTileset();
		int selectedTileIndex = tileset.getSelectedTileIndex();
		Point positionInMap = new Point(cursorX / 2, cursorY / 2);
		map.addTile(new TileOnMap(selectedTileIndex, positionInMap, Tile.LAYER_BELOW));
		tileset.setSelectedTileIndex(selectedTileIndex);
	    }
	}
    }

    /**
     * The mouse motion listener associated to the map image.
     */
    private class MapMouseMotionListener extends MouseMotionAdapter {

	/**
	 * This method is called when the cursor is moved onto the map view.
	 * If a tile is selected in the tileset, it is displayed under the cursor.
	 */
	public void mouseMoved(MouseEvent mouseEvent) {

	    if (isImageLoaded()) {

		int x = mouseEvent.getX();
		int y = mouseEvent.getY();

		// if the mouse is outside the map, remove the tile displayed under the cursor
		if (x >= map.getWidth() * 2 || y >= map.getHeight() * 2) {

		    cursorX = -1;
		    cursorY = -1;
		    repaint();
		}
		else {

		    // else display the tile under the cursor
		    Tileset tileset = map.getTileset();
		    Tile selectedTileInTileset = tileset.getSelectedTile();
		    
		    if (selectedTileInTileset != null) {
			// snap the tile to the 16*16 grid
			x -= x % 16;
			y -= y % 16;
			
			if (x != cursorX || y != cursorY) {
			    cursorX = x;
			    cursorY = y;
			    repaint();
			}
		    }
		}
	    }
	}
    }
}
