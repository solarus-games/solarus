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
 * Represents some stairs where the hero can walk to go to
 * another floor or change its layer inside a single floor.
 */
public class Stairs extends MapEntity {

    /**
     * Subtypes of stairs.
     */
    public enum Subtype implements EntitySubtype {
	SPIRAL_UPSTAIRS,
	SPIRAL_DOWNSTAIRS,
	STRAIGHT_UPSTAIRS,
	STRAIGHT_DOWNSTAIRS,
	INSIDE_FLOOR
	;

	public static final String[] humanNames = {
	    "Spiral staircase (going upstairs)",
	    "Spiral staircase (going downstairs)",
	    "Straight staircase (going upstairs)",
	    "Straight staircase (going downstairs)",
	    "Inside a single floor"
	};

	public static Subtype get(int id) {
	    return values()[id];
	}

	public int getId() {
	    return ordinal();
	}
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("stairs.png",   0, 32, 32, 32),
	new EntityImageDescription("stairs.png",  32, 32, 32, 32),
	new EntityImageDescription("stairs.png",  64, 32, 32, 32),
	new EntityImageDescription("stairs.png",  96, 32, 32, 32),
	new EntityImageDescription("stairs.png", 128, 32, 32, 32),
    };

    /**
     * Creates a new staircase.
     * @param map the map
     */
    public Stairs(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
	return 4;
    }

    /**
     * Returns whether this entity is an obstacle.
     * @return Obstacle.OBSTACLE
     */
    public Obstacle getObstacle() {
	return Obstacle.OBSTACLE;
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {

	super.setSubtype(subtype);

	if (!isInsideFloor() && direction % 2 == 0) {
	    setDirection(1);
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
      currentImageDescription.setX(getSubtype().ordinal() * 32);
      currentImageDescription.setY(getDirection() * 32);
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setDirection(1);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	if (!isInsideFloor() && getDirection() % 2 == 0) {
	    throw new MapException("Invalid direction for this subtype of stairs");
	}
    }

    /**
     * Returns whether this stairs in inside a single floor.
     * @return true if the subtype is INSIDE_FLOOR
     */
    private boolean isInsideFloor() {
        return subtype == Subtype.INSIDE_FLOOR;
    }
}

