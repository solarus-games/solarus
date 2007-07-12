package editors;

import java.util.*;
import java.io.*;

/**
 * This class describes a map.
 * A map is observable. When it changes, the observers are notified with
 * a parameter indicating what has just changed:
 *   TODO
 */
public class Map extends Observable implements Serializable {

    /**
     * Version number of the class serialization.
     */
    public static final long serialVersionUID = 1L;

    /**
     * Name of the map.
     */
    private String name;

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
	this.isSaved = false;
    }

    /**
     * Returns the default path of the map files, determined with ZSDX root path.
     * @return the default path of the map files
     */
    public static String getDefaultMapPath() {
	return Configuration.getInstance().getZsdxRootPath() + File.separator + "tools" +
	    File.separator + "editors" + File.separator + "maps";
    }

    /**
     * Returns the name of the tileset.
     * @return the name of the tileset, for example "house"
     */
    public String getName() {
	return name;
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

 	// open the tileset file
	ObjectOutputStream out = new ObjectOutputStream(new FileOutputStream(mapFile));

	// write the object
	out.writeObject(map);
	out.close();

	map.setSaved(true);
    }
}
