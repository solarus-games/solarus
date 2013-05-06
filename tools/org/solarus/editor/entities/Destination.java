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
import org.solarus.editor.*;

/**
 * Represents the destination of a teletransporter in a map.
 */
public class Destination extends MapEntity {

    /**
     * The sprite representing this destination.
     */
    private Sprite sprite;

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("destination_point.png", 64, 0, 32, 32)
    };

    /**
     * Origin point of a destination point.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * Creates a new destination point.
     * @param map the map
     */
    public Destination(Map map) throws MapException {
        super(map, 16, 16);
        setDirection(3);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (8,13)
     */
    protected Point getOrigin() {
        return origin;
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
     * @return true
     */
    public boolean canHaveNoDirection() {
        return true;
    }

    /**
     * Returns the text to display in the direction chooser for the 'no direction' option.
     * @return the text to display in the direction chooser for the 'no direction' option
     */
    public String getNoDirectionText() {
        return "Keep the same direction";
    }

    /*
     * Updates the description of the image currently representing the entity.
     * @param name name of the property
     * @param value value of the property
     */
    public void updateImageDescription() {

        if (sprite == null) {
            currentImageDescription.setRectangle((getDirection() + 1) * 32, 0, 32, 32);
        }
    }

    /**
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        createStringProperty("sprite", true, null);
    }

    /**
     * Notifies this entity that a property specific to its type has just changed.
     * Does nothing by default.
     * @param name Name of the property that has changed.
     * @param value The new value.
     */
    protected void notifyPropertyChanged(String name, String value) throws MapException {

        if (name.equals("sprite")) {

            if (value != null) {
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
            // no sprite: display a destination icon
            super.paint(g, zoom, showTransparency);
        }
        else {
          // display the appropriate sprite
          sprite.paint(g, zoom, showTransparency,
              getX(), getY(), null, 0, 0);
        }
    }
}

