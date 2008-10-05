package zsdx;

import org.ini4j.Ini;
import java.awt.*;
import java.util.*;
import java.io.*;

/**
 * Represents the properties of a dungeon, like the floors
 * and the chests it contains.
 * These properties are loaded and saved from file dungeons.zsd.
 */
public class Dungeon {

    private int dungeonNumber;

    private int lowestFloor;
    private Vector<Dimension> floorSizes;
/*
    private Vector<DungeonElement> chests;
    private Vector<DungeonElement> minibosses;
    private DungeonElement boss;
*/
    private int nbChestsSaved;
    private int nbMinibossesSaved;

    /**
     * Creates a dungeon and loads it from file dungeons.zsd.
     * @param dungeon_number the number of the dungeon to load, between 1 and 20
     */
    public Dungeon(int dungeon_number) {
	this.dungeonNumber = dungeon_number;
	load();
    }

    /**
     * Returns the ini file name.
     * @return the ini file name
     */
    private static String getFileName() {
	return Project.getDataPath() + "/dungeons.zsd";
    }

    /**
     * Loads the data of this dungeon from file dungeons.zsd.
     */
    private void load() {

	String fileName = getFileName();
	try {
	    lowestFloor = 100;
	    floorSizes = new Vector<Dimension>();

	    /*
	    chests = new Vector<DungeonElement>();
	    minibosses = new Vector<DungeonElement>();
	    */

	    Ini ini = new Ini(new FileReader(fileName));

	    // parse the floors (the floors must be before the chests, minibosses and boss)
	    String floorPrefix = "dungeon_" + dungeonNumber + ".floor_";
	    for (Ini.Section section: ini.values()) {
		String sectionName = section.getName();
		if (sectionName.startsWith(floorPrefix)) {

		    String suffix = sectionName.substring(floorPrefix.length());
		    int floor = Integer.parseInt(suffix);
		    int width = Integer.parseInt(section.get("width"));
		    int height = Integer.parseInt(section.get("height"));
		    floorSizes.add(new Dimension(width, height));

		    if (floor < lowestFloor) {
			lowestFloor = floor;
		    }
		}
	    }

	    // parse the dungeon elements: chests, minibosses, boss
	    /* useless in the editor, actually!
	    String elementsPrefix = "dungeon_" + dungeonNumber + ".map_";
	    for (Ini.Section section: ini.values()) {
		String sectionName = section.getName();
		if (sectionName.startsWith(elementsPrefix)) {

		    String suffix = sectionName.substring(floorPrefix.length());
		    DungeonElement element = new DungeonElement();
		    element.floor = Integer.parseInt(section.get("floor"));
		    element.x = Integer.parseInt(section.get("x"));
		    element.y = Integer.parseInt(section.get("y"));
		    element.savegameIndex = Integer.parseInt(section.get("save"));

		    if (suffix.contains("chest")) {
			chests.add(element);
		    }
		    else if (suffix.contains("miniboss")) {
			minibosses.add(element);
		    }
		    else if (suffix.contains("boss")) {
			boss = element;
		    }
		    else {
			throw new IllegalArgumentException("Unknown element " + sectionName);
		    }
		}
	    }
	    */
	}
	catch (IOException ex) {
	    System.err.println("Cannot load the dungeon file " + fileName + ": " + ex.getMessage());
	    System.exit(1);
	}
	catch (IllegalArgumentException ex) {
	    System.err.println("Cannot load the dungeon file " + fileName + ": " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Saves the floor elements in the dungeons.zsd file.
     */
    public void saveFloorElements() {
	// TODO
    }

    /**
     * Saves the data of a map of this dungeon in file dungeons.zsd.
     * This function is called when a map has just been saved. It updates the dungeons.zsd file
     * with the new information of that map.
     * @param map a map of this dungeon
     */
    public static void saveMapInfo(Map map) {

	String fileName = getFileName();
	try {
	    Ini ini = new Ini(new FileReader(fileName));
	    removeDungeonElements(ini, map);

	    if (map.isInDungeon()) {
		Dungeon dungeon = map.getDungeon();
		dungeon.saveDungeonElements(ini, map);
	    }
	    ini.store(new FileWriter(fileName));
	}
	catch (IOException ex) {
	    System.err.println("Cannot save the map information in the dungeon file " + fileName + ": " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Removes from file dungeons.zmc any element (chests, minibosses or boss)
     * referencing the specified map.
     * @param ini the ini file
     * @param map a map
     */
    private static void removeDungeonElements(Ini ini, Map map) throws IOException {

	String mapString = "map_" + map.getId();
	for (Ini.Section section: ini.values()) {
	    String sectionName = section.getName();
	    if (sectionName.contains(mapString)) {
		ini.remove(section);
	    }
	}
    }

    /**
     * Saves in file dungeons.zmc the dungeon elements (chests, minibosses or boss)
     * of the specified map.
     * @param ini the ini file
     * @param map a map
     */
    private void saveDungeonElements(Ini ini, Map map) throws IOException {

	nbChestsSaved = 0;
	nbMinibossesSaved = 0;

	MapEntities[] allEntities = map.getAllEntities();
	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {
	    for (ActiveEntity entity: allEntities[layer].getActiveEntities()) {

		if (entity instanceof Chest) {
		    Chest chest = (Chest) entity;
		    saveDungeonElement(ini, map, "chest_" + nbChestsSaved,
			    chest.getX(), chest.getY(), chest.getSavegameIndex());
		    nbChestsSaved++;
		}
		// TODO: miniboss, boss
	    }
	}
    }

    /**
     * Saves into the dungeons.zsd file an element of a map (chest, miniboss or boss).
     * @param ini the ini file
     * @param map the map
     * @param name name of this element (ex: "chest_4", "miniboss_1" or "boss")
     * @param x x position of the element
     * @param y y position of the element
     * @param savegameIndex index of the boolean that stores this element's state
     */
    private void saveDungeonElement(Ini ini, Map map, String name, int x, int y, int savegameIndex) {

	StringBuffer sectionName = new StringBuffer();
	sectionName.append("dungeon_");
	sectionName.append(dungeonNumber);
	sectionName.append('.');
	sectionName.append("map_");
	sectionName.append(map.getId());
	sectionName.append('.');
	sectionName.append(name);
	
	Ini.Section section = ini.new Section(sectionName.toString());
	section.put("floor", Integer.toString(map.getFloor()));
	section.put("x", Integer.toString(x));
	section.put("y", Integer.toString(y));
	section.put("savegameIndex", Integer.toString(savegameIndex));

	ini.put(sectionName.toString(), section);
    }

    // floors

    /**
     * Returns the number of floors in this dungeon.
     */
    public int getNbFloors() {
	return floorSizes.size();
    }

    /**
     * Returns the lowest floor in this dungeon.
     * @return the lowest floor, between -16 and 15
     */
    public int getLowestFloor() {
	return lowestFloor;
    }

    /**
     * Returns the highest floor in this dungeon.
     * @return the highest floor, between -16 and 15
     */
    public int getHighestFloor() {
	return lowestFloor + getNbFloors() - 1;
    }

    /**
     * Returns a default floor, to propose initially to the user when he
     * associates a map to this dungeon.
     * @return a default floor
     */
    public int getDefaultFloor() {

	int result;

	if (hasFloor(0)) {
	    result = 0;
	}

	else if (getLowestFloor() > 0) {
	    result = getLowestFloor();
	}

	else {
	    result = getHighestFloor();
	}

	return result;
    }

    /**
     * Returns whether a floor exists in this dungeon
     * @param floor a floor (between -16 and 15)
     * @return true if this floor exists in this dungeon
     */
    public boolean hasFloor(int floor) {
	return floor >= getLowestFloor() && floor <= getHighestFloor();
    }

    /**
     * Describes an element of the dungeon (chest, miniboss or boss)
     * as specified in the dungeons.zsd file.
     */
    /*
    private class DungeonElement {
        public int floor;
        public int x;
        public int y;
        public int savegameIndex;

        public DungeonElement() {
            
        }
    }*/
}
