package editors;

import java.awt.*;
import java.io.*;
import java.util.*;

/**
 * Represents a tile for the map editor, i.e. a tile placed on the map.
 * A TileOnMap is composed by a Tile (as created in the tileset editor)
 * and how the tile is placed on the map: its position, its layer and
 * how the pattern is repeated or not.
 */
public class TileOnMap extends Observable implements Serializable {

    /**
     * Version number of the class serialization.
     */
    public static final long serialVersionUID = 2L;

    /**
     * The tileset from which this tile is extracted.
     */
    private transient Tileset tileset;

    /**
     * Index of the tile in the tileset.
     * It is used only to generate the C++ code.
     */
    private final int tileIndex;

    /**
     * Position of the tile in the map.
     */
    private Rectangle positionInMap;

    /**
     * Layer of the map.
     */
    private int layer;

    /**
     * Number of times the pattern is repeated on x.
     */
    private int repeatX;

    /**
     * Number of times the pattern is repeated on y.
     */
    private int repeatY;

    /**
     * Color to display instead of the transparent pixels of the image.
     */
    private static final Color bgColor = new Color(128, 128, 255);

    /**
     * Simple constructor.
     * @param tileset the tileset
     * @param tileIndex index of the tile in the tileset
     * @param x x position of the tile on the map
     * @param y y position of the tile on the map
     */
    public TileOnMap(Tileset tileset, int tileIndex, int x, int y) {
	this(tileset, tileIndex, x, y, tileset.getTile(tileIndex).getDefaultLayer(), 1, 1);
    }

    /**
     * Constructor.
     * @param tileset the tileset
     * @param tileIndex index of the tile in the tileset
     * @param x x position of the tile on the map
     * @param y y position of the tile on the map
     * @param layer layer of the tile
     * @param repeatX number of times the pattern is repeated on x
     * @param repeatY number of times the pattern is repeated on y
     */
    public TileOnMap(Tileset tileset, int tileIndex, int x, int y, int layer, int repeatX, int repeatY) {
	this.tileset = tileset;
	this.tileIndex = tileIndex;
	this.layer = layer;
	this.repeatX = repeatX;
	this.repeatY = repeatY;

	Tile tile = tileset.getTile(tileIndex); // get the original tile from the tileset
	this.positionInMap = new Rectangle(x, y, tile.getWidth() * repeatX, tile.getHeight() * repeatY);
    }

    /**
     * Changes the tileset used to represent this tile on the map.
     * The corresponding tile from the new tileset (i.e. the tile
     * with the same index) must have exactly the same properties,
     * otherwise a TilesetException is thrown.
     * @param tileset the tileset
     * @throws TilesetException if the new tileset could not be applied
     * because the new tile doesn't exist or is different.
     */
    public void setTileset(Tileset tileset) throws TilesetException {
	
	if (tileset != this.tileset) {
		
	    try {
		Tile newTile = tileset.getTile(tileIndex);
		
		// if a tileset was already defined, check that the
		// tile has the same properties
		if (this.tileset != null) {
		    
		    Tile oldTile = this.tileset.getTile(tileIndex);
		    
		    if (!newTile.equals(oldTile)) {
			throw new TilesetException("Unable to apply the tileset because the tile #" + tileIndex + " is different in this tileset.");
		    }

		    // update the size on the map if the size in the tileset has changed
		    positionInMap.width = newTile.getWidth() * repeatX;
		    positionInMap.height = newTile.getHeight() * repeatY;
		}
		
		this.tileset = tileset;
	    }
	    catch (ArrayIndexOutOfBoundsException e) {
		throw new TilesetException("Unable to apply the tileset because the tile #" + tileIndex + "doesn't exist in this tileset.");
	    }
	}
    }

    /**
     * Returns the index of the tile in the tileset.
     * @return the index of the tile in the tileset.
     */
    public int getTileIndex() {
	return tileIndex;
    }

    /**
     * Returns the position of the tile on the map.
     * @return the position of the tile on the map
     */
    public Rectangle getPositionInMap() {
	return positionInMap;
    }

    /**
     * Changes the position of the tile on the map, by specifying two points.
     * The tile is resized (i.e. repeatX and repeatY are updated) so that
     * the tile fits exactly in the rectangle formed by the two points.
     * @param x1 x coordinate of the first point
     * @param y1 y coordinate of the first point
     * @param x2 x coordinate of the second point
     * @param y2 y coordinate of the second point
     * @throws MapException if the rectangle width or its height is zero
     * (no other verifications are performed)
     */
    public void setPositionInMap(int x1, int y1, int x2, int y2) throws MapException {
	
	// check the rectangle validity
	if (x1 == x2 || y1 == y2) {
	    throw new MapException("No rectangle defined");
	}
	
	Rectangle positionInTileset = tileset.getTile(tileIndex).getPositionInTileset();
	
	// x
	positionInMap.x = Math.min(x1, x2);
	positionInMap.width = Math.abs(x2 - x1);
	repeatX = positionInMap.width / positionInTileset.width;
	
	// y
	positionInMap.y = Math.min(y1, y2);
	positionInMap.height = Math.abs(y2 - y1);
	repeatY = positionInMap.height / positionInTileset.height;
	
	// notify
	setChanged();
	notifyObservers();
    }
    
