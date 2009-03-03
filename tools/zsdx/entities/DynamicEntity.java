package zsdx.entities;

import zsdx.*;

/**
 * This class represents a non-static map entity, such as a chest, a door, a switch...
 */
public abstract class DynamicEntity extends MapEntity {

    /**
     * Creates a new interactive entity.
     * @param map the map
     * @param width width of the entity
     * @param height height of the entity
     */
    protected DynamicEntity(Map map, int width, int height) throws MapException {
	super(map, width, height);
    }
}
