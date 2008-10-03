package zsdx;

import java.awt.*;
import java.util.*;

/**
 * Represents the properties of a dungeon, like the floors
 * and the chests it contains.
 * These properties are loaded and saved from file dungeons.zsd.
 */
public class Dungeon {

    private int dungeonNumber;

    private int lowestFloor;
    private Vector<Dimension> floorSizes;

    private Vector<DungeonElement> chests;
    private Vector<DungeonElement> minibosses;
    private DungeonElement boss;

    /**
     * Creates a dungeon and loads it from file dungeons.zsd.
     * @param dungeon_number the number of the dungeon to load, between 1 and 20
     */
    public Dungeon(int world) {

    }

    /**
     * Loads the data of this dungeon from file dungeons.zsd.
     */
    public void load() {

    }
    
    /**
     * Saves the data of this dungeon in file dungeons.zsd.
     */
    public void save() {
	
    }

    // floors

    public boolean hasFloor(int floor) {
	return true;
    }


    public int getDefaultFloor() {
	
    }
    
    /**
     * Description of an element in a dungeon: chests, minibosses, boss
     */
    private class DungeonElement {
	public int savegameVariable;
	public int floor;
	public Point coordinates;
    }
}
