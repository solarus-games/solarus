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
 * Represents an entity that Link can destroy (lift and throw or cut)
 * and that can hide a treasure.
 */
public class DestructibleItem extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("destructible_items.png", 0, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 16, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 32, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 48, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 64, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 80, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 96, 0, 16, 24),
    };

    /**
     * Size of each subtype.
     */
    private static final Dimension[] sizes = {
	new Dimension(16, 16),
	new Dimension(16, 16),
	new Dimension(16, 16),
	new Dimension(16, 16),
	new Dimension(16, 16),
	new Dimension(16, 16),
	new Dimension(16, 24),
    };

    /**
     * Origin point of a destructible item.
     */
    private static final Point[] origins = {
      new Point(8, 13),
      new Point(8, 13),
      new Point(8, 13),
      new Point(8, 13),
      new Point(8, 13),
      new Point(8, 13),
      new Point(8, 21),
    };

    /**
     * Subtypes of destructible items.
     */
    public enum Subtype implements EntitySubtype {
	POT,
	SKULL,
	BUSH,
	STONE_SMALL_WHITE,
	STONE_SMALL_BLACK,
	GRASS,
	BOMB_FLOWER;

	public static final String[] humanNames = {
	    "Pot",
	    "Skull",
	    "Bush",
	    "Small white stone",
	    "Small black stone",
	    "Grass",
	    "Bomb flower",
	};

	public int getId() {
	    return ordinal();
	}

	public static Subtype get(int id) {
	    return values()[id];
	}
    };

    /**
     * Creates a new destructible. 
     * @param map the map
     */
    public DestructibleItem(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the origin point
     */
    protected Point getOrigin() {
	return origins[subtype.ordinal()];
    }

    /**
     * Returns the entity's obstacle property.
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
	
	Dimension size = sizes[getSubtype().ordinal()];
	setSizeImpl(size.width, size.height);
	
	setChanged();
	notifyObservers();
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
/*    public void updateImageDescription() {
	int x = getSubtypeId() * 16;
	int height = (getSubtype() != Subtype.BOMB_FLOWER) ? 16 : 24;
	currentImageDescription.setX(x);
	currentImageDescription.setHeight(height);
    }
*/
    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("treasureName", Item.randomId);
	setProperty("treasureVariant", 1);
	setProperty("treasureSavegameVariable", -1);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	int variant = getIntegerProperty("treasureVariant");
	if (variant < 1) {
	    throw new MapException("Invalid treasure variant: " + variant);
	}

	int savegameVariable = getIntegerProperty("treasureSavegameVariable");
	if (savegameVariable < -1 || savegameVariable >= 32768) {
	    throw new MapException("Invalid treasure savegame variable");
	}
    }
}