    /**
     * Changes the position of the tile on the map, by specifying a point.
     * The size of the tile is not changed.
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     */
    public void setPositionInMap(int x, int y) throws MapException {

	positionInMap.x = x;
	positionInMap.y = y;
	
	// notify
	setChanged();
	notifyObservers();
    }

    /**
     * Changes the position of the selected tiles.
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     */
    public void move(int dx, int dy) {
	
	positionInMap.x += dx;
	positionInMap.y += dy;

	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether or not a point is in the tile.
     * @param x x of the point
     * @param y y of the point
     * @return true if the point is in the tile, false otherwise
     */
    public boolean containsPoint(int x, int y) {
	return positionInMap.contains(x, y);
    }

    /**
     * Returns the layer of the tile.
     * @return the layer of the tile: Tile.LAYER_BELOW (most of the tiles),
     * Tile.LAYER_INTERMEDIATE or Tile.LAYER_ABOVE.
     */
    public int getLayer() {
	return layer;
    }

    /**
     * Changes the layer of the tile.
     * @param layer the layer of the tile: Tile.LAYER_BELOW (most of the tiles),
     * Tile.LAYER_INTERMEDIATE or Tile.LAYER_ABOVE.
     */
    public void setLayer(int layer) {
	if (layer != this.layer) {
	    this.layer = layer;
	    setChanged();
	    notifyObservers();
	}
    }

//     /**
//      * Returns the x coordinate of the tile on the map.
//      * @return the x coordinate of the tile on the map
//      */
//     public int getX() {
// 	return positionInMap.x;
//     }

//     /**
//      * Changes the x coordinate of the tile on the map.
//      * @param x the x coordinate of the tile on the map
//      */
//     public void setX(int x) {
// 	if (x != positionInMap.x) {
// 	    positionInMap.x = x;
// 	    setChanged();
// 	    notifyObservers();
// 	}
//     }

//     /**
//      * Returns the y coordinate of the tile on the map.
//      * @return the y coordinate of the tile on the map
//      */
//     public int getY() {
// 	return positionInMap.y;
//     }

//     /**
//      * Changes the y coordinate of the tile on the map.
//      * @param y the y coordinate of the tile on the map
//      */
//     public void setY(int y) {
// 	if (y != positionInMap.y) {
// 	    positionInMap.y = y;
// 	    setChanged();
// 	    notifyObservers();
// 	}
//     }

//     /**
//      * Returns the number of times the pattern is repeated on x.
//      * @return the number of times the pattern is repeated on x
//      */
//     public int getRepeatX() {
// 	return repeatX;
//     }

//     /**
//      * Changes the number of times the pattern is repeated on x.
//      * @param repeatX the number of times the pattern is repeated on x
//      */
//     public void setRepeatX(int repeatX) {
// 	if (repeatX != this.repeatX) {
// 	    this.repeatX = repeatX;
// 	    positionInMap.width = tile.getWidth() * repeatX;
// 	    setChanged();
// 	    notifyObservers();
// 	}
//     }

//     /**
//      * Returns the number of times the pattern is repeated on y.
//      * @return the number of times the pattern is repeated on y
//      */
//     public int getRepeatY() {
// 	return repeatY;
//     }

//     /**
//      * Changes the number of times the pattern is repeated on y.
//      * @param repeatY the number of times the pattern is repeated on y
//      */
//     public void setRepeatY(int repeatY) {
// 	if (repeatY != this.repeatY) {
// 	    this.repeatY = repeatY;
// 	    positionInMap.height = tile.getHeight() * repeatY;
// 	    setChanged();
// 	    notifyObservers();
// 	}
//     }

    /**
     * Draws the tile on the map view.
     * @param g graphic context
     * @param tileset the tileset
     * @param scale scale of the image (1: unchanged, 2: zoom of 200%)
     */
    public void paint(Graphics g, int scale) {

	// source image
	Tile tile = tileset.getTile(tileIndex); // get the original tile from the tileset
	Rectangle positionInTileset = tile.getPositionInTileset();
	int sx1 = positionInTileset.x * scale;
	int sx2 = sx1 + positionInTileset.width * scale;
	int sy1 = positionInTileset.y * scale;
	int sy2 = sy1 + positionInTileset.height * scale;

	Image tilesetImage = (scale == 2) ? tileset.getDoubleImage() : tileset.getImage();
	
	// destination image: we have to repeat the pattern
	
	int width = positionInTileset.width * scale;
	int height = positionInTileset.height * scale;

	int dx1;
	int dx2;
	int dy1;
	int dy2;
	
	dx2 = positionInMap.x * scale;
	for (int j = 0; j < repeatX; j++) {
	    dx1 = dx2;
	    dx2 += width;
	    dy2 = positionInMap.y * scale;
	    for (int k = 0; k < repeatY; k++) {
		dy1 = dy2;
		dy2 += height;
		g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, bgColor, tileset);
	    }
	}

    }
}
