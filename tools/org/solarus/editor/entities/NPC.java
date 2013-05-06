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

import java.awt.*;
import java.util.NoSuchElementException;

import org.solarus.editor.*;

/**
 * Represents an entity that triggers a message or an event when the player
 * presses the action key in front of it.
 */
public class NPC extends MapEntity {

    /**
     * Subtypes of NPC.
     */
    public enum Subtype implements EntitySubtype {
        // We use integers ids for historical reasons.
        GENERALIZED_NPC("0"),
        USUAL_NPC("1");

        public static final String[] humanNames = {
            "Generalized NPC (some solid object)",
            "Usual NPC (a person)"
        };

        private String id;

        private Subtype(String id) {
            this.id = id;
        }

        public String getId() {
            return id;
        }

        public static Subtype get(String id) {
            for (Subtype subtype: values()) {
                if (subtype.getId().equals(id)) {
                    return subtype;
                }
            }
            throw new NoSuchElementException(
                    "No NPC subtype with id '" + id + "'");
        }
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("npc.png", 0, 0, 32, 32),
        new EntityImageDescription("npc.png", 32, 0, 16, 16)
    };

    /**
     * Description of the image currently representing this kind of entity.
     */
    public static final EntityImageDescription[] currentImageDescriptions = {
        generalImageDescriptions[0],
        new EntityImageDescription("npc.png", 32, 0, 16, 24)
    };

    /**
     * The sprite representing this entity (if any).
     */
    private Sprite sprite;

    /**
     * Origin point of each type of NPC.
     */
    private static final Point[] origins = {
        new Point(8, 13),
        new Point(8, 21)
    };

    /**
     * Size of each type of NPC.
     */
    private static final Dimension[] sizes = {
        new Dimension(16, 16),
        new Dimension(16, 24)
    };

    /**
     * Creates a new NPC.
     * @param map the map
     */
    public NPC(Map map) throws MapException {
        super(map, 16, 16);
        setDirection(3);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
        return origins[getSubtype().ordinal()];
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
        return 4;
    }

    /**
     * Returns whether this entity can have the special direction value -1
     * indicating that no direction is set.
     * @return true if this entity can have the special direction value -1
     */
    public boolean canHaveNoDirection() {
        return true;
    }

    /**
     * For an entity that includes an option to set 'no direction'
     * (i.e. when canHaveNoDirection() returns true),
     * this method returns the text that will be displayed in the direction chooser.
     * @return the text that will be displayed in the direction chooser for the 'no direction' option if any
     */
    public String getNoDirectionText() {
        return "Any";
    }

    /**
     * Returns whether this entity is an obstacle.
     * @return Obstacle.OBSTACLE
     */
    public Obstacle getObstacle() {
        return Obstacle.OBSTACLE;
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {
        super.setSubtype(subtype);

        Dimension size = sizes[getSubtype().ordinal()];
        setSizeUnchecked(size.width, size.height);

        setChanged();
        notifyObservers();
    }

    /**
     * Returns whether the specified behavior string is valid
     * @param behavior a behavior string
     * @return true if it is valid
     */
    private boolean isBehaviorValid(String behavior) {
        return behavior.equals("map")
            || behavior.substring(0, 5).equals("item#")
            || behavior.substring(0, 7).equals("dialog#");
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
        currentImageDescription = currentImageDescriptions[getSubtype().ordinal()];
    }

    /**
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        createStringProperty("sprite", true, null);
        createStringProperty("behavior", true, "map");
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
        if (spriteName != null && !isValidSpriteName(spriteName)) {
            throw new MapException("Invalid sprite name: '" + getStringProperty("sprite") + "'");
        }

        if (!isBehaviorValid(getStringProperty("behavior"))) {
            throw new MapException("Invalid behavior string: '" + getStringProperty("behavior") + "'");
        }

        if (getSubtype() == Subtype.USUAL_NPC && getDirection() == -1) {
            throw new MapException("A usual NPC must have an initial direction");
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
            if (direction == -1) {
                direction = 3;
            }
            sprite.paint(g, zoom, showTransparency,
                    getX(), getY(), null, direction, 0);
        }
    }
}

