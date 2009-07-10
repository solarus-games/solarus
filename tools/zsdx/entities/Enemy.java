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
import zsdx.*;

/**
 * Represents an enemy placed on a map.
 */
public class Enemy extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("enemies.png", 0, 4, 16, 16),
	new EntityImageDescription("enemies.png", 16, 0, 16, 16),
	new EntityImageDescription("enemies.png", 32, 0, 16, 16)
    };

    /**
     * Description of the image representing currently the entity.
     */
    public static final EntityImageDescription[] currentImageDescriptions = {
	new EntityImageDescription("enemies.png", 0, 0, 16, 32),
	new EntityImageDescription("enemies.png", 16, 0, 16, 16),
	new EntityImageDescription("enemies.png", 32, 0, 16, 16)
    };

    /**
     * Origin point of each type of enemy.
     */
    private static final Point[] origins = {
	new Point(8, 29),
	new Point(8, 8),
	new Point(8, 13)
    };

    /**
     * Size of each type of enemy.
     */
    private static final Dimension[] sizes = {
	new Dimension(16, 32),
	new Dimension(16, 16),
	new Dimension(16, 16)
    };

    /**
     * Subtypes of enemies.
     */
    public enum Subtype implements EntitySubtype {
	SIMPLE_GREEN_SOLDIER,
	BUBBLE,
	TENTACLE;

	public static final String[] humanNames = {
	    "Simple green soldier",
	    "Bubble",
	    "Tentacle"
	};

	public int getId() {
	    return ordinal();
	}

	public static Subtype get(int id) {
	    return values()[id];
	}
    }

    /**
     * Enemy ranks.
     */
    public enum Rank {
	NORMAL,
	MINIBOSS,
	BOSS;

	public static final String[] humanNames = {"Normal", "Miniboss", "Boss"};

	public int getId() {
	    return ordinal();
	}

	public static Rank get(int id) {
	    return values()[id];
	}
    }

    /**
     * Creates a new enemy.
     * By default, the subtype is a simple green soldier and the pickable item is random. 
     * @param map the map
     */
    public Enemy(Map map)throws MapException {
	super(map, 16, 16);

	setDirection(3);
	Dimension size = sizes[getSubtypeId()];
	setSizeImpl(size.width, size.height);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
	return origins[getSubtypeId()];
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
	return 4;
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Sets the subtype of this enemy.
     * @param subtype the subtype of enemy
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
	setProperty("rank", Rank.NORMAL.ordinal());
	setProperty("savegameVariable", -1);
	setProperty("pickableItemSubtype", PickableItem.Subtype.RANDOM.getId());
	setProperty("pickableItemSavegameVariable", -1);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	int savegameVariable = getIntegerProperty("savegameVariable");
	if (savegameVariable < -1 || savegameVariable >= 32768) {
	    throw new MapException("Invalid enemy savegame variable");
	}

	Rank rank = Rank.get(getIntegerProperty("rank"));
	if (rank != Rank.NORMAL && savegameVariable < 0) {
	    throw new MapException("This enemy must be saved");
	}

	PickableItem.Subtype pickableItemSubtype = PickableItem.Subtype.get(getIntegerProperty("pickableItemSubtype"));
	int pickableItemSavegameVariable = getIntegerProperty("pickableItemSavegameVariable");

	if (pickableItemSavegameVariable < -1 || pickableItemSavegameVariable >= 32768) {
	    throw new MapException("Invalid pickable item savegame variable");
	}

	boolean isSaved = (pickableItemSavegameVariable >= 0 && pickableItemSavegameVariable < 32768);

	if (pickableItemSubtype.mustBeSaved() && !isSaved) {
	    throw new MapException("This pickable item must be saved");
	}

	if (!pickableItemSubtype.mustBeSaved() && isSaved) {
	    throw new MapException("This pickable item cannot be saved");
	}

	boolean inDungeon = map.isInDungeon();
	boolean mustBeInDungeon = pickableItemSubtype.isOnlyInDungeon();
	if (mustBeInDungeon && !inDungeon) {
	    throw new MapException("This pickable item is available only in a dungeon");
	}

	if (pickableItemSubtype == PickableItem.Subtype.SMALL_KEY && !map.hasSmallKeys()) {
	    throw new MapException("The small keys are not enabled in this map");
	}
    }
}
