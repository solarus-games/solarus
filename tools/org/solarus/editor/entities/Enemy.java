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
 * Represents an enemy placed on a map.
 */
public class Enemy extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("miscellaneous_entities.png", 48, 32, 16, 16),
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
    public Enemy(Map map) throws MapException {
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
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        createStringProperty("breed", false, "");
        createIntegerProperty("rank", true, Rank.NORMAL.ordinal());
        createStringProperty("savegame_variable", true, null);
        createStringProperty("treasure_name", true, null);
        createIntegerProperty("treasure_variant", true, 1);
        createStringProperty("treasure_savegame_variable", true, null);
    }

    /**
     * Notifies this entity that a property specific to its type has just changed.
     * Does nothing by default.
     * @param name Name of the property that has changed.
     * @param value The new value.
     */
    protected void notifyPropertyChanged(String name, String value) throws MapException {

        if (name.equals("breed")) {

            if (value.length() > 0) {
                try {
                    sprite = new Sprite("enemies/" + value, getMap());
                    setSizeUnchecked(sprite.getSize(null, 0));
                }
                catch (QuestEditorException ex) {
                    System.err.println(ex.getMessage());

                    // Keep the default sprite.
                    sprite = null;
                    setSizeUnchecked(16, 16);
                }
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

        String breed = getStringProperty("breed");
        if (breed.length() == 0
            || breed.indexOf(' ') != -1
            || breed.indexOf('\t') != -1) {
            throw new MapException("An enemy's breed cannot be empty or have whitespaces");
        }

        String savegameVariable = getStringProperty("savegame_variable");
        if (savegameVariable != null && !isValidSavegameVariable(savegameVariable)) {
            throw new MapException("Invalid enemy savegame variable");
        }

        Integer variant = getIntegerProperty("treasure_variant");
        if (variant != null && variant < 1) {
            throw new MapException("Invalid treasure variant: " + variant);
        }

        String treasureSavegameVariable = getStringProperty("treasure_savegame_variable");
        if (treasureSavegameVariable != null && !isValidSavegameVariable(treasureSavegameVariable)) {
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

