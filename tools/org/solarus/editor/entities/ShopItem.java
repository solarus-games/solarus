/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
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
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        createStringProperty("treasure_name", false, null);
        createIntegerProperty("treasure_variant", true, 1);
        createStringProperty("treasure_savegame_variable", true, null);
        createIntegerProperty("price", false, 0);
        createStringProperty("dialog", false, null);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

        String treasureName = getStringProperty("treasure_name");
        if (treasureName == null) {
            throw new MapException("The treasure of a shop item cannot be empty");
        }

        Integer variant = getIntegerProperty("treasure_variant");
        if (variant != null && variant < 1) {
            throw new MapException("Invalid treasure variant: " + variant);
        }

        String savegameVariable = getStringProperty("treasure_savegame_variable");
        if (savegameVariable != null && !isValidSavegameVariable(savegameVariable)) {
            throw new MapException("Invalid savegame variable");
        }

        int price = getIntegerProperty("price");
        if (price <= 0) {
            throw new MapException("The price must be positive");
        }

        String dialogId = getStringProperty("dialog");
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
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
        currentImageDescription = new EntityImageDescription("miscellaneous_entities.png", 0, 64, 64, 64);
    }
}

