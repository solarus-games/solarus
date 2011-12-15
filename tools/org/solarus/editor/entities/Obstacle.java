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
import javax.swing.*;
import java.util.*;

/**
 * Enumeration of the obstacle property of the tiles.
 * Values lower than or equal to NONE correspond to entities the hero can walk on.
 * Values higher than NONE correspond to obstacles.
 */
public enum Obstacle {

    LAVA(-6, "obstacle_lava.png"),
    PRICKLE(-5, "obstacle_prickle.png"),
    LADDER(-4, "obstacle_ladder.png"),
    HOLE(-3, "obstacle_hole.png"),
    SHALLOW_WATER(-2, "obstacle_shallow_water.png"),
    DEEP_WATER(-1, "obstacle_deep_water.png"),
    NONE(0, "obstacle_none.png"),
    OBSTACLE(1, "obstacle.png"),
    TOP_RIGHT(2, "obstacle_top_right.png"),
    TOP_LEFT(3, "obstacle_top_left.png"),
    BOTTOM_LEFT(4, "obstacle_bottom_left.png"),
    BOTTOM_RIGHT(5, "obstacle_bottom_right.png"),
    EMPTY(6, "obstacle_empty.png"),
    TOP_RIGHT_WATER(7, "obstacle_top_right_water.png"),
    TOP_LEFT_WATER(8, "obstacle_top_left_water.png"),
    BOTTOM_LEFT_WATER(9, "obstacle_bottom_left_water.png"),
    BOTTOM_RIGHT_WATER(10, "obstacle_bottom_right_water.png"),
    ;

    private int id;
    private String iconFileName;

    private static ImageIcon[] icons = null;

    public static final String[] humanNames = {
    	"Lava",
    	"Prickles",
    	"Ladder",
    	"Hole",
    	"Shallow water",
    	"Deep water",
    	"No obstacle",
    	"Full obstacle",
    	"Top right",
    	"Top left",
    	"Bottom left",
    	"Bottom right",
    	"Empty",
    	"Top right (water)",
    	"Top left (water)",
    	"Bottom left (water)",
    	"Bottom right (water)",
    };

    /**
     * Creates an obstacle property
     * @param id id of this obstacle property
     * @param iconFileName name of the icon file representing this obstacle property
     */
    private Obstacle(int id, String iconFileName) {
	this.id = id;
	this.iconFileName = iconFileName;
    }

    /**
     * Returns the obstacle property with the specified id.
     * @param id id of the obstacle to get
     * @return the obstacle property with this id
     */
    public static Obstacle get(int id) throws NoSuchElementException {
	for (Obstacle t: values()) {
	    if (t.getId() == id) {
		return t;
	    }
	}

	throw new NoSuchElementException("Unknown obstacle property id: " + id);
    }

    /**
     * Returns the id of this obstacle property.
     * @return the id
     */
    public int getId() {
	return id;
    }

    /**
     * Returns the name of this obstacle property.
     * @return the name
     */
    public String getName() {
	return humanNames[ordinal()];
    }

    /**
     * Returns whether this obstacle property corresponds to a wall.
     * @return true if this is a wall
     */
    public boolean isWall() {
	return getId() >= OBSTACLE.getId();
    }

    /**
     * Returns whether this obstacle property corresponds to a diagonal wall.
     * @return true if this is a diagonal wall
     */
    public boolean isDiagonal() {
	return getId() >= TOP_RIGHT.getId() && getId() <= BOTTOM_RIGHT.getId();
    }

    /**
     * Returns the icon representing this obstacle property.
     * @return the icon for this obstacle property
     */
    public ImageIcon getIcon() {
	return getIcons()[ordinal()];
    }

    /**
     * Returns all the icons.
     * @return icon of each type of obstacle
     */
    public static ImageIcon[] getIcons() {

	if (icons == null) {
	    icons = new ImageIcon[values().length];
	    int i = 0;
	    for (Obstacle value: values()) {
		icons[i] = Project.getEditorImageIcon(value.iconFileName);
		icons[i].setDescription(value.name());
		i++;
	    }
	}
	return icons;
    }
}

