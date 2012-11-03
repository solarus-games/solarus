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

import java.util.*;
import java.lang.reflect.*;

/**
 * Represents the different types of map entities available in the editor.
 */
public enum EntityType {

    // Java name          human name           Lua name           class                  subtype enum (optional)
    TILE                 ("Tile",              "tile",            Tile.class,            null),
    DESTINATION          ("Destination",       "destination",     Destination.class,     null),
    TELETRANSPORTER      ("Teletransporter",   "teletransporter", Teletransporter.class, null),
    PICKABLE             ("Pickable item",     "pickable",        Pickable.class,        null),
    DESTRUCTIBLE         ("Destructible item", "destructible",    Destructible.class,    Destructible.Subtype.class),
    CHEST                ("Chest",             "chest",           Chest.class,           null),
    JUMPER               ("Jumper",            "jumper",          Jumper.class,          null),
    ENEMY                ("Enemy",             "enemy",           Enemy.class,           null),
    NPC                  ("NPC",               "npc",             NPC.class,             NPC.Subtype.class),
    BLOCK                ("Block",             "block",           Block.class,           null),
    DYNAMIC_TILE         ("Dynamic tile",      "dynamic_tile",    DynamicTile.class,     null),
    SWITCH               ("Switch",            "switch",          Switch.class,          Switch.Subtype.class),
    WALL                 ("Wall",              "wall",            Wall.class,            null),
    SENSOR               ("Sensor",            "sensor",          Sensor.class,          null),
    CRYSTAL              ("Crystal",           "crystal",         Crystal.class,         null),
    CRYSTAL_BLOCK        ("Crystal block",     "crystal_block",   CrystalBlock.class,    CrystalBlock.Subtype.class),
    SHOP_ITEM            ("Shop item",         "shop_item",       ShopItem.class,        null),
    CONVEYOR_BELT        ("Conveyor belt",     "conveyor_belt",   ConveyorBelt.class,    null),
    DOOR                 ("Door",              "door",            Door.class,            Door.Subtype.class),
    STAIRS               ("Stairs",            "stairs",          Stairs.class,          Stairs.Subtype.class)
    ;

    private Class<? extends MapEntity> entityClass;
    private Class<? extends EntitySubtype> subtypeEnum;
    private String humanName;
    private String luaName;

    /**
     * Creates an entity type.
     * @param humanName A human readable name describing this entity type.
     * @param luaName Name of this entity type in Lua data files.
     * @param entityClass Subclass of MapEntity representing the entities of this type.
     * @param subtypeEnum Enumeration describing the subtypes of this type (or null
     * if there is no notion of subtype).
     */
    private EntityType(String humanName,
            String luaName,
            Class<? extends MapEntity> entityClass,
            Class<? extends EntitySubtype> subtypeEnum) {
        this.humanName = humanName;
        this.luaName = luaName;
        this.entityClass = entityClass;
        this.subtypeEnum = subtypeEnum;
    }

    /**
     * Returns the entity type with the specified class.
     * @param entityClass class of the entity type to get
     * @return the entity type represented by this class
     * @throws NoSuchElementException if the class is incorrect
     */
    public static EntityType get(Class<? extends MapEntity> entityClass) throws NoSuchElementException {

        for (EntityType t: values()) {
            if (t.getEntityClass() == entityClass) {
                return t;
            }
        }

        throw new NoSuchElementException("Unknown entity class: " + entityClass.getName());
    }

    /**
     * Returns the human name of this kind of entity.
     * @return The human name of this kind of entity.
     */
    public String getHumanName() {
        return humanName;
    }

    /**
     * Returns the Lua name of this kind of entity.
     * @return The Lua name of this kind of entity.
     */
    public String getLuaName() {
        return luaName;
    }

    /**
     * Returns the subclass of MapEntity representing this type of entity.
     * @return the entity class
     */
    public Class<? extends MapEntity> getEntityClass() {
        return entityClass;
    }

    /**
     * Returns the enumeration describing the subtype of this kind of entity.
     * @return the enumeration describing the subtype of this kind of entity
     */
    public Class<? extends EntitySubtype> getSubtypeEnum() {
        return subtypeEnum;
    }

    /**
     * Returns whether this entity type has a subtype.
     * @return true if this entity type has a subtype
     */
    public boolean hasSubtype() {
        return getSubtypeEnum() != null;
    }

    /**
     * Returns a subtype value corresponding to the specified id.
     * @param id Td of the subtype to get.
     * @return The subtype with this id for the current entity type.
     */
    public EntitySubtype getSubtype(String id) {

        EntitySubtype subtype = null;
        try {
            // call the get(int id) public static method from the enumeration
            subtype = (EntitySubtype) subtypeEnum
                    .getMethod("get", String.class).invoke(null, id);
        }
        catch (NoSuchMethodException ex) {
            System.err.println("The method 'get' is missing in enumeration "
                    + subtypeEnum.getName());
            ex.printStackTrace();
            System.exit(1);
        }
        catch (IllegalAccessException ex) {
            System.err.println("Cannot access method 'get' in enumeration "
                    + subtypeEnum.getName() + ": " + ex.getMessage());
            ex.printStackTrace();
            System.exit(1);
        }
        catch (InvocationTargetException ex) {
            ex.getCause().printStackTrace();
            System.exit(1);
        }

        return subtype;
    }

    /**
     * Returns the default subtype for this type of entity.
     * If the entity type has no subtype, null is returned.
     * @return the default subtype
     */
    public EntitySubtype getDefaultSubtype() {

        EntitySubtype defaultSubtype = null;
        try {
            if (hasSubtype()) {
                // Call the values() method from the enumeration.
                Object values = subtypeEnum.getMethod("values").invoke(null);
                defaultSubtype = (EntitySubtype) Array.get(values, 0);
            }
        }
        catch (NoSuchMethodException ex) {
            System.err.println("The method 'values' is missing in enumeration "
                    + subtypeEnum.getName());
            ex.printStackTrace();
            System.exit(1);
        }
        catch (IllegalAccessException ex) {
            System.err.println("Cannot access method 'values' in enumeration "
                    + subtypeEnum.getName() + ": " + ex.getMessage());
            ex.printStackTrace();
            System.exit(1);
        }
        catch (InvocationTargetException ex) {
            ex.getCause().printStackTrace();
            System.exit(1);
        }
        return defaultSubtype;
    }

    /**
     * Returns the human readable name of the specified subtype for the current type.
     * @param subtype a subtype of the current type
     * @return the human readable name of this subtype
     */
    public String getSubtypeName(EntitySubtype subtype) {

        String name = null;
        try {
            Class<? extends EntitySubtype> subtypeEnum = getSubtypeEnum();
            String[] names = (String[]) subtypeEnum.getField("humanNames").get(null);
            name = names[((Enum<?>) subtype).ordinal()];
        }
        catch (NoSuchFieldException ex) {
            System.err.println("The field 'humanNames' is missing in enumeration " + subtypeEnum.getName());
            ex.printStackTrace();
            System.exit(1);
        }
        catch (IllegalAccessException ex) {
            System.err.println("Cannot access field 'humanNames' in enumeration " + subtypeEnum.getName() + ": ex.getMessage()");
            ex.printStackTrace();
            System.exit(1);
        }

        return name;
    }
}

