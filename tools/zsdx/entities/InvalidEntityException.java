package zsdx.entities;

import zsdx.*;

/**
 * This exception is thrown when an invalid entity is detected on the map.
 */
public class InvalidEntityException extends MapException {

    private MapEntity entity;

    /**
     * Constructor.
     * @param message error message
     * @param entity the invalid entity detected
     */
    public InvalidEntityException(String message, MapEntity entity) {
	super(message);
	this.entity = entity;
    }
    
    /**
     * Returns the invalid entity that was detected.
     * @return the invalid entity
     */
    public MapEntity getEntity() {
	return entity;
    }
}
