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
import java.util.NoSuchElementException;

/**
 * An orange or blue block that can be raised or lowered with the help of a crystal.
 */
public class CrystalBlock extends MapEntity {

    /**
     * Subtypes of raised blocks.
     */
    public enum Subtype implements EntitySubtype {
        // We use integers ids for historical reasons.
        INITIALLY_LOWERED("0"),
        INITIALLY_RAISED("1");

        public static final String[] humanNames = {
            "Initially lowered",
            "Initially raised"
        };

        private String id;

        private Subtype(String id) {
            this.id = id;
        }

        public String getId() {
            return id;
        }
        
        public String getAnimationName() {
            return this == INITIALLY_LOWERED ?
                    "orange_lowered" :
                    "blue_raised";
        }

        public static Subtype get(String id) {
            for (Subtype subtype: values()) {
                if (subtype.getId().equals(id)) {
                    return subtype;
                }
            }
            throw new NoSuchElementException(
                    "No crystal block subtype with id '" + id + "'");
        }
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("entity_crystal_block.png", 0, 0, 32, 32), // initially lowered
        new EntityImageDescription("entity_crystal_block.png", 32, 0, 32, 32)  // initially raised
    };

    /**
     * Unitary size of this type of entity.
     */
    private static final Dimension unitarySize = new Dimension(16, 16);

    /**
     * Creates a new crystal block.
     * @param map the map
     */
    public CrystalBlock(Map map) throws MapException {
        super(map, 16, 16);
        setSubtype(Subtype.INITIALLY_LOWERED);

        try {
            setSprite(new Sprite("entities/crystal_block", map));
        }
        catch (SpriteException ex) {
            // The sprite could not be loaded.
            // The default icon for this entity will be used instead.
        }
    }

    /**
     * Returns whether or not this entity is resizable.
     * @return true
     */
    public boolean isResizable() {
        return true;
    }

    /**
     * Returns the minimum size of the entity (for a resizable entity).
     * When the entity is resized, its new size must be a multiple of this minimum size.
     * @return the minimum size of the entity
     */
    public Dimension getUnitarySize() {
        return unitarySize;
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
        int x = (getSubtype() == Subtype.INITIALLY_LOWERED) ? 0 : 32;
        currentImageDescription.setX(x);
    }

    /**
     * Draws the entity on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

        Rectangle positionInMap = getPositionInMap();
        Rectangle rectangle = new Rectangle(positionInMap.x, positionInMap.y,
                unitarySize.width, unitarySize.height);
        Sprite sprite = getSprite();

        for (int i = 0; i < getHeight(); i += unitarySize.height) {
            rectangle.x = positionInMap.x;
            for (int j = 0; j < getWidth(); j += unitarySize.width) {

                if (sprite == null) {
                    currentImageDescription.paint(g, zoom, showTransparency, rectangle);
                }
                else {
                    String animationName = ((Subtype) getSubtype()).getAnimationName();
                    sprite.paint(
                            g,
                            zoom,
                            showTransparency,
                            rectangle.x,
                            rectangle.y,
                            animationName,
                            0,
                            sprite.getAnimation(animationName).getDirection(0).getNbFrames() - 1
                    );
                }
                rectangle.x += unitarySize.width;
            }
            rectangle.y += unitarySize.height;
        }
    }
}

