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
 * Represents the destination point of a teletransporter in a map.
 */
public class DestinationPoint extends MapEntity {

    /**
     * Subtypes of destination points.
     */
    public enum Subtype implements EntitySubtype {
	INVISIBLE,
	GRAY;

	public static final String[] humanNames = {
	  "Invisible", "Gray"  
	};

	public int getId() {
	    return ordinal();
	}

	public static Subtype get(int id) {
	    return values()[id];
	}
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("destination_point.png", 64, 0, 32, 32),    // invisible (up)
	new EntityImageDescription("destination_point.png", 160, 0, 16, 16)    // gray
    };

    /**
     * Origin point of a destination point.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * Creates a new destination point.
     * @param map the map
     */
    public DestinationPoint(Map map) throws MapException {
	super(map, 16, 16);
	subtype = Subtype.INVISIBLE;
	setDirection(3);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (8,13)
     */
    protected Point getOrigin() {
	return origin;
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
	return 4;
    }

    /**
     * Returns whether this entity can have the special direction value -1
     * indicating that no direction is set.
     * @return true
     */
    public boolean canHaveNoDirection() {
	return true;
    }

    /**
     * Returns the text to display in the direction chooser for the 'no direction' option.
     * @return the text to display in the direction chooser for the 'no direction' option
     */
    public String getNoDirectionText() {
	return "Keep the same direction";
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     * @throws MapException if the subtype is not valid
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {

	if (subtype != this.subtype) {
	    if (subtype == Subtype.INVISIBLE) {
		if (this.subtype != null) {
		    setDirection(3);
		}
	    }
	    else {
		    setDirection(-1);
	    }
	    super.setSubtype(subtype);
	}
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	if (subtype == Subtype.INVISIBLE) {
	    currentImageDescription.setRectangle((getDirection() + 1) * 32, 0, 32, 32);
	}
	else {
	    currentImageDescription.setRectangle(160, 0, 16, 16);	    
	}
    }
}
