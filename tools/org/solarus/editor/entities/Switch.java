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
 * A switch is a button that can be enabled by the hero or a block.
 */
public class Switch extends MapEntity {

    /**
     * Subtypes of switches.
     */
    public enum Subtype implements EntitySubtype {
	WALKABLE_INVISIBLE,
	WALKABLE_VISIBLE,
	ARROW_TARGET;

	public static final String[] humanNames = {
	  "Walkable invisible",
	  "Walkable visible",
	  "Arrow target"
	};

	public int getId() {
	    return ordinal();
	}

	public static Subtype get(int id) {
	    return values()[id];
	}

	public static boolean isWalkable(EntitySubtype subtype) {
	    return subtype == WALKABLE_INVISIBLE || subtype == WALKABLE_VISIBLE;
	}
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("miscellaneous_entities.png", 56, 16, 16, 16), // walkable invisible
	new EntityImageDescription("miscellaneous_entities.png", 72, 16, 16, 16), // walkable visible
	new EntityImageDescription("miscellaneous_entities.png", 88, 16, 16, 16), // arrow target
    };

    /**
     * Creates a new switch.
     * @param map the map
     */
    public Switch(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Returns whether this switch is walkable.
     * @return true if the subtype is WALKABLE_INVISIBLE or WALKABLE_VISIBLE
     */
    public boolean isWalkable() {
        return Subtype.isWalkable(getSubtype());
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     * @throws MapException if the subtype is not valid
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {

	if (subtype != this.subtype) {
	    if (subtype == Subtype.WALKABLE_INVISIBLE) {
		setProperty("needsBlock", false);
	    }
	    else if (Subtype.isWalkable(subtype)) {
		setProperty("needsBlock", false);
		setProperty("disableWhenLeaving", false);
	    }
	    super.setSubtype(subtype);
	}
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("needsBlock", false);
	setProperty("disableWhenLeaving", false);
	setSubtype(Subtype.WALKABLE_INVISIBLE);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	if (getSubtype() == Subtype.WALKABLE_INVISIBLE && getBooleanProperty("needsBlock")) {
	    throw new MapException("Cannot put a block on an invisible switch");
	}

	if (!isWalkable()) {
	  
	    if (getBooleanProperty("needsBlock")) {
	        throw new MapException("Cannot put a block on a non-walkable switch");
	    }

	    if (getBooleanProperty("disableWhenLeaving")) {
	        throw new MapException("Cannot disable the switch when leaving for a non-walkable switch");
	    }
	}
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	currentImageDescription = generalImageDescriptions[subtype.getId()];
    }
}

