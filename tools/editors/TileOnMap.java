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
     * Index of the tile in the tileset.
     */
    private final int tileIndex;

    /**
     * Coordinates of the tile in the map.
     */
    private Point positionInMap;

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
     * Simple constructor, without repeating the pattern.
     */
    public TileOnMap(int tileIndex, Point positionInMap, int layer) {
	this(tileIndex, positionInMap, layer, 1, 1);
    }

    /**
     * Constructor.
     */
    public TileOnMap(int tileIndex, Point positionInMap, int layer, int repeatX, int repeatY) {
	this.tileIndex = tileIndex;
	this.positionInMap = positionInMap;
	this.layer = layer;
	this.repeatX = repeatX;
	this.repeatY = repeatY;
    }

    /**
     * Returns the index of the tile in the tileset.
     * @return the index of the tile in the tileset.
     */
    public int getIndex() {
	return tileIndex;
    }

    /**
     * Returns the position of the tile on the map.
     * @return the position of the tile on the map
     */
    public Point getPositionInMap() {
	return positionInMap;
    }


    /**
     * Changes the position of the tile on the map.
     * @param positionInMap the position of the tile on the map
     */
    public void setPositionInMap(Point positionInMap) {
	this.positionInMap = positionInMap;
	setChanged();
	notifyObservers();
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
	setChanged();
	notifyObservers();
    }

}
