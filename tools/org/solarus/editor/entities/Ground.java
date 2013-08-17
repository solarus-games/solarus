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
public enum Ground {

    EMPTY("ground_empty.png"),
    TRAVERSABLE("ground_traversable.png"),
    WALL("ground_wall.png"),
    LOW_WALL("ground_low_wall.png"),
    WALL_TOP_RIGHT("ground_wall_top_right.png"),
    WALL_TOP_LEFT("ground_wall_top_left.png"),
    WALL_BOTTOM_LEFT("ground_wall_bottom_left.png"),
    WALL_BOTTOM_RIGHT("ground_wall_bottom_right.png"),
    WALL_TOP_RIGHT_WATER("ground_wall_top_right_water.png"),
    WALL_TOP_LEFT_WATER("ground_wall_top_left_water.png"),
    WALL_BOTTOM_LEFT_WATER("ground_wall_bottom_left_water.png"),
    WALL_BOTTOM_RIGHT_WATER("ground_wall_bottom_right_water.png"),
    DEEP_WATER("ground_deep_water.png"),
    SHALLOW_WATER("ground_shallow_water.png"),
    HOLE("ground_hole.png"),
    ICE("ground_ice.png"),
    LADDER("ground_ladder.png"),
    PRICKLE("ground_prickles.png"),
    LAVA("ground_lava.png"),
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
     * Creates a ground kind.
     * @param id Id of this ground kind.
     * @param iconFileName Name of the icon file representing this ground kind.
     */
    private Ground(String iconFileName) {
        this.iconFileName = iconFileName;
    }

    /**
     * Returns the name of this ground kind.
     * @return The name.
     */
    public String getName() {
        return humanNames[ordinal()];
    }

    /**
     * Returns whether this ground kind corresponds to a wall.
     * @return \c true if this is a wall.
     */
    public boolean isWall() {
        return this == WALL || isDiagonal();
    }

    /**
     * Returns whether this ground property corresponds to a diagonal wall.
     * @return \c true if this is a diagonal wall.
     */
    public boolean isDiagonal() {
        return this == WALL_TOP_RIGHT ||
          this == WALL_TOP_LEFT ||
          this == WALL_BOTTOM_LEFT ||
          this == WALL_BOTTOM_RIGHT ||
          this == WALL_TOP_RIGHT_WATER ||
          this == WALL_TOP_LEFT_WATER ||
          this == WALL_BOTTOM_LEFT_WATER ||
          this == WALL_BOTTOM_RIGHT_WATER;
    }

    /**
     * Returns the icon representing this ground.
     * @return The icon for this ground.
     */
    public ImageIcon getIcon() {
        return getIcons()[ordinal()];
    }

    /**
     * Returns all the icons.
     * @return Icon of each type of ground.
     */
    public static ImageIcon[] getIcons() {

        if (icons == null) {
            icons = new ImageIcon[values().length];
            int i = 0;
            for (Ground value: values()) {
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

