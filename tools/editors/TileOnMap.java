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
    public static final long serialVersionUID = 1L;

    /**
     * The tile from the tileset.
     */
    private final Tile tile;

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
     * Simple constructor.
     * @param tile the tile from the tileset
     * @param tileIndex index of the tile in the tileset
     * @param x x position of the tile on the map
     * @param y y position of the tile on the map
     */
    public TileOnMap(Tile tile, int tileIndex, int x, int y) {
	this(tile, tileIndex, x, y, tile.getDefaultLayer(), 1, 1);
    }

    /**
     * Constructor.
     * @param tile the tile from the tileset
     * @param tileIndex index of the tile in the tileset
     * @param x x position of the tile on the map
     * @param y y position of the tile on the map
     * @param layer layer of the tile
     * @param repeatX number of times the pattern is repeated on x
     * @param repeatY number of times the pattern is repeated on y
     */
    public TileOnMap(Tile tile, int tileIndex, int x, int y, int layer, int repeatX, int repeatY) {
	this.tile = tile;
	this.tileIndex = tileIndex;
	this.positionInMap = new Rectangle(x, y, tile.getWidth() * repeatX, tile.getHeight() * repeatY);
	this.layer = layer;
	this.repeatX = repeatX;
	this.repeatY = repeatY;
    }

    /**
     * Returns the tile of the tileset.
     * @return the tile of the tileset
     */
    public Tile getTile() {
	return tile;
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
     * (no other checks are performed)
     */
     public void setPositionInMap(int x1, int y1, int x2, int y2) throws MapException {

	 // check the rectangle validity
	 if (x1 == x2 || y1 == y2) {
	     throw new MapException("No rectangle defined");
	 }

	 Rectangle positionInTileset = this.tile.getPositionInTileset();

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

    /**
     * Returns the x coordinate of the tile on the map.
     * @return the x coordinate of the tile on the map
     */
    public int getX() {
	return positionInMap.x;
    }

    /**
     * Changes the x coordinate of the tile on the map.
     * @param x the x coordinate of the tile on the map
     */
    public void setX(int x) {
	if (x != positionInMap.x) {
	    positionInMap.x = x;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the y coordinate of the tile on the map.
     * @return the y coordinate of the tile on the map
     */
    public int getY() {
	return positionInMap.y;
    }

    /**
     * Changes the y coordinate of the tile on the map.
     * @param y the y coordinate of the tile on the map
     */
    public void setY(int y) {
	if (y != positionInMap.y) {
	    positionInMap.y = y;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the number of times the pattern is repeated on x.
     * @return the number of times the pattern is repeated on x
     */
    public int getRepeatX() {
	return repeatX;
    }

    /**
     * Changes the number of times the pattern is repeated on x.
     * @param repeatX the number of times the pattern is repeated on x
     */
    public void setRepeatX(int repeatX) {
	if (repeatX != this.repeatX) {
	    this.repeatX = repeatX;
	    positionInMap.width = tile.getWidth() * repeatX;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the number of times the pattern is repeated on y.
     * @return the number of times the pattern is repeated on y
     */
    public int getRepeatY() {
	return repeatY;
    }

    /**
     * Changes the number of times the pattern is repeated on y.
     * @param repeatY the number of times the pattern is repeated on y
     */
    public void setRepeatY(int repeatY) {
	if (repeatY != this.repeatY) {
	    this.repeatY = repeatY;
	    positionInMap.height = tile.getHeight() * repeatY;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Draws the tile on the map view.
     * @param g graphic context
     * @param tileset the tileset
     * @param scale scale of the image (1: unchanged, 2: zoom of 200%)
     */
    public void paint(Graphics g, Tileset tileset, int scale) {

	// source image
	Image tilesetImage = (scale == 2) ? tileset.getDoubleImage() : tileset.getImage();
	Rectangle positionInTileset = tile.getPositionInTileset();
	int sx1 = positionInTileset.x * scale;
	int sx2 = sx1 + positionInTileset.width * scale;
	int sy1 = positionInTileset.y * scale;
	int sy2 = sy1 + positionInTileset.height * scale;
	
	// destination image: we have to repeat the pattern
	
	int width = positionInTileset.width * scale;
	int height = positionInTileset.width * scale;

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
		g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, tileset);
	    }
	}

    }
}
