/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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

import java.awt.*;

import org.solarus.editor.*;

/**
 * Represents a treasure that the hero can pick on the ground.
 */
public class Pickable extends MapEntity {


    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("entity_pickable.png", 0, 0, 32, 32)
    };

    /**
     * Origin point of a pickable item.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * Creates a new pickable item.
     * @param map the map
     */
    public Pickable(Map map) throws MapException {
        super(map, 16, 16);
        setSprite(new Sprite("entities/items", map));
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (8,13)
     */
    protected Point getOrigin() {
        return origin;
    }

    /**
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        createStringProperty("treasure_name", true, null);
        createIntegerProperty("treasure_variant", true, 1);
        createStringProperty("treasure_savegame_variable", true, null);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

        Integer treasureVariant = getIntegerProperty("treasure_variant");
        if (treasureVariant != null && treasureVariant < 1) {
            throw new MapException("Invalid treasure variant: " + treasureVariant);
        }

        String treasureSavegameVariable = getStringProperty("treasure_savegame_variable");
        if (treasureSavegameVariable != null && !isValidSavegameVariable(treasureSavegameVariable)) {
            throw new MapException("Invalid savegame variable");
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

        // Display the sprite of the treasure.
        Sprite sprite = getSprite();
        String animation = getStringProperty("treasure_name");
        int direction = getIntegerProperty("treasure_variant") - 1;
        if (animation != null &&
                sprite.hasAnimation(animation) &&
                direction <= sprite.getAnimation(animation).getNbDirections()) {
            sprite.paint(g, zoom, showTransparency,
                    getX(), getY(), animation, direction, 0);
        }
        else {
            // Display a default image.
            super.paint(g, zoom, showTransparency);
        }
    }
}

