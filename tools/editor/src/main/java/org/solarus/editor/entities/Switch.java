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

import java.util.NoSuchElementException;

import org.solarus.editor.*;

/**
 * A switch is a button that can be enabled by the hero or a block.
 */
public class Switch extends MapEntity {

    /**
     * Subtypes of switches.
     */
    public enum Subtype implements EntitySubtype {
        WALKABLE("walkable"),
        ARROW_TARGET("arrow_target"),
        SOLID("solid");

        public static final String[] humanNames = {
          "Walkable",
          "Arrow target",
          "Solid"
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
                    "No switch subtype with id '" + id + "'");
        }

        public static boolean isWalkable(EntitySubtype subtype) {
            return subtype == WALKABLE;
        }
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
        new EntityImageDescription("entity_switch.png", 0, 0, 32, 32),
        new EntityImageDescription("entity_switch.png", 0, 0, 32, 32),
        new EntityImageDescription("entity_switch.png", 0, 0, 32, 32),
    };

    /**
     * Creates a new switch.
     * @param map the map
     */
    public Switch(Map map) throws MapException {
        super(map, 16, 16);
    }

    /**
     * Returns whether this switch is walkable.
     * @return true if the subtype is WALKABLE.
     */
    public boolean isWalkable() {
        return Subtype.isWalkable(getSubtype());
    }

    /**
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        createStringProperty("sprite", true, null);
        createStringProperty("sound", true, null);
        createBooleanProperty("needs_block", false, false);
        createBooleanProperty("inactivate_when_leaving", false, false);
        setSubtype(Subtype.WALKABLE);
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

            if (value != null && value.length() > 0) {
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
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

        String spriteName = getStringProperty("sprite");
        if (!isSpriteOrSoundNameValid(spriteName)) {
            throw new MapException("Invalid sprite name: '" + spriteName + "'");
        }

        String soundId = getStringProperty("sound");
        if (!isSpriteOrSoundNameValid(soundId)) {
            throw new MapException("Invalid sound id: '" + soundId + "'");
        }

        if (!isWalkable()) {

            if (getBooleanProperty("needs_block")) {
                throw new MapException("Cannot put a block on a non-walkable switch");
            }

            if (getBooleanProperty("inactivate_when_leaving")) {
                throw new MapException("Cannot inactivate the switch when leaving for a non-walkable switch");
            }
        }
    }

    /**
     * Returns whether the specified teletransporter sprite or sound name is valid.
     * @param name A sprite or sound name.
     * @return true if it is valid.
     */
    private boolean isSpriteOrSoundNameValid(String name) {
        return name == null || !name.isEmpty();
    }

}

