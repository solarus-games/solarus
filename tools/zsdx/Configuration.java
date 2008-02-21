package zsdx;

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
    private static Configuration instance = new Configuration();
    
    static {
	try {
	    ResourceDatabase.load();
	}
	catch (ZSDXException ex) {
	    // no resource database was found at this location
	}
    }

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
	    zsdxRootPath = defaultDirectory.getCanonicalPath() + File.separator;
	}
	catch (IOException ex) {
	    zsdxRootPath = "." + File.separator;
	}
    }

    /**
     * Returns the only instance.
     * @return the only instance of Configuration
     */
    public static Configuration getInstance() {
	return instance;
    }

    /**
     * Returns the root path of ZSDX.
     * @return the root path
     */
    public static String getZsdxRootPath() {
	return instance.zsdxRootPath;
    }

    /**
     * Sets the root path of ZSDX and loads the project located at this path.
     * @param zsdxRootPath the root path
     * @param ZSDXException if the specified path doesn't contain a valid ZSDX project
     */
    public static void setZsdxRootPath(String zsdxRootPath) throws ZSDXException {
	instance.zsdxRootPath = zsdxRootPath;

	try {
	    ResourceDatabase.load();
	}
	finally {
	    instance.setChanged();
	    instance.notifyObservers();
	}
    }

    /**
     * Returns the data path of ZSDX.
     * @return the path of all data files
     */
    public static String getDataPath() {
	return getZsdxRootPath() + File.separator + "data";
    }

    /**
     * Returns the file containing the database of the game resources (game.zsd).
     * @return the file containing the database of the game resources
     */
    public static File getResourceDatabaseFile() {
	return new File(getDataPath() + File.separator + "game.zsd");

    }

    /**
     * Returns the path of the image files, determined with ZSDX root path.
     * @return the path of the image files
     */
    public static String getImagePath() {
	return getDataPath() + File.separator + "images";
    }

    /**
     * Returns the path of the tileset files, determined with ZSDX root path.
     * @return the path of the tileset files
     */
    public static String getTilesetPath() {
	return getDataPath() + File.separator + "tilesets";
    }

    /**
     * Returns a tileset file from its id.
     * @param tilesetId id of a tileset
     * @return the corresponding tileset file
     */
    public static File getTilesetFile(String tilesetId) {
	
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
    public static File getTilesetImageFile(String tilesetId) {
	
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
    public static String getMapPath() {
	return getDataPath() + File.separator + "maps";
    }

    /**
     * Returns a map file knowing its id.
     * @param mapId id of a map
     * @return the file corresponding to this id
     */
    public static File getMapFile(String mapId) {

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
    public static String getMusicPath() {
	return getDataPath() + File.separator + "music";
    }

}
