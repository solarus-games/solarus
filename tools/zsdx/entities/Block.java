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
package zsdx.entities;

import java.awt.*;
import zsdx.Map;
import zsdx.*;

/**
 * Represents a block or a statue that the hero can push or pull.
 */
public class Block extends MapEntity {

    /**
     * Subtypes of blocks.
     */
    public enum Subtype implements EntitySubtype {
	NORMAL_BLOCK,
	STATUE;

	public static final String[] humanNames = {
	    "Normal block",
	    "Statue",
	};

	public static Subtype get(int id) {
	    return values()[id];
	}

	public int getId() {
	    return ordinal();
	}
    }

    /**
     * Limitations of moves for blocks.
     */
    public enum MaximumMoves {
	NONE,
	ONE,
	INFINITE;

	public static final String[] humanNames = {
	    "Cannot move",
	    "One move only",
	    "Infinite moves",
	};

	public static MaximumMoves get(int id) {
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
	new EntityImageDescription("miscellaneous_entities.png", 56, 0, 16, 16),
	new EntityImageDescription("miscellaneous_entities.png", 32, 0, 24, 24),
    };

    /**
     * Description of the image currently representing this kind of entity.
     */
    public static final EntityImageDescription[] currentImageDescriptions = {
	generalImageDescriptions[0],
	new EntityImageDescription("miscellaneous_entities.png", 36, 0, 16, 24),
    };

    /**
     * Origin point of each subtype.
     */
    private static final Point[] origins = {
	new Point(8, 13),
	new Point(8, 21),
    };

    /**
     * Size of each subtype.
     */
    private static final Dimension[] sizes = {
	new Dimension(16, 16),
	new Dimension(16, 24),
    };

    /**
     * Creates a new block.
     * @param map the map
     */
    public Block(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
	return origins[getSubtypeId()];
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
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

	Dimension size = sizes[getSubtypeId()];
	setSizeImpl(size.width, size.height);

	setChanged();
	notifyObservers();
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	currentImageDescription = currentImageDescriptions[getSubtypeId()];
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("maximumMoves", MaximumMoves.ONE.getId());
    }
}
