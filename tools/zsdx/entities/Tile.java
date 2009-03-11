package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * Represents a tile for the map editor, i.e. a tile placed on the map.
 * A Tile is composed by a TilePattern (as created in the tileset editor)
 * and how the tile is placed on the map: its position, its layer and
 * how the pattern is repeated or not.
 */
public class Tile extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = null;

    /**
     * The tileset from which this tile is extracted.
     */
    protected Tileset tileset;

    /**
     * Creates a new tile on the map.
     * @param map the map
     * @throws MapException if the tile is not valid
     */
    public Tile(Map map) throws MapException {
	super(map, 0, 0);
	this.tileset = map.getTileset();
	this.layer = -1;
    }

    /**
     * Sets a property specific to a tile.
     * @param name name of the property
     * @param value value of the property
     */
    public void setProperty(String name, String value) throws MapException {

	super.setProperty(name, value);

	if (name.equals("tilePatternId")) {
	    if (tileset == null) {
		return;
	    }

	    int tileId = Integer.parseInt(value);
	    TilePattern tilePattern = tileset.getTilePattern(tileId); // get the tile pattern from the tileset
	    setSize(tilePattern.getWidth(), tilePattern.getHeight());

	    if (layer == -1) {
		setLayer(tilePattern.getDefaultLayer());
	    }
	}
    }

    /**
     * Changes the tileset used to represent this tile on the map.
     * The corresponding tile pattern from the new tileset (i.e. the tile pattern
     * with the same id) must have exactly the same properties,
     * otherwise a TilesetException is thrown.
     * @param tileset the tileset
     * @throws TilesetException if the new tileset could not be applied
     * because the new tile pattern doesn't exist or is different.
     */
    public void setTileset(Tileset tileset) throws TilesetException {

	if (tileset != this.tileset) {

	    int tilePatternId = getTilePatternId();
	    try {
		TilePattern newTilePattern = tileset.getTilePattern(tilePatternId);

		// if a tileset was already defined, check that the
		// tile has the same properties
		if (this.tileset != null) {

		    TilePattern oldTilePattern = this.tileset.getTilePattern(tilePatternId);

		    if (!newTilePattern.equals(oldTilePattern)) {
			throw new TilesetException("Unable to apply the tileset because the tile pattern #" + tilePatternId + " is different in this tileset.");
		    }

		    // update the size on the map if the size in the tileset has changed
		    positionInMap.width = newTilePattern.getWidth() * getRepeatX();
		    positionInMap.height = newTilePattern.getHeight() * getRepeatY();
		}

		this.tileset = tileset;
	    }
	    catch (NoSuchElementException e) {
		throw new TilesetException("Unable to apply the tileset because the tile pattern #" + tilePatternId + " doesn't exist in this tileset.");
	    }
	}
    }

    /**
     * Returns the id of the tile pattern in the tileset.
     * @return the id of the tile pattern in the tileset.
     */
    public int getTilePatternId() {
	return getIntegerProperty("tilePatternId");
    }

    /**
     * Returns the tile's obstacle property.
     * @return the tile's obstacle property
     */
    public int getObstacle() {
	return tileset.getTilePattern(getTilePatternId()).getObstacle();
    }

    /**
     * Returns whether or not the entity is resizable.
     * A tile is resizable (i.e. its pattern can be replicated
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

	TilePattern tilePattern = tileset.getTilePattern(getTilePatternId());
	return tilePattern.getSize();
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

	TilePattern tilePattern = tileset.getTilePattern(getTilePatternId());

	int x = getX();
	int y = getY();
	int repeatX = getRepeatX();
	int repeatY = getRepeatY();
	int width = tilePattern.getWidth();
	int height = tilePattern.getHeight();

	for (int i = 0; i < repeatY; i++) {
	    for (int j = 0; j < repeatX; j++) {
		tilePattern.paint(g, tileset, x, y, zoom, showTransparency);
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
	setProperty("tilePatternId", -1);
    }

    /**
     * Creates a dynamic tile with the same properties than this static tile.
     * @return the dynamic tile corresponding to this static tile
     * @throws MapException if the dynamic tile could not be created
     */
    public final DynamicTile createDynamicTile() throws MapException {

	String description = toString();

	// add the name and the 'enabled' boolean
	int index = description.lastIndexOf('\t');
	StringBuffer buff = new StringBuffer(description.substring(0, index));
	buff.append('\t');
	buff.append(getDefaultNamePrefix());
	buff.append('\t');
	buff.append(getTilePatternId());
	buff.append('\t');
	buff.append('1');

	return (DynamicTile) create(map, EntityType.DYNAMIC_TILE, null); 
    }
}
