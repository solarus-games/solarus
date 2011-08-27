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
    };

    /**
     * The sprite representing this entity (if any).
     */
    private Sprite sprite;

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
     * @param map the map
     */
    public Enemy(Map map)throws MapException {
	super(map, 16, 16);
        setDirection(3);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {

        if (sprite == null) {
            return new Point(8, 29);
        }
        return sprite.getOrigin(null, getDirection());
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
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("breed", "");
	setProperty("rank", Rank.NORMAL.ordinal());
	setProperty("savegameVariable", -1);
	setProperty("treasureName", Item.randomId);
	setProperty("treasureVariant", 1);
	setProperty("treasureSavegameVariable", -1);
    }

    /**
     * Sets a property specific to this kind of entity.
     * @param name name of the property
     * @param value value of the property
     */
    public void setProperty(String name, String value) throws MapException {

        super.setProperty(name, value);

        if (name.equals("breed")) {

            if (value.length() > 0) {
                sprite = new Sprite("enemies/" + value, map);
                setSizeImpl(sprite.getSize(null, 0));
            }
            else {
                sprite = null;
            }
        }
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	String breed = getProperty("breed");
	if (breed.length() == 0
	    || breed.indexOf(' ') != -1
	    || breed.indexOf('\t') != -1) {
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
     * Draws this entity on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

        if (sprite == null) {
            // display a default enemy icon
            super.paint(g, zoom, showTransparency);
        }
        else {
            // display the sprite
            sprite.paint(g, zoom, showTransparency,
                    getX(), getY(), null, getDirection(), 0);
        }
    }
}

