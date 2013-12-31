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

import java.awt.*;

import org.solarus.editor.*;

/**
 * Represents an entity that Link can destroy (lift and throw or cut)
 * and that can hide a treasure.
 */
public class Destructible extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("entity_destructible.png", 0, 0, 32, 32)
    };

    /**
     * Origin point of this entity.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * The sprite representing this entity.
     */
    private Sprite sprite;

    /**
     * Creates a new destructible.
     * @param map the map
     */
    public Destructible(Map map) throws MapException {
        super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
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
        createStringProperty("sprite", false, "");
        createStringProperty("destruction_sound", true, null);
        createIntegerProperty("weight", true, 0);
        createBooleanProperty("can_be_cut", true, false);
        createBooleanProperty("can_explode", true, false);
        createBooleanProperty("can_regenerate", true, false);
        createIntegerProperty("damage_on_enemies", true, 1);
        createStringProperty("ground", true, Ground.WALL.getName());
    }

    /**
     * Notifies this entity that a property specific to its type has just changed.
     * Does nothing by default.
     * @param name Name of the property that has changed.
     * @param value The new value.
     */
    protected void notifyPropertyChanged(String name, String value) throws MapException {

        if (name.equals("sprite")) {

            if (isValidSpriteName(value)) {
                sprite = new Sprite(value, getMap());
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

        Integer variant = getIntegerProperty("treasure_variant");
        if (variant != null && variant < 1) {
            throw new MapException("Invalid treasure variant: " + variant);
        }

        String savegameVariable = getStringProperty("treasure_savegame_variable");
        if (savegameVariable != null && !isValidSavegameVariable(savegameVariable)) {
            throw new MapException("Invalid treasure savegame variable");
        }

        String spriteName = getStringProperty("sprite");
        if (!isValidSpriteName(spriteName)) {
            throw new MapException("Invalid sprite name: '" + spriteName + "'");
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
            super.paint(g, zoom, showTransparency);
        }
        else {
            // display the sprite
            int direction = getDirection();
            sprite.paint(g, zoom, showTransparency,
                    getX(), getY(), null, direction, 0);
        }
    }
}

