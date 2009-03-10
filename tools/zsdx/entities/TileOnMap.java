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
     * Creates a new tile on the map.
     * @param map the map
     * @throws MapException if the tile is not valid
     */
    public TileOnMap(Map map) throws MapException {
	super(map, 0, 0);
	this.tileset = map.getTileset();
    }

    /**
     * Sets a property specific to a tile.
     * @param name name of the property (must be "tileId")
     * @param value value of the property
     */
    public void setProperty(String name, String value) throws MapException {

	super.setProperty(name, value);

	if (tileset == null) {
	    return;
	}

	int tileId = Integer.parseInt(value);
	Tile tile = tileset.getTile(tileId); // get the original tile from the tileset
	setSize(tile.getWidth(), tile.getHeight());
	setLayer(tile.getDefaultLayer()); // TODO not for existing tiles!
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

	    int tileId = getTileId();
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
	return getIntegerProperty("tileId");
    }

    /**
     * Returns the tile's obstacle property.
     * @return the tile's obstacle property
     */
    public int getObstacle() {
	return tileset.getTile(getTileId()).getObstacle();
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
	
	Tile tile = tileset.getTile(getTileId());
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

	Tile tile = tileset.getTile(getTileId());

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

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("tileId", -1);
    }
}
