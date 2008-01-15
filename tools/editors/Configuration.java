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
	return new File(getZsdxRootPath() + File.separator + "data" + File.separator + "game.zsd");

    }

    /**
     * Returns the path of the tileset files, determined with ZSDX root path.
     * @return the path of the tileset files
     */
    public String getTilesetPath() {
	return getZsdxRootPath() + File.separator + "data" + File.separator + "tilesets";
    }

    /**
     * Returns a tileset file from its Id.
     * @param tilesetId id of a tileset
     * @return the corresponding tileset file
     */
    public File getTilesetFile(int tilesetId) {
	
	NumberFormat nf = NumberFormat.getInstance();
	nf.setMinimumIntegerDigits(4);
	nf.setGroupingUsed(false);

	return new File(getTilesetPath() + File.separator + "tilesets" + File.separator +
			"tileset" + nf.format(tilesetId) + ".zsd");
    }

    /**
     * Returns the path of the map files, determined with ZSDX root path.
     * @return the path of the map files
     */
    public String getMapPath() {
	return getZsdxRootPath() + File.separator + "data" + File.separator + "maps";
    }

    /**
     * Returns a map file knowing its id.
     * @param mapId id of a map
     * @return the file corresponding to this id
     */
    public File getMapFile(int mapId) {

	NumberFormat nf = NumberFormat.getInstance();
	nf.setMinimumIntegerDigits(4);
	nf.setGroupingUsed(false);

	return new File(getMapPath() + File.separator + "maps" + File.separator +
			"map" + nf.format(mapId) + ".zsd");
    }

    /**
     * Returns the path of the music files, determined with ZSDX root path.
     * @return the path of the music files
     */
    public String getMusicPath() {
	return getZsdxRootPath() + File.separator + "data" + File.separator + "music";
    }

}
