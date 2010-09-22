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
 * Represents a shop item that the hero can buy.
 */
public class ShopItem extends MapEntity {

    /**
     * Description of the default images representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions =
    {new EntityImageDescription("miscellaneous_entities.png", 64, 32, 32, 32)};

    /**
     * Creates a new shop item.
     * @param map the map
     */
    public ShopItem(Map map) throws MapException {
	super(map, 32, 32);
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("treasureName", Item.noneId);
	setProperty("treasureVariant", 1);
	setProperty("treasureSavegameVariable", -1);
	setProperty("price", 0);
	setProperty("messageId", "");
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	String treasureName = getProperty("treasureName");

	if (treasureName.equals(Item.noneId)) {
	    throw new MapException("The content must be an existing item");
	}

	int savegameVariable = getIntegerProperty("treasureSavegameVariable");
	if (savegameVariable < -1 || savegameVariable >= 32768) {
	    throw new MapException("Invalid savegame variable");
	}

	int price = getIntegerProperty("price");
	if (price <= 0) {
	    throw new MapException("The price must be positive");
	}

	String messageId = getProperty("messageId");
	if (messageId.length() == 0) {
	    throw new MapException("No description message defined");
	}
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
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	currentImageDescription = new EntityImageDescription("miscellaneous_entities.png", 0, 64, 64, 64);
    }
}
