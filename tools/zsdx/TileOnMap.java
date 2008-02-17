package zsdx;

import java.awt.*;
import java.util.*;

/**
 * Represents a tile for the map editor, i.e. a tile placed on the map.
 * A TileOnMap is composed by a Tile (as created in the tileset editor)
 * and how the tile is placed on the map: its position, its layer and
 * how the pattern is repeated or not.
 */
public class TileOnMap extends MapEntity {

    /**
     * The tileset from which this tile is extracted.
     */
    private Tileset tileset;

    /**
     * Id of the tile in the tileset.
     */
    private final int tileId;

    /**
     * Number of times the pattern is repeated on x.
     */
    private int repeatX;

    /**
     * Number of times the pattern is repeated on y.
     */
    private int repeatY;

    /**
     * Creates a new tile on the map.
     * @param map the map
     * @param tileId id of the tile in the tileset
     * @param x x position of the tile on the map
     * @param y y position of the tile on the map
     * @throws MapException if the tile is not valid
     */
    public TileOnMap(Map map, int tileId, int x, int y) throws MapException {
	super(map, LAYER_LOW, x, y, 0, 0);
	
	this.tileset = map.getTileset();
	this.tileId = tileId;

	Tile tile = tileset.getTile(tileId); // get the original tile from the tileset
	setSize(tile.getWidth(), tile.getHeight());
    }

    /**
     * Creates a tile on a map from a string.
     * @param tokenizer the string tokenizer, which has already parsed the common part of the string
     * (i.e. the layer, the coordinates and the type of entity have already been handled)
     * @param map the map
     * @throws ZSDXException if there is a syntax error in the string
     */
    public TileOnMap(StringTokenizer tokenizer, Map map) throws ZSDXException {
	super();
	
	Tile tile = null;

	try {
	    this.tileId = Integer.parseInt(tokenizer.nextToken());
	    this.repeatX = Integer.parseInt(tokenizer.nextToken());
	    this.repeatY = Integer.parseInt(tokenizer.nextToken());

	    this.tileset = map.getTileset();
	    
	    tile = map.getTileset().getTile(tileId); // get the original tile from the tileset
	    setSize(tile.getWidth() * repeatX, tile.getHeight() * repeatY);
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_TILE
     */
    public int getType() {
	return ENTITY_TILE;
    }

    /**
     * Returns whether the entity has a name.
     * @return false
     */
    public boolean hasName() {
	return false;
    }

    /**
     * Returns the number of directions of the entity.
     * @return 0 (a tile has no direction)
     */
    public int getNbDirections() {
	return 0;
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
		    positionInMap.width = newTile.getWidth() * repeatX;
		    positionInMap.height = newTile.getHeight() * repeatY;
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
     * Changes the size of the tile on the map.
     * This is a redefinition of MapEntity.setSize() to update repeatX and repeatY.
     * @param width width of the tile in pixels
     * @param height height of the tile in pixels
     * @throws MapException if the size specified is zero
     */
    public void setSize(int width, int height) throws MapException {

	super.setSize(width, height);
	
	Rectangle positionInTileset = tileset.getTile(tileId).getPositionInTileset();	
	repeatX = positionInMap.width / positionInTileset.width;
	repeatY = positionInMap.height / positionInTileset.height;

	setChanged();
	notifyObservers();
    }
    
    /**
     * Returns the minimum width of the entity (for a resizable entity).
     * When the entity is resized, its new width must be a multiple of this minimum size.
     * @return the minimum width of the entity
     */
    public int getUnitWidth() {
	Tile tile = tileset.getTile(tileId);
	return tile.getWidth();
    }

    /**
     * Returns the minimum height of the entity (for a resizable entity).
     * When the entity is resized, its new height is a multiple of this minimum height.
     * @return the minimum height of the entity
     */
    public int getUnitHeight() {
	Tile tile = tileset.getTile(tileId);
	return tile.getHeight();
    }

    /**
     * Returns the number of times the pattern is repeated on x.
     * @return the number of times the pattern is repeated on x
     */
    public int getRepeatX() {
	return repeatX;
    }

    /*
     * Returns the number of times the pattern is repeated on y.
     * @return the number of times the pattern is repeated on y
     */
    public int getRepeatY() {
	return repeatY;
    }

    /**
     * Draws the tile.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

	int scale = (int) zoom;

	// source image
	Tile tile = tileset.getTile(tileId); // get the original tile from the tileset
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
		if (showTransparency) {
		    g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, tileset);
		}
		else {
		    g.drawImage(tilesetImage, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, bgColor, tileset);
		}
	    }
	}
    }

    /**
     * Returns a string describing this tile.
     * @return a string representation of the tile
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string (i.e. the kind of entity, the layer, the coordinates)
	buff.append(super.toString());

	buff.append('\t');
	buff.append(tileId);
	buff.append('\t');
	buff.append(repeatX);
	buff.append('\t');
	buff.append(repeatY);
	return buff.toString();
    }
}
