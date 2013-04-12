/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
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

import java.awt.Graphics;
import java.awt.Point;
import java.util.NoSuchElementException;

import org.solarus.editor.*;

/**
 * Represents a chest placed on the map.
 */
public class Chest extends MapEntity {

    /**
     * The different possible ways of opening a chest.
     */
    public enum OpeningMethod {

        BY_INTERACTION("interaction"),
        BY_INTERACTION_IF_SAVEGAME_VARIABLE("interaction_if_savegame_variable"),
        BY_INTERACTION_IF_ITEM("interaction_if_item");

        public static final String[] humanNames = {
            "Hero can open",
            "Needs savegame variable",
            "Needs equipment item"
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
                    "No chest opening method with name '" + id + "'");
        }
    }

    /**
     * The sprite representing this entity.
     */
    private Sprite sprite;

    /**
     * Description of the default images representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions =
        {new EntityImageDescription("chest.png", 0, 0, 16, 16)};

    /**
     * Creates a new chest.
     * @param map the map
     */
    public Chest(Map map) throws MapException {
        super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
        return new Point(8, 13);
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
        setProperty("treasure_name", null);
        setIntegerProperty("treasure_variant", null);
        setProperty("treasure_savegame_variable", null);
        setProperty("sprite", "entities/chest");
        setProperty("opening_method", OpeningMethod.BY_INTERACTION.getId());
        setProperty("opening_condition", null);
        setBooleanProperty("opening_condition_consumed", false);
        setProperty("cannot_open_dialog_id", null);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

        String spriteName = getProperty("sprite");
        if (!isValidSpriteName(spriteName)) {
            throw new MapException("Invalid sprite name: '" + spriteName + "'");
        }

        OpeningMethod openingMethod = OpeningMethod.get(getProperty("opening_method"));
        String openingCondition = getProperty("opening_condition");
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

        String treasureName = getProperty("treasure_name");
        Integer variant = getIntegerProperty("treasure_variant");
        if (treasureName != null && (variant == null || variant < 1)) {
            throw new MapException("A variant must be defined with this treasure");
        }

        if (variant != null && variant < 1) {
            throw new MapException("Invalid treasure variant: " + variant);
        }

        String savegameVariable = getProperty("treasure_savegame_variable");
        if (savegameVariable != null && !isValidSavegameVariable(savegameVariable)) {
            throw new MapException("Invalid treasure savegame variable");
        }
    }

    /**
     * Returns whether this entity is an obstacle.
     * @return Obstacle.OBSTACLE
     */
    public Obstacle getObstacle() {
        return Obstacle.OBSTACLE;
    }

    /**
     * Sets a property specific to this kind of entity.
     * @param name name of the property
     * @param value value of the property
     */
    public void setProperty(String name, String value) throws MapException {

        super.setProperty(name, value);

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

