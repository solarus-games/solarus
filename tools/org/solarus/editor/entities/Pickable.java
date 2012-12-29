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

import java.awt.*;
import org.solarus.editor.*;

/**
 * Represents a treasure that the hero can pick on the ground.
 */
public class Pickable extends MapEntity {


    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("pickable_items.png", 0, 0, 16, 16)
        // TODO load the sprite instead (for now we always show a green rupee)
    };

    /**
     * Origin point of a pickable item.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * Creates a new pickable item.
     * @param map the map
     */
    public Pickable(Map map) throws MapException {
        super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (8,13)
     */
    protected Point getOrigin() {
        return origin;
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
        setProperty("treasure_name", null);
        setIntegerProperty("treasure_variant", null);
        setIntegerProperty("treasure_savegame_variable", null);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

        String treasureName = getProperty("treasure_name");

        if (treasureName == null) {
            throw new MapException("The treasure of a pickable item cannot be empty");
        }

        Integer variant = getIntegerProperty("treasure_variant");
        if (treasureName != null && (variant == null || variant < 1)) {
            throw new MapException("A variant must be defined with this treasure");
        }

        if (variant != null && variant < 1) {
            throw new MapException("Invalid treasure variant: " + variant);
        }

        String savegameVariable = getProperty("treasure_savegame_variable");
        if (savegameVariable != null && !isValidSavegameVariable(savegameVariable)) {
            throw new MapException("Invalid savegame variable");
        }
    }
}

