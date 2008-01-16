package editors;

import java.io.*;
import java.util.*;

/**
 * This class contains the list of the game resources and their name.
 * Each resource name is associated with an id.
 * This class contains the information of the file game.zsd.
 */
public class GameResourceList extends Observable {

    private static GameResourceList instance;

    private TreeMap<Integer, String> maps;
    private int maxMapId;

    private TreeMap<Integer, String> tilesets;
    private int maxTilesetId;

    private TreeMap<String, String> musics;

    private static final String mapResourceType = "map";
    private static final String tilesetResourceType = "tileset";
    private static final String musicResourceType = "music";


    /**
     * Creates the list of the game resources from the file game.zsd.
     */
    private GameResourceList() {

	// maps
	maps = new TreeMap<Integer, String>();
	maxMapId = -1;

	tilesets = new TreeMap<Integer, String>();
	maxTilesetId = -1;

	musics = new TreeMap<String, String>();

	try {
	    load();
	}
	catch (ZSDXException ex) {

	}
    }

    public static GameResourceList getInstance() {

	if (instance == null) {
	    instance = new GameResourceList();
	}

	return instance;
    }

    /**
     * Reads the file game.zsd, i.e. the list of the game resources and their name.
     * @throws ZSDXException if the file could not be read or contains an error
     */
    public void load() throws ZSDXException {
	
	int lineNumber = 0;

	try {
	    File dbFile = Configuration.getInstance().getResourceDatabaseFile();
	    BufferedReader buff = new BufferedReader(new FileReader(dbFile));

	    musics.clear();
	    maps.clear();
	    tilesets.clear();

	    String line = buff.readLine();
	    while (line != null) {

		lineNumber++;

		StringTokenizer tokenizer = new StringTokenizer(line, "\t");
		String resourceType = tokenizer.nextToken();
		String resourceId = tokenizer.nextToken();
		String resourceName = tokenizer.nextToken();
		int id;

		if (resourceType.equals(mapResourceType)) {
		    addMap(Integer.parseInt(resourceId), resourceName);
		}
		else if (resourceType.equals(tilesetResourceType)) {
		    addTileset(Integer.parseInt(resourceId), resourceName);
		}
		else if (resourceType.equals(musicResourceType)) {
		    musics.put(resourceId, resourceName);
		}
		else {
		    throw new ZSDXException("Unknown resource type '" + resourceType + "'");
		}

		line = buff.readLine();
	    }
	    buff.close();
	}
	catch (ZSDXException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": " + ex.getMessage());
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": Integer expected");
	}
	catch (IOException ex) {
	    throw new ZSDXException(ex.getMessage());
	}
    }
    
    /**
     * Saves the list of the game resources and their names into the file game.zsd.
     * @throws ZSDXException if the file could not be written
     */
    public void save() throws ZSDXException {

	try {
	    
	    File dbFile = Configuration.getInstance().getResourceDatabaseFile();
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(dbFile)));
	    
	    Iterator<Integer> intIterator;
	    Iterator<String> stringIterator;
	    
	    // save the maps
	    intIterator = maps.keySet().iterator();
	    while (intIterator.hasNext()) {
		int id = intIterator.next();
		String name = maps.get(id);
		out.println(mapResourceType + "\t\t" + id + "\t\t" + name);
	    }
	    
	    // save the tilesets
	    intIterator = tilesets.keySet().iterator();
	    while (intIterator.hasNext()) {
		int id = intIterator.next();
		String name = tilesets.get(id);
		out.println(tilesetResourceType + "\t\t" + id + "\t\t" + name);
	    }
	    
	    // save the musics
	    stringIterator = musics.keySet().iterator();
	    while (stringIterator.hasNext()) {
		String id = stringIterator.next();
		String name = musics.get(id);
		out.println(musicResourceType + "\t\t" + id + "\t\t" + name);
	    }

	    out.close();
	}
	catch (IOException ex) {
	    throw new ZSDXException(ex.getMessage());
	}
    }

    // maps
    
    /**
     * Returns the name of a map from its id.
     * @param mapId id of a map
     * @return the name of this map
     * @throws ZSDXException if this map doesn't exist
     */
    public String getMapName(int mapId) throws ZSDXException {

	String name = maps.get(mapId);

	if (name == null) {
	    throw new ZSDXException("There is no map with id " + mapId);
	}

	return name;
    }

    /**
     * Changes the name of a map.
     * @param mapId id of a map
     * @return the name of this map
     * @throws ZSDXException if this map doesn't exist
     */
    public void setMapName(int mapId, String name) throws ZSDXException {

	String oldName = maps.get(mapID);
	if (oldName == null) {
	    throw new ZSDXException("There is no map with id " + mapId);
	}

	if (!name.equals(oldName)) {
	    maps.put(mapId, name);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Adds a new map in the database and computes an id for this map.
     * @param name name of the map to add
     * @return the id that was just assigned to the map
     */
    public int addMap(String name) {

	maxMapId++;
	maps.put(maxMapId, name);

	setChanged();
	notifyObservers();

	return maxMapId;
    }

    /**
     * Adds a map in the database, with a specified id.
     * @param mapId id of the map to add
     * @param name name of the map to add
     * @throws ZSDXException if a map already exists with this id.
     */
    private void addMap(int mapId, String name) throws ZSDXException {

	if (maps.containsKey(mapId)) {
	    throw new ZSDXException("A map with the Id " + mapId + " already exists");
	}

	maps.put(mapId, name);

	if (mapId > maxMapId) {
	    maxMapId = mapId;
	}

	setChanged();
	notifyObservers();
    }

    // tilesets

    /**
     * Returns the name of a tileset from its id.
     * @param tilesetId id of a tileset
     * @return the name of this tileset
     * @throws ZSDXException if this tileset doesn't exist
     */
    public String getTilesetName(int tilesetId) throws ZSDXException {

	String name = tilesets.get(tilesetId);

	if (name == null) {
	    throw new ZSDXException("There is no tileset with id " + tilesetId);
	}

	return name;
    }

    /**
     * Changes the name of a tileset.
     * @param tilesetId id of a tileset
     * @return the name of this tileset
     * @throws ZSDXException if this tileset doesn't exist
     */
    public void setTilesetName(int tilesetId, String name) throws ZSDXException {

	String oldName = tilesets.get(tilesetID);
	if (oldName == null) {
	    throw new ZSDXException("There is no tileset with id " + tilesetId);
	}

	if (!name.equals(oldName)) {
	    tilesets.put(tilesetId, name);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Adds a new tileset in the database and computes an id for this tileset.
     * @param name name of the tileset to add
     * @return the id that was just assigned to the tileset
     */
    public int addTileset(String name) {

	maxTilesetId++;
	tilesets.put(maxTilesetId, name);

	setChanged();
	notifyObservers();

	return maxTilesetId;
    }

    /**
     * Adds a tileset in the database, with a specified id.
     * @param tilesetId id of the tileset to add
     * @param name name of the tileset to add
     * @throws ZSDXException if a tileset already exists with this id.
     */
    private void addTileset(int tilesetId, String name) throws ZSDXException {

	if (tilesets.containsKey(tilesetId)) {
	    throw new ZSDXException("A tileset with the Id " + tilesetId + " already exists");
	}

	tilesets.put(tilesetId, name);

	if (tilesetId > maxTilesetId) {
	    maxTilesetId = tilesetId;
	}

	setChanged();
	notifyObservers();
    }

    // musics

    public String getMusicName(String musicId) {
	return musics.get(musicId);
    }

}
