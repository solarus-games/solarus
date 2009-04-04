/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package zsdx;

import org.ini4j.Ini;
import java.awt.*;
import java.util.*;
import java.io.*;
import zsdx.entities.*;

/**
 * Represents the properties of a dungeon, like the floors
 * and the chests it contains.
 * These properties are loaded and saved from file dungeons.zsd.
 */
public class Dungeon {

    private int dungeonNumber;

    private int lowestFloor;
    private Vector<Dimension> floorSizes;

    private int nbChestsSaved;
    private int nbBossesSaved;

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

	    Ini ini = new Ini(new FileReader(fileName));

	    // parse the floors (the floors must be before the chests and the bosses)
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
     * Removes from file dungeons.zmc any element (chests, or bosses)
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
     * Saves in file dungeons.zmc the dungeon elements (chests and bosses)
     * of the specified map.
     * @param ini the ini file
     * @param map a map
     */
    private void saveDungeonElements(Ini ini, Map map) throws IOException {

	nbChestsSaved = 0;
	nbBossesSaved = 0;

	MapEntities[] allEntities = map.getAllEntities();
	for (int layer = 0; layer < MapEntity.LAYER_NB; layer++) {
	    for (MapEntity entity: allEntities[layer].getDynamicEntities()) {

		if (entity instanceof Chest) {
		    Chest chest = (Chest) entity;
		    saveDungeonElement(ini, map, "chest_" + nbChestsSaved,
			    chest.getX(), chest.getY(),
			    chest.getIntegerProperty("savegameVariable"), chest.isBigChest());
		    nbChestsSaved++;
		}
		else if (entity instanceof Enemy) {
		    Enemy enemy = (Enemy) entity;
		    Enemy.Rank rank = Enemy.Rank.get(enemy.getIntegerProperty("rank"));
		    
		    if (rank != Enemy.Rank.NORMAL) {
			saveDungeonElement(ini, map, "boss_" + nbBossesSaved,
				enemy.getX(), enemy.getY(), enemy.getIntegerProperty("savegameVariable"),
				rank == Enemy.Rank.BOSS); 
			nbBossesSaved++;
		    }
		}
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
     * @param big is this a big chest/boss
     * @param savegameIndex index of the boolean that stores this element's state
     */
    private void saveDungeonElement(Ini ini, Map map, String name, int x, int y, int savegameIndex, boolean big) {

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
	section.put("big", big ? "1" : "0");

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
}
