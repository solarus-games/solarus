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

import java.awt.*;
import org.solarus.editor.*;

/**
 * Represents a treasure that the hero can pick on the ground.
 */
public class PickableItem extends MapEntity {


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
    public PickableItem(Map map) throws MapException {
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
	setProperty("treasureName", Item.noneId);
	setProperty("treasureVariant", 1);
	setProperty("treasureSavegameVariable", -1);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	String treasureName = getProperty("treasureName");

	if (treasureName.equals(Item.noneId) || treasureName.equals(Item.randomId)) {
	    throw new MapException("The treasure of a pickable item cannot be 'None' or 'Random'");
	}

	int savegameVariable = getIntegerProperty("treasureSavegameVariable");
	if (savegameVariable < -1 || savegameVariable >= 32768) {
	    throw new MapException("Invalid savegame variable");
	}
    }
}

