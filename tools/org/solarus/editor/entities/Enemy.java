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
 * Represents an enemy placed on a map.
 */
public class Enemy extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("enemies.png", 0, 4, 16, 16),
	new EntityImageDescription("enemies.png", 16, 0, 16, 16),
	new EntityImageDescription("enemies.png", 32, 0, 16, 16),
	new EntityImageDescription("enemies.png", 56, 0, 16, 16),
	new EntityImageDescription("enemies.png", 80, 0, 16, 16),

	new EntityImageDescription("bosses.png", 0, 0, 176, 96),
	new EntityImageDescription("bosses.png", 176, 0, 48, 72),
	new EntityImageDescription("bosses.png", 224, 0, 48, 48),

	new EntityImageDescription("enemies.png", 0, 4, 16, 16),
    };

    /**
     * Description of the image representing currently the entity.
     */
    public static final EntityImageDescription[] currentImageDescriptions = {
	new EntityImageDescription("enemies.png", 0, 0, 16, 32),
	new EntityImageDescription("enemies.png", 16, 0, 16, 16),
	new EntityImageDescription("enemies.png", 32, 0, 16, 16),
	new EntityImageDescription("enemies.png", 48, 0, 32, 24),
	new EntityImageDescription("enemies.png", 80, 0, 16, 16),
	
	new EntityImageDescription("bosses.png", 0, 0, 176, 96),
	new EntityImageDescription("bosses.png", 176, 0, 48, 72),
	new EntityImageDescription("bosses.png", 224, 0, 48, 48),

	new EntityImageDescription("enemies.png", 0, 0, 16, 32),
    };

    /**
     * Origin point of each type of enemy.
     */
    private static final Point[] origins = {
	new Point(8, 29),
	new Point(8, 8),
	new Point(8, 13),
	new Point(16, 12),
	new Point(8, 8),
	
	new Point(88, 64),
	new Point(24, 69),
	new Point(24, 29),

	new Point(8, 29),
    };

    /**
     * Size of each type of enemy.
     */
    private static final Dimension[] sizes = {
	new Dimension(16, 32),
	new Dimension(16, 16),
	new Dimension(16, 16),
	new Dimension(32, 24),
	new Dimension(16, 16),

	new Dimension(176, 96),
	new Dimension(48, 72),
	new Dimension(48, 48),

	new Dimension(16, 32),
    };

    /**
     * Subtypes of enemies.
     */
    public enum Subtype implements EntitySubtype {
	SIMPLE_GREEN_SOLDIER,
	BUBBLE,
	TENTACLE,
	MINILLOSAUR,
	CHAIN_AND_BALL,
	
	PAPILLAUSOR_KING,
	KHORNETH,
	KHOTOR,
	CUSTOM;

	public static final String[] humanNames = {
	    "Simple green soldier",
	    "Bubble",
	    "Tentacle",
	    "Minillosaur",
	    "Chain and Ball",
	    
	    "Papillausor King",
	    "Khorneth",
	    "Khotor",
	    "Custom",
	};

	public int getId() {
	    // make the bosses have an id greater than 1000
	    int index = ordinal(); // CUSTOM is -1
	    int firstBossIndex = PAPILLAUSOR_KING.ordinal();
	    if (index < firstBossIndex) {
		return index;
	    }
	    
	    return index - firstBossIndex + 1000;
	}

	public static Subtype get(int id) {
	  if (id >= PAPILLAUSOR_KING.getId()) {
	    int firstBossIndex = PAPILLAUSOR_KING.ordinal();
            id = id - 1000 + firstBossIndex;
	  }
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
	Dimension size = sizes[getSubtype().ordinal()];
	setSizeImpl(size.width, size.height);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
	return origins[getSubtype().ordinal()];
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
	
	Dimension size = sizes[getSubtype().ordinal()];
	setSizeImpl(size.width, size.height);
	
	setChanged();
	notifyObservers();
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("breed", "_none");
	setProperty("rank", Rank.NORMAL.ordinal());
	setProperty("savegameVariable", -1);
	setProperty("treasureName", Item.randomId);
	setProperty("treasureVariant", 1);
	setProperty("treasureSavegameVariable", -1);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	String breed = getProperty("breed");
	if (breed.length() == 0
	    || breed.indexOf(' ') != -1
	    || breed.indexOf(' ') != -1) {
	    throw new MapException("An enemy's breed cannot be empty or have whitespaces");
	}

	int savegameVariable = getIntegerProperty("savegameVariable");
	if (savegameVariable < -1 || savegameVariable >= 32768) {
	    throw new MapException("Invalid enemy savegame variable");
	}

	Rank rank = Rank.get(getIntegerProperty("rank"));
	if (rank != Rank.NORMAL && savegameVariable < 0) {
	    throw new MapException("This enemy must be saved");
	}

	int treasureSavegameVariable = getIntegerProperty("treasureSavegameVariable");
	if (treasureSavegameVariable < -1 || treasureSavegameVariable >= 32768) {
	    throw new MapException("Invalid treasure savegame variable");
	}
    }

    /**
     * Updates the description of the image currently representing the entity.
     * By default, the image description is a copy of the general image description of this kind of entity.
     * Redefine this method to display the entity with an image containing
     * the entity's current properties, by modifying the currentImageDescription field.
     */
    public void updateImageDescription() {
      currentImageDescription = currentImageDescriptions[getSubtype().ordinal()];
    }
}

