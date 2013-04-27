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
 * Represents an entity that Link can destroy (lift and throw or cut)
 * and that can hide a treasure.
 */
public class Destructible extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("destructible_items.png", 0, 0, 16, 16),
        new EntityImageDescription("destructible_items.png", 16, 0, 16, 16),
        new EntityImageDescription("destructible_items.png", 32, 0, 16, 16),
        new EntityImageDescription("destructible_items.png", 48, 0, 16, 16),
        new EntityImageDescription("destructible_items.png", 64, 0, 16, 16),
        new EntityImageDescription("destructible_items.png", 80, 0, 16, 16),
        new EntityImageDescription("destructible_items.png", 96, 0, 16, 24),
    };

    /**
     * Size of each subtype.
     */
    private static final Dimension[] sizes = {
        new Dimension(16, 16),
        new Dimension(16, 16),
        new Dimension(16, 16),
        new Dimension(16, 16),
        new Dimension(16, 16),
        new Dimension(16, 16),
        new Dimension(16, 24),
    };

    /**
     * Origin point of a destructible item.
     */
    private static final Point[] origins = {
      new Point(8, 13),
      new Point(8, 13),
      new Point(8, 13),
      new Point(8, 13),
      new Point(8, 13),
      new Point(8, 13),
      new Point(8, 21),
    };

    /**
     * Subtypes of destructible items.
     */
    public enum Subtype implements EntitySubtype {
        POT("pot"),
        SKULL("skull"),  // TODO remove this (obsolete).
        BUSH("bush"),
        STONE_SMALL_WHITE("white_stone"),
        STONE_SMALL_BLACK("black_stone"),
        GRASS("grass"),
        BOMB_FLOWER("bomb_flower");

        public static final String[] humanNames = {
            "Pot",
            "Skull",
            "Bush",
            "Small white stone",
            "Small black stone",
            "Grass",
            "Bomb flower",
        };

        private final String id;

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
                    "No destructible item subtype with id '" + id + "'");
        }
    };

    /**
     * Creates a new destructible.
     * @param map the map
     */
    public Destructible(Map map) throws MapException {
        super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the origin point
     */
    protected Point getOrigin() {
        return origins[subtype.ordinal()];
    }

    /**
     * Returns the entity's obstacle property.
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
        setSizeImpl(size.width, size.height);

        setChanged();
        notifyObservers();
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

        if (subtype == Subtype.SKULL) {
            throw new MapException("The skull subtype is obsolete, please use pot instead");
        }

        Integer variant = getIntegerProperty("treasure_variant");
        if (variant != null && variant < 1) {
            throw new MapException("Invalid treasure variant: " + variant);
        }

        String savegameVariable = getStringProperty("treasure_savegame_variable");
        if (savegameVariable != null && !isValidSavegameVariable(savegameVariable)) {
            throw new MapException("Invalid treasure savegame variable");
        }
    }
}

