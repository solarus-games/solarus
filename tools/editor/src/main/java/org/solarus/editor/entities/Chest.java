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
     * Description of the default images representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions =
        {new EntityImageDescription("entity_chest.png", 0, 0, 32, 32)};

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
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        createStringProperty("treasure_name", true, null);
        createIntegerProperty("treasure_variant", true, 1);
        createStringProperty("treasure_savegame_variable", true, null);
        createStringProperty("sprite", false, "entities/chest");
        createStringProperty("opening_method", true, OpeningMethod.BY_INTERACTION.getId());
        createStringProperty("opening_condition", true, null);
        createBooleanProperty("opening_condition_consumed", true, false);
        createStringProperty("cannot_open_dialog", true, null);
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

        OpeningMethod openingMethod = OpeningMethod.get(getStringProperty("opening_method"));
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

        Integer variant = getIntegerProperty("treasure_variant");
        if (variant != null && variant < 1) {
            throw new MapException("Invalid treasure variant: " + variant);
        }

        String savegameVariable = getStringProperty("treasure_savegame_variable");
        if (savegameVariable != null && !isValidSavegameVariable(savegameVariable)) {
            throw new MapException("Invalid treasure savegame variable");
        }
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
                setSprite(new Sprite(value, getMap()));
            }
        }
    }
}

