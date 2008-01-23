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
     * Simple constructor.
     * @param tileset the tileset
     * @param tileId id of the tile in the tileset
     * @param x x position of the tile on the map
     * @param y y position of the tile on the map
     * @throws MapException if the tile is not valid
     */
    public TileOnMap(Tileset tileset, int tileId, int x, int y) throws MapException {
	this(tileset, tileId, x, y, 1, 1, tileset.getTile(tileId).getDefaultLayer());
    }

    /**
     * Constructor.
     * @param tileset the tileset
     * @param tileId id of the tile in the tileset
     * @param x x position of the tile on the map
     * @param y y position of the tile on the map
     * @param layer layer of the tile
     * @param repeatX number of times the pattern is repeated on x
     * @param repeatY number of times the pattern is repeated on y
     * @throws MapException if repeatX or repeatY are zero
     */
    public TileOnMap(Tileset tileset, int tileId, int x, int y, int repeatX, int repeatY, int layer) throws MapException {
	super(layer, x, y, 0, 0);

	this.tileset = tileset;
	this.tileId = tileId;
	this.repeatX = repeatX;
	this.repeatY = repeatY;

	Tile tile = tileset.getTile(tileId); // get the original tile from the tileset
	setSize(tile.getWidth() * repeatX, tile.getHeight() * repeatY);
    }

    /**
     * Creates a tile on a map from a string.
     * @param description a string representing the tile, as returned by toString()
     * @param tileset the tileset
     * @throws ZSDXException if there is a syntax error in the string
     */
    public TileOnMap(String description, Tileset tileset) throws ZSDXException {
	super();
	
	int x = 0, y = 0;
	Tile tile = null;

	try {
	    StringTokenizer tokenizer = new StringTokenizer(description);
	    
	    this.tileId = Integer.parseInt(tokenizer.nextToken());
	    setLayer(Integer.parseInt(tokenizer.nextToken()));
	    x = Integer.parseInt(tokenizer.nextToken());
	    y = Integer.parseInt(tokenizer.nextToken());
	    this.repeatX = Integer.parseInt(tokenizer.nextToken());
	    this.repeatY = Integer.parseInt(tokenizer.nextToken());

	    this.tileset = tileset;
	    
	    tile = tileset.getTile(tileId); // get the original tile from the tileset
	    setPositionInMap(x, y);
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
     * Draws the tile on the map view.
     * @param g graphic context
     * @param scale scale of the image (1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, int scale, boolean showTransparency) {

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
	buff.append(tileId);
	buff.append('\t');
	buff.append(layer);
	buff.append('\t');
	buff.append(positionInMap.x);
	buff.append('\t');
	buff.append(positionInMap.y);
	buff.append('\t');
	buff.append(repeatX);
	buff.append('\t');
	buff.append(repeatY);
	return buff.toString();
    }
}
