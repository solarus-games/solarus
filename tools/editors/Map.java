package editors;

import java.util.*;
import java.io.*;
import java.awt.*; // Color, Dimension

/**
 * This class describes a map.
 * A map is observable.
 */
public class Map extends Observable implements Serializable {

    /**
     * Version number of the class serialization.
     */
    public static final long serialVersionUID = 3L;

    /**
     * Name of the map.
     */
    private String name;

    /**
     * Size of the map, in pixels (the width and the height
     * should be multiples of 8).
     */
    private Dimension size;

    /**
     * Background music.
     */
    private String music;

    /**
     * Background color (default is black).
     */
    private Color backgroundColor;

    /**
     * Tileset of the map.
     * The tileset is the set of small images (tiles) used to build the map. 
     */
    private transient Tileset tileset;

    /**
     * Name of the tileset of the map.
     * The name is saved when the map is serialized, not the tileset itself.
     */
    private String tilesetName;

    /**
     * Tiles of the map.
     * This is an array of three tile vectors, one for each layer.
     */
    private TileOnMapVector[] tiles;
    
    /**
     * Tells whether the map has changed since the last save.
     * True if there has been no modifications, false otherwise.
     */
    private transient boolean isSaved; 

    /**
     * Creates a new map.
     * @param name name of the map to create
     */
    public Map(String name) {
	super();
	this.name = name;
	this.size = new Dimension(320, 240);
	this.tileset = null;
	this.tilesetName = null;
	this.music = null;
	this.backgroundColor = Color.BLACK;

	this.tiles = new TileOnMapVector[3];
	for (int i = 0; i < 3; i++) {
	    this.tiles[i] = new TileOnMapVector();
	}

	this.isSaved = false;
    }

    /**
     * Returns the name of the tileset.
     * @return the name of the tileset, for example "house"
     */
    public String getName() {
	return name;
    }

    /**
     * Returns the map size.
     * @return the map size (in pixels)
     */
    public Dimension getSize() {
	return size;
    }

    /**
     * Returns the map width.
     * @return the map width (in pixels)
     */
    public int getWidth() {
	return size.width;
    }

    /**
     * Returns the map height.
     * @return the map height (in pixels)
     */
    public int getHeight() {
	return size.height;
    }

    /**
     * Changes the map size.
     * If the new size is lower than the old one, the tiles in the removed area
     * are destroyed.
     * The width and the height must be multiples of 8.
     * @param size the new map size (in pixels)
     * @throws NumberFormatException if the width and the height are incorrect
     */
    public void setSize(Dimension size) {
	if (size.width <= 0 || size.height <= 0 || size.width % 8 != 0 || size.height % 8 != 0) {
	    throw new NumberFormatException();
	}

	this.size = size;
	setSaved(false);
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the tileset associated to this map.
     * The tileset is the set of small images (tiles) used to build the map. 
     * @return the tileset
     */
    public Tileset getTileset() {
	return tileset;
    }

    /**
     * Returns the name of the tileset associated to this map.
     * The tileset is the set of small images (tiles) used to build the map. 
     * @return the tileset name
     */
    public String getTilesetName() {
	return tilesetName;
    }

    /**
     * Changes the tileset of the map.
     * @param tileset name of the new tileset
     * @throws IOException if this tileset cannot be loaded
     */
    public void setTileset(String tilesetName) throws IOException {

	// if the tileset is removed
	if (tilesetName == null) {

	    this.tilesetName = null;
	    this.tileset = null;
	    setSaved(false);
	    setChanged();
	    notifyObservers();
	}

	// if a tileset is set for the first time, or the tileset is changed
	else if (tileset == null || !tilesetName.equals(this.tilesetName)) {
	
	    String path = Configuration.getInstance().getDefaultTilesetPath()
		+ File.separator + tilesetName + ".zsd";
	    
	    this.tileset = Tileset.load(new File(path));
	    this.tilesetName = tilesetName;
	    setSaved(false);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the total number of tiles of the map.
     * @return the total number of tiles of the map.
     */
    public int getNbTiles() {
	return tiles[Tile.LAYER_BELOW].size()
	    + tiles[Tile.LAYER_INTERMEDIATE].size()
	    + tiles[Tile.LAYER_ABOVE].size();
    }

    /**
     * Returns the tiles of the map.
     * @return an array of 3 vectors of TileOnMap: a vector for each layer
     */
    public TileOnMapVector[] getTiles() {
	return tiles;
    }

    /**
     * Returns the tiles of the map on a given layer.
     * @param layer the layer: Tile.LAYER_BELOW, Tile.LAYER_INTERMEDIATE or Tile.LAYER_ABOVE
     * @return the vector of TileOnMap for this layer
     */
    public TileOnMapVector getTiles(int layer) {
	return tiles[layer];
    }

    /**
     * Adds a new tile on the map.
     * @param tile the tile to add
     */
    public void addTile(TileOnMap tile) {
	tiles[tile.getLayer()].add(tile);
	setSaved(false);
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the default background music of the map.
     * @return the name of the new music, i.e. a music file name without the extension ".it".
     */
    public String getMusic() {
	return music;
    }

    /**
     * Changes the default background music of the map.
     * @param music the name of the music, i.e. a music file name without the extension ".it".
     */
    public void setMusic(String music) {
	this.music = music;
	setSaved(false);
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the background color of the map.
     * @return the background color
     */
    public Color getBackgroundColor() {
	return backgroundColor;
    }

    /**
     * Changes the background color of the map.
     * @param backgroundColor the new background color
     */
    public void setBackgroundColor(Color backgroundColor) {
	this.backgroundColor = backgroundColor;
	setSaved(false);
	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether or not the tileset has changed since the last save.
     * @return true if there has been no modifications, false otherwise
     */
    public boolean isSaved() {
	return isSaved;
    }

    /**
     * Sets whether the tileset has changed since the last save.
     * @param isSaved true if there has been no modifications, false otherwise
     */
    public void setSaved(boolean isSaved) {
	this.isSaved = isSaved;
    }

    /**
     * Loads a map from the map file.
     */
    public static Map load(File mapFile) throws IOException {

 	// open the map file
	ObjectInputStream in = new ObjectInputStream(new FileInputStream(mapFile));
	Map map = null;

	// read the object
	try {
	    map = (Map) in.readObject();
	    map.setTileset(map.getTilesetName());
	}
	catch (ClassNotFoundException e) {
	    System.err.println("Unable to read the object: " + e.getMessage());
	    e.printStackTrace();
	    System.exit(1);
	}

	in.close();

	map.setSaved(true);

	return map;
    }

    /**
     * Saves the data into the map file.
     */
    public static void save(File mapFile, Map map) throws IOException {

 	// open the map file
	ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream(mapFile));

	// write the object
	out.writeObject(map);
	out.close();

	map.setSaved(true);
    }
}
