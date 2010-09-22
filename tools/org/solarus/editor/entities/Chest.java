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
package org.solarus.editor.entities;

import org.solarus.editor.*;

/**
 * Represents a chest placed on the map.
 * Specific properties of a chest:
 * - bigChest
 * - treasureName
 * - treasureVariant
 * - treasureSavegameVariable
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
	setProperty("treasureName", Item.noneId);
	setProperty("treasureVariant", 1);
	setProperty("treasureSavegameVariable", -1);
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

	String treasureName = getProperty("treasureName");
	if (treasureName.equals(Item.randomId)) {
	    throw new MapException("The treasure of a chest cannot be random");
	}

	int savegameVariable = getIntegerProperty("treasureSavegameVariable");
	if (savegameVariable < -1 || savegameVariable >= 32768) {
	    throw new MapException("Invalid savegame variable");
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
