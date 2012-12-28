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
        setBooleanProperty("is_big_chest", false);
        setProperty("treasure_name", Item.noneId);
        setIntegerProperty("treasure_variant", null);
        setProperty("treasure_savegame_variable", null);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

        String treasureName = getProperty("treasure_name");
        if (treasureName == null) {
            throw new MapException("A treasure must be specified");
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
            throw new MapException("Invalid treasure savegame variable");
        }
    }

    /**
     * Returns whether this chest is a big chest.
     * @return true if this is a big chest
     */
    public boolean isBigChest() {
        return getBooleanProperty("is_big_chest");
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

        if (name.equals("is_big_chest")) {
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

