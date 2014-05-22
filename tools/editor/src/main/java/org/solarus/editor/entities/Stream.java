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

import org.solarus.editor.*;

import java.awt.*;

/**
 * A special terrain where the hero is moved into a specific direction.
 */
public class Stream extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("entity_stream.png", 128, 0, 32, 32)
    };

    /**
     * Origin point of a stream.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * Creates a new stream.
     * @param map the map
     */
    public Stream(Map map) throws MapException {
        super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (0, 0)
     */
    protected Point getOrigin() {
        return origin;
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 8
     */
    public int getNbDirections() {
        return 8;
    }

    /**
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        createStringProperty("sprite", true, null);
        createIntegerProperty("speed", true, 64);
        createBooleanProperty("allow_movement", true, true);
        createBooleanProperty("allow_attack", true, true);
        createBooleanProperty("allow_item", true, true);
    }

    /**
     * Notifies this entity that a property specific to its type has just changed.
     * Does nothing by default.
     * @param name Name of the property that has changed.
     * @param value The new value.
     * @throws MapException if sprite cannot be loaded.
     */
    @Override
    protected void notifyPropertyChanged(String name, String value) throws MapException {

        if (name.equals("sprite")) {

            if (isValidSpriteName(value)) {
                try {
                    setSprite(new Sprite(value, getMap()));
                } catch (SpriteException ex) {
                    throw new MapException(ex.getMessage());
                }
            }
            else {
                setSprite(null);
            }
        }
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
        currentImageDescription.setRectangle(getDirection() * 16, 0, 16, 16);
    }
}

