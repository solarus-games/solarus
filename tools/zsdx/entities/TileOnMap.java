package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * Represents a tile for the map editor, i.e. a tile placed on the map.
 * A TileOnMap is composed by a Tile (as created in the tileset editor)
 * and how the tile is placed on the map: its position, its layer and
 * how the pattern is repeated or not.
 */
public class TileOnMap extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = null;

    /**
     * The tileset from which this tile is extracted.
     */
    private Tileset tileset;

    /**
     * Id of the tile in the tileset.
     */
    private int tileId;

    /**
     * Creates a new tile on the map.
     * @param map the map
     * @param tileId id of the tile in the tileset
     * @param x x position of the tile on the map
     * @param y y position of the tile on the map
     * @throws MapException if the tile is not valid
     */
    public TileOnMap(Map map, int tileId, int x, int y) throws MapException {
	super(map, LAYER_LOW, x, y, 0, 0, false);

	this.tileset = map.getTileset();
	this.tileId = tileId;
	Tile tile = tileset.getTile(tileId); // get the original tile from the tileset
	setSize(tile.getWidth(), tile.getHeight());
	setLayer(tile.getDefaultLayer());
    }

    /**
     * Creates an existing tile on a map from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws IllegalArgumentException if a tile on the map doesn't exist in the tileset
     * @throws ZSDXException if there is a syntax error in the string 
     */
    public TileOnMap(Map map, StringTokenizer tokenizer) throws IllegalArgumentException, ZSDXException {
	super(map, tokenizer);
	
	try {
	    this.tileId = Integer.parseInt(tokenizer.nextToken());
	    this.tileset = map.getTileset();

	    // check the tile size on the map (only now because we didn't have the tileset before)
	    checkSize(getWidth(), getHeight());
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
	catch (IllegalArgumentException ex) {
	    throw new TilesetException(ex.getMessage());
	}
    }

    /**
     * Returns a string describing this tile.
     * @return a string representation of the tile
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// add the specific properties of a tile
	buff.append('\t');
	buff.append(tileId);
	return buff.toString();
    }

    /**
     * Changes the tileset used to represent this tile on the map.
     * The corresponding tile from the new tileset (i.e. the tile
     * with the same id) must have exactly the same properties,
     * otherwise a TilesetException is thrown.
     * @param tileset the tileset
     * @throws TilesetException if the new tileset could not be applied
     * because the new tile doesn't exist or is different.
     */
    public void setTileset(Tileset tileset) throws TilesetException {
	
	if (tileset != this.tileset) {
		
	    try {
		Tile newTile = tileset.getTile(tileId);
		
		// if a tileset was already defined, check that the
		// tile has the same properties
		if (this.tileset != null) {
		    
		    Tile oldTile = this.tileset.getTile(tileId);
		    
		    if (!newTile.equals(oldTile)) {
			throw new TilesetException("Unable to apply the tileset because the tile #" + tileId + " is different in this tileset.");
		    }

		    // update the size on the map if the size in the tileset has changed
		    positionInMap.width = newTile.getWidth() * getRepeatX();
		    positionInMap.height = newTile.getHeight() * getRepeatY();
		}
		
		this.tileset = tileset;
	    }
	    catch (NoSuchElementException e) {
		throw new TilesetException("Unable to apply the tileset because the tile #" + tileId + " doesn't exist in this tileset.");
	    }
	}
    }

    /**
     * Returns the id of the tile in the tileset.
     * @return the id of the tile in the tileset.
     */
    public int getTileId() {
	return tileId;
    }

    /**
     * Returns the tile's obstacle property.
     * @return OBSTACLE_NONE, OBSTACLE, OBSTACLE_TOP_RIGHT,
     * OBSTACLE_TOP_LEFT, OBSTACLE_BOTTOM_LEFT or OBSTACLE_BOTTOM_RIGHT
     */
    public int getObstacle() {
	return tileset.getTile(tileId).getObstacle();
    }

    /**
     * Returns whether or not the entity is resizable.
     * A tile on a map is resizable (i.e. its pattern can be replicated
     * horizontally or vertically).
     * @return true
     */
    public boolean isResizable() {
	return true;
    }
    
    /**
     * Returns the minimum size of the entity (for a resizable entity).
     * When the entity is resized, its new size must be a multiple of this minimum size.
     * @return the minimum size of the entity
     */
    public Dimension getUnitarySize() {
	
	if (tileset == null) { // special case when the tileset is not initialized yet
	    return super.getUnitarySize();
	}
	
	Tile tile = tileset.getTile(tileId);
	return tile.getSize();
    }

    /**
     * Returns the number of times the pattern is repeated on x.
     * @return the number of times the pattern is repeated on x
     */
    public int getRepeatX() {
	return positionInMap.width / getUnitarySize().width;
    }

    /**
     * Returns the number of times the pattern is repeated on y.
     * @return the number of times the pattern is repeated on y
     */
    public int getRepeatY() {
	return positionInMap.height / getUnitarySize().height;
    }

    /**
     * Draws the tile.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

	Tile tile = tileset.getTile(tileId);

	int x = getX();
	int y = getY();
	int repeatX = getRepeatX();
	int repeatY = getRepeatY();
	int width = tile.getWidth();
	int height = tile.getHeight();

	for (int i = 0; i < repeatY; i++) {
	    for (int j = 0; j < repeatX; j++) {
		tile.paint(g, tileset, x, y, zoom, showTransparency);
		x += width;
	    }
	    y += height;
	    x = getX();
	}
    }
}
