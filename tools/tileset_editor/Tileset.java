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
    public static final long serialVersionUID = 1L;

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
     * Number of tiles in the tileset.
     */
    private int nbTiles;

    /**
     * The tiles.
     */
    private Tile[] tiles;

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
     * Redefinition of setChanged from Observable to make a call to setSaved(false). 
     */
    public void setChanged() {
	super.setChanged();
	setSaved(false);
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
	return nbTiles;
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
     * Sets the index of the selected tile.
     * @param selectedTileIndex -1 if no tile is selected, 0 to nbTiles - 1 if an existing tile is selected,
     * or nbTiles if a new tile is selected
     */
    public void setSelectedTileIndex(int selectedTileIndex) {
	this.selectedTileIndex = selectedTileIndex;
    }
}
