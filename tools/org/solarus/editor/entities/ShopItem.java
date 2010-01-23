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
	setProperty("content", TreasureContent.NOTHING.getId());
	setProperty("amount", 1);
	setProperty("savegameVariable", -1);
	setProperty("price", 0);
	setProperty("messageId", "");
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	TreasureContent content = TreasureContent.get(getIntegerProperty("content"));

	if (content == TreasureContent.NOTHING) {
	    throw new MapException("The content must be an existing item");
	}

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
