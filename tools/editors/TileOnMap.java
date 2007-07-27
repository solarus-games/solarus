package editors;

import java.awt.*; // Point
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
     * Changes the position of the tile on the map.
     * @param positionInMap the position of the tile on the map
     */
    public void setPositionInMap(Rectangle positionInMap) {
	this.positionInMap = positionInMap;
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
	this.layer = layer;
	setChanged();
	notifyObservers();
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
	this.repeatX = repeatX;
	positionInMap.width = tile.getWidth() * repeatX;
	setChanged();
	notifyObservers();
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
	this.repeatY = repeatY;
	positionInMap.height = tile.getHeight() * repeatY;
	setChanged();
	notifyObservers();
    }

}
