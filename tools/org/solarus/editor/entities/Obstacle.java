/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
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
import java.io.*;

/**
 * Enumeration of the obstacle property of the tiles.
 */
public enum Obstacle {

    EMPTY("obstacle_empty.png"),
    TRAVERSABLE("obstacle_none.png"),
    OBSTACLE("obstacle.png"),
    LOW_WALL("obstacle_low_wall.png"),
    TOP_RIGHT("obstacle_top_right.png"),
    TOP_LEFT("obstacle_top_left.png"),
    BOTTOM_LEFT("obstacle_bottom_left.png"),
    BOTTOM_RIGHT("obstacle_bottom_right.png"),
    TOP_RIGHT_WATER("obstacle_top_right_water.png"),
    TOP_LEFT_WATER("obstacle_top_left_water.png"),
    BOTTOM_LEFT_WATER("obstacle_bottom_left_water.png"),
    BOTTOM_RIGHT_WATER("obstacle_bottom_right_water.png"),
    DEEP_WATER("obstacle_deep_water.png"),
    SHALLOW_WATER("obstacle_shallow_water.png"),
    HOLE("obstacle_hole.png"),
    ICE("obstacle_ice.png"),
    LADDER("obstacle_ladder.png"),
    PRICKLE("obstacle_prickle.png"),
    LAVA("obstacle_lava.png"),
    ;

    private String iconFileName;

    private static ImageIcon[] icons = null;

    public static final String[] humanNames = {
            "Empty",
            "Traversable",
            "Full obstacle",
            "Low wall",
            "Top right",
            "Top left",
            "Bottom left",
            "Bottom right",
            "Top right (water)",
            "Top left (water)",
            "Bottom left (water)",
            "Bottom right (water)",
            "Deep water",
            "Shallow water",
            "Hole",
            "Ice",
            "Ladder",
            "Prickles",
            "Lava",
    };

    /**
     * Creates an obstacle property
     * @param id id of this obstacle property
     * @param iconFileName name of the icon file representing this obstacle property
     */
    private Obstacle(String iconFileName) {
        this.iconFileName = iconFileName;
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
        return this == OBSTACLE || isDiagonal();
    }

    /**
     * Returns whether this obstacle property corresponds to a diagonal wall.
     * @return true if this is a diagonal wall
     */
    public boolean isDiagonal() {
        return this == TOP_RIGHT ||
          this == TOP_LEFT ||
          this == BOTTOM_LEFT ||
          this == BOTTOM_RIGHT ||
          this == TOP_RIGHT_WATER ||
          this == TOP_LEFT_WATER ||
          this == BOTTOM_LEFT_WATER ||
          this == BOTTOM_RIGHT_WATER;
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
                try {
                    icons[i] = Project.getEditorImageIcon(value.iconFileName);
                }
                catch (IOException ex) {
                    System.err.println(ex.getMessage());
                    icons[i] = new ImageIcon();
                }
                icons[i].setDescription(value.name());
                i++;
            }
        }
        return icons;
    }
}

