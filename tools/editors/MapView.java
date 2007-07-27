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
     * X of the tile image displayed under the pointer in the map image.
     * -1 indicates that no tile image is displayed under the pointer
     */
    private int cursorX = -1;
    
    /**
     * Y of the tile image displayed under the pointer in the map image.
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
	if (map.getTileset() != null) {
	    map.getTileset().addObserver(this);
	}

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
     * This function is called when the map, the tileset or the configuration changes.
     */
    public void update(Observable o, Object obj) {

	if (o instanceof Map) {
	    // the map has been modified

	    if (obj instanceof Tileset) {
		// the tileset has changed
		Tileset tileset = map.getTileset();
		tileset.addObserver(this);
		update(tileset, null);
	    }
	    
	    // redraw the image
	    repaint();
	}
	else if (o instanceof Configuration && map != null) {
	    // TODO
	}
	else if (o instanceof Tileset) {
	    // the selected tile in the tileset has changed
	    
	    Tileset tileset = map.getTileset();
	    if (tileset.getSelectedTile() != null && map.getSelectedTile() != null) {

		// if a tile was just selected in the tileset whereas there is already
		// a tile selected in the map, unselected the tile in the map
		map.setSelectedTile(null);
	    }
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
			Tile tile = tileOnMap.getTile();

			// source image
			Rectangle positionInTileset = tile.getPositionInTileset();
			int sx1 = positionInTileset.x * 2;
			int sx2 = sx1 + positionInTileset.width * 2;
			int sy1 = positionInTileset.y * 2;
			int sy2 = sy1 + positionInTileset.height * 2;
			
			// destination image
			Rectangle positionInMap = tileOnMap.getPositionInMap();
			int dx1 = positionInMap.x * 2;
			int dx2 = dx1 + positionInMap.width * 2;
			int dy1 = positionInMap.y * 2;
			int dy2 = dy1 + positionInMap.height * 2;
			
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

		// selected tile in the map
		else {
		    
		    TileOnMap selectedTileInMap = map.getSelectedTile();
		    if (selectedTileInMap != null) {

			g.setColor(Color.GREEN);

			Rectangle selectedRectangle = selectedTileInMap.getPositionInMap();
			int x1 = selectedRectangle.x * 2;
			int x2 = (selectedRectangle.x + selectedRectangle.width) * 2 - 1;
			int y1 = selectedRectangle.y * 2;
			int y2 = (selectedRectangle.y + selectedRectangle.height) * 2 - 1;

			g.drawLine(x1, y1, x2, y1);
			g.drawLine(x2, y1, x2, y2);
			g.drawLine(x2, y2, x1, y2);
			g.drawLine(x1, y2, x1, y1);
			
			g.drawLine(x1, y1 + 1, x2, y1 + 1);
			g.drawLine(x2 - 1, y1, x2 - 1, y2);
			g.drawLine(x2, y2 - 1, x1, y2 - 1);
			g.drawLine(x1 + 1, y2, x1 + 1, y1);
		    }
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

	    if (!isImageLoaded()) {
		return;
	    }

	    // place a new tile
	    if (cursorX != -1 && cursorY != -1) {
		Tileset tileset = map.getTileset();
		map.addTile(new TileOnMap(tileset.getSelectedTile(), tileset.getSelectedTileIndex(),
					  cursorX / 2, cursorY / 2));
	    }

	    // select an existing tile
	    else {
		int x = mouseEvent.getX();
		int y = mouseEvent.getY();
		map.setSelectedTile(x / 2, y / 2);
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
