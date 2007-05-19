package tileset_editor;

import java.util.*;
import java.io.*;

/**
 * This class describes a tileset.
 */
public class Tileset extends Observable implements Serializable {

    /**
     * Version number of the class serialization.
     */
    public static final long serialVersionUID = 2L;

    // common data for all tilesets 

    /**
     * Root path of ZSDX.
     */
    private static String zsdxRootPath;

    // tileset data 

    /**
     * Name of the tileset.
     */
    private String name;

    /**
     * The tiles.
     */
    private Vector<Tile> tiles;

    // information about the user actions on the tileset

    /**
     * Tells whether the tileset has changed since the last save.
     * True if there has been no modifications, false otherwise.
     */
    private transient boolean isSaved; 

    /**
     * Index of the tile currently selected by the user.
     * -1: no tile is selected
     * 0 to nbTiles - 1: an existing tile is selected
     * nbTiles: a new tile is selected, ready to be added
     */
    private transient int selectedTileIndex;

    /**
     * Creates a new tileset.
     * @param tilesetFile file of the tileset (should be a one)
     * @param name name of the tileset to create
     */
    public Tileset(String name) {
	super();
	this.name = name;
	this.isSaved = false;
	this.selectedTileIndex = -1; // none
	tiles = new Vector<Tile>();

	tiles.add(new Tile(new Rectangle(16, 32, 16, 16), Tile.OBSTACLE, Tile.NO_ANIMATION, 0));
    }

    /**
     * Returns the root path of ZSDX.
     * @return the root path
     */
    public static String getZsdxRootPath() {
	return zsdxRootPath;
    }

    /**
     * Sets the root path of ZSDX.
     * @param path the root path
     */
    public static void setZsdxRootPath(String path) {
	zsdxRootPath = path;
    }

    /**
     * Returns the default path of the tileset files, determined with ZSDX root path.
     * @return the default path of the tileset files
     */
    public static String getDefaultTilesetPath() {
	return getZsdxRootPath() + File.separator + "tools" +
	    File.separator + "tileset_editor" +
	    File.separator + "tilesets";
    }

    /**
     * Returns the name of the tileset.
     * @return the name of the tileset, for example "house"
     */
    public String getName() {
	return name;
    }

    /**
     * Returns the path of the file containing the tileset's image.
     * @return the image file path
     */
    public String getImagePath() {
	return zsdxRootPath + File.separator + "images" +
	    File.separator + "tilesets" + File.separator + name + ".png";
    }

    /**
     * Returns the number of tiles in the tileset.
     * @return the number of tiles
     */
    public int getNbTiles() {
	return tiles.size();
    }

    /**
     * Returns a tile.
     * @param index index of the tile to get
     * @return the tile with this index
     */
    public Tile getTile(int index) {
	return tiles.get(index);
    }

    /**
     * Returns the index of the selected tile.
     * @return -1 if no tile is selected, 0 to nbTiles - 1 if an existing tile is selected,
     * or nbTiles if a new tile is selected
     */
    public int getSelectedTileIndex() {
	return selectedTileIndex;
    }

    /**
     * Sets the index of the selected tile and notifies the observers.
     * Does nothing if the index is not changed.
     * @param selectedTileIndex -1 if no tile is selected, 0 to nbTiles - 1 if an existing tile is selected,
     * or nbTiles if a new tile is selected
     */
    public void setSelectedTileIndex(int selectedTileIndex) {
	if (selectedTileIndex != this.selectedTileIndex) {
	    this.selectedTileIndex = selectedTileIndex;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the selected tile.
     * @return the selected tile, or null if there is no selected tile or if doesn't exist yet
     */
    public Tile getSelectedTile() {
	if (selectedTileIndex >= 0 && selectedTileIndex < getNbTiles()) {
	    return getTile(selectedTileIndex);
	}
	else {
	    return null;
	}	
    }

    /**
     * Returns whether the tileset has changed since the last save.
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
     * Loads a tileset from the tileset file.
     */
    public static Tileset load(File tilesetFile) throws IOException {

 	// open the tileset file
	ObjectInputStream in = new ObjectInputStream(new FileInputStream(tilesetFile));
	Tileset tileset = null;

	// read the object
	try {
	    tileset = (Tileset) in.readObject();
	}
	catch (ClassNotFoundException e) {
	    System.err.println("Unable to read the object: " + e.getMessage());
	    e.printStackTrace();
	    System.exit(1);
	}

	in.close();

	tileset.setSaved(true);
	tileset.setSelectedTileIndex(-1); // none

	return tileset;
    }

    /**
     * Saves the data into the tileset file.
     */
    public static void save(File tilesetFile, Tileset tileset) throws IOException {

 	// open the tileset file
	ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream(tilesetFile));

	// write the object
	out.writeObject(tileset);
	out.close();

	tileset.setSaved(true);
    }

}
