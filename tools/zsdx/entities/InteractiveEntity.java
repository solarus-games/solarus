package zsdx.entities;

import java.awt.*;
import zsdx.Map;
import zsdx.*;

/**
 * Represents an entity that triggers a message or an event when the player
 * presses the action key in front of it.
 */
public class InteractiveEntity extends DynamicEntity {

    /**
     * Subtypes of interactive entities.
     */
    public enum Subtype implements EntitySubtype {
	CUSTOM,
	NON_PLAYING_CHARACTER,
	SIGN,
	WATER_FOR_BOTTLE;

	public static final String[] humanNames = {
	    "Custom",
	    "Non-playing character",
	    "Sign",
	    "Water for bottle"
	};

	public static Subtype get(int id) {
	    return values()[id];
	}

	public int getId() {
	    return ordinal();
	}
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("interactive_entities.png", 0, 0, 32, 32),
	new EntityImageDescription("interactive_entities.png", 32, 0, 16, 16),
	new EntityImageDescription("interactive_entities.png", 48, 0, 16, 16),
	new EntityImageDescription("interactive_entities.png", 64, 0, 32, 32)
    };

    /**
     * Description of the image currently representing this kind of entity.
     */
    public static final EntityImageDescription[] currentImageDescriptions = {
	generalImageDescriptions[0],
	new EntityImageDescription("interactive_entities.png", 32, 0, 16, 24),
	generalImageDescriptions[2],
	generalImageDescriptions[3]
    };

    /**
     * Origin point of each type of interactive entity.
     */
    private static final Point[] origins = {
	new Point(0, 0),
	new Point(8, 21),
	new Point(8, 13),
	new Point(0, 0)
    };

    /**
     * Size of each type of interactive entity.
     */
    private static final Dimension[] sizes = {
	new Dimension(16, 16),
	new Dimension(16, 24),
	new Dimension(16, 16),
	new Dimension(16, 16)
    };

    // specific fields

    /**
     * Creates a new interactive entity.
     * @param map the map
     */
    public InteractiveEntity(Map map) throws MapException {
	super(map, 16, 16);
	setDirection(3);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
	return origins[getSubtypeId()];
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
	return 4;
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Returns whether this entity is an obstacle.
     * @return true
     */
    public int getObstacle() {
	return OBSTACLE;
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {
	super.setSubtype(subtype);

	Dimension size = sizes[getSubtypeId()];
	setSizeImpl(size.width, size.height);

	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether the specified sprite name or message id is valid
     * @param value a sprite name or a message id
     * @return true if it is valid
     */
    private boolean isStringValid(String value) {
	return (value != null &&
		value.length() != 0 && 
		(value.charAt(0) != '_' || value.equals("_none")));
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	currentImageDescription = currentImageDescriptions[getSubtypeId()];
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("sprite", "");
	setProperty("messageId", "");
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	if (!isStringValid(getProperty("sprite"))) {
	    throw new MapException("Invalid sprite name");
	}

	if (!isStringValid(getProperty("messageId"))) {
	    throw new MapException("Invalid message id");
	}
    }
}
