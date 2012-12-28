/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
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
        setProperty("treasure_name", null);
        setIntegerProperty("treasure_variant", null);
        setProperty("treasure_savegame_variable", null);
        setIntegerProperty("price", 0);
        setProperty("dialog", null);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

        String treasureName = getProperty("treasure_name");
        if (treasureName == null
                || treasureName.equals(Item.noneId)) {
            throw new MapException("The treasure of a shop item cannot be empty or random");
        }

        Integer variant = getIntegerProperty("treasure_variant");
        if (!treasureName.equals(Item.noneId)
                && (variant == null || variant < 1)) {
            throw new MapException("A variant must be defined with this treasure");
        }

        if (variant != null && variant < 1) {
            throw new MapException("Invalid treasure variant: " + variant);
        }

        String savegameVariable = getProperty("treasure_savegame_variable");
        if (savegameVariable != null && !savegameVariable.isEmpty()) {
            throw new MapException("Invalid savegame variable");
        }

        int price = getIntegerProperty("price");
        if (price <= 0) {
            throw new MapException("The price must be positive");
        }

        String dialogId = getProperty("dialog");
        if (dialogId == null || dialogId.length() == 0) {
            throw new MapException("No description dialog defined");
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

