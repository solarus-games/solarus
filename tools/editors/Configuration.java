package editors;

import java.util.*;
import java.io.*;
import java.text.*;

/**
 * This singleton class contains the information about the editor configuration: ZSDX root path.
 */
public class Configuration extends Observable {

    /**
     * The only instance.
     */
    private static Configuration instance;

    /**
     * Root path of ZSDX.
     */
    private String zsdxRootPath;

    /**
     * Constructor.
     */
    private Configuration() {

	/* Default path : assuming that we are in the directory 'tools',
	 * ZSDX should be the parent directory.
	 */
	File defaultDirectory = new File("..");
	try {
	    zsdxRootPath = defaultDirectory.getCanonicalPath();
	}
	catch (IOException e) {
	    // just left the path blank if we couldn't get the default path
	}
    }

    /**
     * Returns the only instance.
     * @return the only instance of Configuration
     */
    public static Configuration getInstance() {

	if (instance == null) {
	    instance = new Configuration();
	}

	return instance;
    }

    /**
     * Returns the root path of ZSDX.
     * @return the root path
     */
    public String getZsdxRootPath() {
	return zsdxRootPath;
    }

    /**
     * Returns the data path of ZSDX.
     * @return the path of all data files
     */
    public String getDataPath() {
	return getZsdxRootPath() + File.separator + "data";
    }

    /**
     * Sets the root path of ZSDX.
     * @param zsdxRootPath the root path
     */
    public void setZsdxRootPath(String zsdxRootPath) {
	if (zsdxRootPath != this.zsdxRootPath) {
	    this.zsdxRootPath = zsdxRootPath;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Returns the file containing the database of the game resources (game.zsd).
     * @return the file containing the database of the game resources
     */
    public File getResourceDatabaseFile() {
	return new File(getDataPath() + File.separator + "game.zsd");

    }

    /**
     * Returns the path of the image files, determined with ZSDX root path.
     * @return the path of the image files
     */
    public String getImagePath() {
	return getDataPath() + File.separator + "images";
    }

    /**
     * Returns the path of the tileset files, determined with ZSDX root path.
     * @return the path of the tileset files
     */
    public String getTilesetPath() {
	return getDataPath() + File.separator + "tilesets";
    }

    /**
     * Returns a tileset file from its id.
     * @param tilesetId id of a tileset
     * @return the corresponding tileset file
     */
    public File getTilesetFile(String tilesetId) {
	
	NumberFormat nf = NumberFormat.getInstance();
	nf.setMinimumIntegerDigits(4);
	nf.setGroupingUsed(false);

	return new File(getTilesetPath() + File.separator +
			"tileset" + nf.format(Integer.parseInt(tilesetId)) + ".zsd");
    }

    /**
     * Returns the image file of a tileset from its id.
     * @param tilesetId id of a tileset
     * @return the corresponding tileset file
     */
    public File getTilesetImageFile(String tilesetId) {
	
	NumberFormat nf = NumberFormat.getInstance();
	nf.setMinimumIntegerDigits(4);
	nf.setGroupingUsed(false);

	return new File(getImagePath() + File.separator + "tilesets" + File.separator +
			"tileset" + nf.format(Integer.parseInt(tilesetId)) + ".png");
    }

    /**
     * Returns the path of the map files, determined with ZSDX root path.
     * @return the path of the map files
     */
    public String getMapPath() {
	return getDataPath() + File.separator + "maps";
    }

    /**
     * Returns a map file knowing its id.
     * @param mapId id of a map
     * @return the file corresponding to this id
     */
    public File getMapFile(String mapId) {

	NumberFormat nf = NumberFormat.getInstance();
	nf.setMinimumIntegerDigits(4);
	nf.setGroupingUsed(false);

	return new File(getMapPath() + File.separator +
			"map" + nf.format(Integer.parseInt(mapId)) + ".zsd");
    }

    /**
     * Returns the path of the music files, determined with ZSDX root path.
     * @return the path of the music files
     */
    public String getMusicPath() {
	return getDataPath() + File.separator + "music";
    }

}
