package editors;

import java.util.*;
import java.io.*;

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
     * Returns the path of the tileset files, determined with ZSDX root path.
     * @return the path of the tileset files
     */
    public String getTilesetPath() {
	return getZsdxRootPath() + File.separator + "tools" +
	    File.separator + "editors" + File.separator + "tilesets";
    }

    /**
     * Returns a tileset file from its name.
     * @param tilesetName name of a tileset
     * @return the corresponding tileset file
     */
    public File getTilesetFile(String tilesetName) {
	
	return new File(getTilesetPath() + File.separator + tilesetName + ".zsd");
    }

    /**
     * Returns the path of the map files, determined with ZSDX root path.
     * @return the path of the map files
     */
    public String getMapPath() {
	return getZsdxRootPath() + File.separator + "tools" +
	    File.separator + "editors" + File.separator + "maps";
    }
    
    /**
     * Returns a map file from its name.
     * @param mapName name of a map
     * @return the corresponding map file
     */
    public File getMapFile(String mapName) {
	
	return new File(getMapPath() + File.separator + mapName + ".zsd");
    }

    /**
     * Returns the path of the music files, determined with ZSDX root path.
     * @return the path of the music files
     */
    public String getMusicPath() {
	return getZsdxRootPath() + File.separator + "music";
    }

}
