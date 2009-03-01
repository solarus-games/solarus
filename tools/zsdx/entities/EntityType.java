package zsdx.entities;

import zsdx.*;

/**
 * Represents the different types of map entities available in the editor.
 */
public enum EntityType {

    TILE              (0, TileOnMap.class, "Tile"),
    DESTINATION_POINT (1, DestinationPoint.class, "Destination point"),
    TELETRANSPORTER   (2, Teletransporter.class, "Teletransporter"),
    PICKABLE_ITEM     (3, PickableItem.class, "Pickable item"),
    DESTRUCTIBLE_ITEM (4, DestructibleItem.class, "Destructible item"),
    CHEST             (5, Chest.class, "Chest"),
    JUMP_SENSOR       (6, JumpSensor.class, "Jump Sensor"),
    ENEMY             (7, Enemy.class, "Enemy"),
    INTERACTIVE       (8, InteractiveEntity.class, "Interactive entity");

    private final int index;
    private Class<? extends MapEntity> entityClass;
    private String name;

    /**
     * Creates an entity type.
     * @param index index of the entity type to create
     * @param entityClass subclass of MapEntity representing the entities of this type
     * @param name name of this entity type
     */
    private EntityType(int index, Class<? extends MapEntity> entityClass, String name) {
	this.index = index;
	this.entityClass = entityClass;
	this.name = name;
    }

    /**
     * Returns the entity type with the specified index.
     * @param index index of the entity type to get
     * @return the entity type with this index
     * @throws MapException if the index is incorrect
     */
    public static EntityType get(int index) throws MapException {

	for (EntityType t: values()) {
	    if (t.getIndex() == index) {
		return t;
	    }
	}

	throw new MapException("Unknown entity type: " + index);
    }

    /**
     * Returns the entity type with the specified class.
     * @param entityClass class of the entity type to get
     * @return the entity type represented by this class
     * @throws MapException if the class is incorrect
     */
    public static EntityType get(Class<? extends MapEntity> entityClass) throws MapException {

	for (EntityType t: values()) {
	    if (t.getClass() == entityClass) {
		return t;
	    }
	}

	throw new MapException("Unknown entity class: " + entityClass.getName());
    }

    /**
     * Returns the index of this entity type.
     * @return the index
     */
    public int getIndex() {
	return index;
    }

    /**
     * Returns the subclass of MapEntity representing this type of entity.
     * @return the entity class
     */
    public Class<? extends MapEntity> getEntityClass() {
	return entityClass;
    }

    /**
     * Returns the name of this kind of entity.
     * @return the name of this kind of entity
     */
    public String getName() {
	return name;
    }
};
