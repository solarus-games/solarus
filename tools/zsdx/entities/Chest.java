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
package zsdx.entities;

import zsdx.*;

/**
 * Represents a chest placed on the map.
 * Specific properties of a chest:
 * - bigChest
 * - content
 * - amount
 * - savegameVariable
 */
public class Chest extends MapEntity {

    /**
     * Description of the default images representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions =
	{new EntityImageDescription("chest.png", 0, 0, 16, 16)};

    /**
     * Creates a new chest.
     * @param map the map
     */
    public Chest(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("bigChest", false);
	setProperty("content", TreasureContent.NOTHING.getId());
	setProperty("amount", 1);
	setProperty("savegameVariable", 0);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	boolean bigChest = isBigChest();

	if (bigChest && !map.isInDungeon()) {
		throw new MapException("Cannot have a big chest outside a dungeon");
	}

	TreasureContent content = TreasureContent.get(getIntegerProperty("content"));
 
	if (content.mustBeInDungeon() && !map.isInDungeon()) {
	    throw new MapException("This kind of treasure can only exist in a dungeon");
	}

	if (content == TreasureContent.SMALL_KEY && !map.hasSmallKeys()) {
	    throw new MapException("The small keys are not enabled for this map. Please enable them first by choosing a variable to save them in this map.");
	}

	int savegameVariable = getIntegerProperty("savegameVariable");
	if (savegameVariable < -1 || savegameVariable >= 32768) {
	    throw new MapException("Invalid savegame variable");
	}

	int amount = getIntegerProperty("amount");
	if (amount < 0) {
	    throw new MapException("The amount cannot be negative");
	}

	if (content.hasAmount() && amount <= 0) {
	    throw new MapException("This kind of treasure must have a positive amount");
	}
    }

    /**
     * Returns whether this chest is a big chest.
     * @return true if this is a big chest
     */
    public boolean isBigChest() {
	return getBooleanProperty("bigChest");
    }

    /**
     * Returns whether this entity is an obstacle.
     * @return Obstacle.OBSTACLE
     */
    public Obstacle getObstacle() {
	return Obstacle.OBSTACLE;
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Sets a property specific to this kind of entity.
     * @param name name of the property
     * @param value value of the property
     */
    public void setProperty(String name, String value) throws MapException {

	super.setProperty(name, value);

	if (name.equals("bigChest")) {
            if (isBigChest()) {
	        setSizeImpl(32, 24);
	    }
	    else {
	        setSizeImpl(16, 16);
	    }
	}
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	
        if (isBigChest()) {
	    currentImageDescription.setRectangle(0, 16, 32, 24);
	}
	else {
	    currentImageDescription.setRectangle(0, 0, 16, 16);
	}
    }
}
