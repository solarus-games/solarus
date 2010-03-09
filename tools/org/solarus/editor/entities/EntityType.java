/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
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

    TILE                 (0, "Tile", Tile.class, null),
    DESTINATION_POINT    (1, "Destination point", DestinationPoint.class, DestinationPoint.Subtype.class),
    TELETRANSPORTER      (2, "Teletransporter", Teletransporter.class, Teletransporter.Subtype.class),
    PICKABLE_ITEM        (3, "Pickable item", PickableItem.class, PickableItem.Subtype.class),
    DESTRUCTIBLE_ITEM    (4, "Destructible item", DestructibleItem.class, DestructibleItem.Subtype.class),
    CHEST                (5, "Chest", Chest.class, null),
    JUMP_SENSOR          (6, "Jump Sensor", JumpSensor.class, null),
    ENEMY                (7, "Enemy", Enemy.class, Enemy.Subtype.class),
    INTERACTIVE          (8, "Interactive entity", InteractiveEntity.class, InteractiveEntity.Subtype.class),
    BLOCK                (9, "Block", Block.class, Block.Subtype.class),
    DYNAMIC_TILE         (10, "Dynamic tile", DynamicTile.class, null),
    SWITCH               (11, "Switch", Switch.class, Switch.Subtype.class),
    CUSTOM_OBSTACLE      (12, "Custom obstacle", CustomObstacle.class, null),
    SENSOR               (13, "Sensor", Sensor.class, Sensor.Subtype.class),
    CRYSTAL_SWITCH       (14, "Crystal switch", CrystalSwitch.class, null),
    CRYSTAL_SWITCH_BLOCK (15, "Crystal switch block", CrystalSwitchBlock.class, CrystalSwitchBlock.Subtype.class),
    SHOP_ITEM            (16, "Shop item", ShopItem.class, null),
    CONVEYOR_BELT        (17, "Conveyor belt", ConveyorBelt.class, null),
    DOOR                 (18, "Door", Door.class, Door.Subtype.class),
    STAIRS               (19, "Stairs", Stairs.class, Stairs.Subtype.class)
    ;

    private final int index;
    private Class<? extends MapEntity> entityClass;
    private Class<? extends EntitySubtype> subtypeEnum;
    private String name;

    /**
     * Creates an entity type.
     * @param index index of the entity type to create
     * @param name a human readable name describing this entity type
     * @param entityClass subclass of MapEntity representing the entities of this type
     * @param subtypeEnum enumeration describing the subtypes of this type (or null
     * if there is no notion of subtype)
     */
    private EntityType(int index, String name, Class<? extends MapEntity> entityClass,
	    Class<? extends EntitySubtype> subtypeEnum) {
	this.index = index;
	this.name = name;
	this.entityClass = entityClass;
	this.subtypeEnum = subtypeEnum;
    }

    /**
     * Returns the entity type with the specified index.
     * @param index index of the entity type to get
     * @return the entity type with this index
     * @throws NoSuchElementException if the index is incorrect
     */
    public static EntityType get(int index) throws NoSuchElementException {

	for (EntityType t: values()) {
	    if (t.getIndex() == index) {
		return t;
	    }
	}

	throw new NoSuchElementException("Unknown entity type: " + index);
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
     * Returns the index of this entity type.
     * @return the index
     */
    public int getIndex() {
	return index;
    }

    /**
     * Returns the name of this kind of entity.
     * @return the name of this kind of entity
     */
    public String getName() {
	return name;
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
     * @param id id of the subtype to get
     * @return the subtype with this id for the current entity type
     */
    public EntitySubtype getSubtype(int id) {

	EntitySubtype subtype = null;
	try {
	    // call the get(int id) public static method from the enumeration 
	    subtype = (EntitySubtype) subtypeEnum.getMethod("get", int.class).invoke(null, id);
	}
	catch (NoSuchMethodException ex) {
	    System.err.println("The method 'get' is missing in enumeration " + subtypeEnum.getName());
	    ex.printStackTrace();
	    System.exit(1);
	}
	catch (IllegalAccessException ex) {
	    System.err.println("Cannot access method 'get' in enumeration " + subtypeEnum.getName() + ": ex.getMessage()");
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

	if (!hasSubtype()) {
	    return null;
	}
	return getSubtype(0);
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
	    name = names[((Enum) subtype).ordinal()];
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

