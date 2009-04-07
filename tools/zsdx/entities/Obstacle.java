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

/**
 * Enumeration of the obstacle property of the tiles.
 */
public enum Obstacle {

    OBSTACLE_NONE,
    OBSTACLE,
    OBSTACLE_TOP_RIGHT,
    OBSTACLE_TOP_LEFT,
    OBSTACLE_BOTTOM_LEFT,
    OBSTACLE_BOTTOM_RIGHT,
    OBSTACLE_SHALLOW_WATER,
    OBSTACLE_DEEP_WATER,
    OBSTACLE_HOLE;

    public static final String[] humanNames = {
	"No obstacle",
	"Full obstacle",
	"Top right",
	"Top left",
	"Bottom left",
	"Bottom right",
	"Shallow water",
	"Deep water",
	"Hole",
    };

    /**
     * Returns the obstacle with the specified index.
     * @param index index of the obstacle to get
     * @return the obstacle property with this index
     */
    public static Obstacle get(int index) {
	return values()[index];
    }

    /**
     * Returns the index of this obstacle.
     * @return the index
     */
    public int getIndex() {
	return ordinal();
    }

    /**
     * Returns all the icons.
     * @return icon of each type of obstacle
     */
    public static ImageIcon[] getIcons() {
	return obstacleIcons;
    }

}
