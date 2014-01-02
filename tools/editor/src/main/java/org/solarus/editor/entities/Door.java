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
import java.util.NoSuchElementException;

import org.solarus.editor.*;

/**
 * Represents a door to open by the player or by a script.
 */
public class Door extends MapEntity {

    /**
     * The different possible ways of opening a door.
     * Note that any kind door can always be opened manually from a Lua script.
     */
    public enum OpeningMethod {

        BY_SCRIPT_ONLY("none"),
        BY_INTERACTION("interaction"),
        BY_INTERACTION_IF_SAVEGAME_VARIABLE("interaction_if_savegame_variable"),
        BY_INTERACTION_IF_ITEM("interaction_if_item"),
        BY_EXPLOSION("explosion");

        public static final String[] humanNames = {
            "Cannot open",
            "Hero can open",
            "Hero needs savegame variable",
            "Hero needs equipment item",
            "Explosion"
        };

        private final String id;

        private OpeningMethod(String id) {
            this.id = id;
        }

        public String getId() {
            return id;
        }

        public static OpeningMethod get(String id) {
            for (OpeningMethod opening_method: values()) {
                if (opening_method.getId().equals(id)) {
                    return opening_method;
                }
            }
            throw new NoSuchElementException(
                    "No door opening method with name '" + id + "'");
        }
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("entity_door.png", 0, 0, 32, 32),
    };

    /**
     * The sprite representing this entity.
     */
    private Sprite sprite;

    /**
     * Creates a new door.
     * @param map the map
     */
    public Door(Map map) throws MapException {
        super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
        return new Point(0, 0);
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
        return 4;
    }

    /**
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        setDirection(1);
        createStringProperty("sprite", false, "");
        createStringProperty("savegame_variable", true, null);
        createStringProperty("opening_method", true, OpeningMethod.BY_SCRIPT_ONLY.getId());
        createStringProperty("opening_condition", true, null);
        createBooleanProperty("opening_condition_consumed", true, false);
        createStringProperty("cannot_open_dialog", true, null);
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

        String spriteName = getStringProperty("sprite");
        if (!isValidSpriteName(spriteName)) {
            throw new MapException("Invalid sprite name: '" + spriteName + "'");
        }

        String savegameVariable = getStringProperty("savegame_variable");
        if (savegameVariable != null && !isValidSavegameVariable(savegameVariable)) {
            throw new MapException("Invalid door savegame variable");
        }

        OpeningMethod openingMethod = Door.OpeningMethod.get(getStringProperty("opening_method"));
        String openingCondition = getStringProperty("opening_condition");
        if (openingMethod == OpeningMethod.BY_INTERACTION_IF_SAVEGAME_VARIABLE) {
            if (!isValidSavegameVariable(openingCondition)) {
                throw new MapException("You must define a valid required savegame variable with this opening method");
            }
        }
        else if (openingMethod == OpeningMethod.BY_INTERACTION_IF_ITEM) {
            if (!isValidSavegameVariable(openingCondition)) {
                throw new MapException("You must choose a required equipment item with this opening method");
            }
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

