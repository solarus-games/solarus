package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * Represents the destination point of a teletransporter in a map.
 */
public class DestinationPoint extends ActiveEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Destination point";

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("destination_point.png", 128, 0, 16, 16),
	new EntityImageDescription("destination_point.png", 0, 0, 32, 32)
    };

    /**
     * Indicates whether the direction of Link is changed when he arrives
     * on this destination. If this boolean is true, then Link' direction is
     * indicated by this entity's direction.
     */
    private boolean changeDirection;

    /**
     * Origin point of a destination point.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * Creates a new destination point at the specified location.
     * @param map the map
     * @param x x coordinate of the entity to create
     * @param y y coordinate of the entity to create
     */
    public DestinationPoint(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);
	setDirection(1);
    }

    /**
     * Creates an existing destination point from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public DestinationPoint(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);

	setSizeImpl(16, 16);

	try {
	    this.changeDirection = (Integer.parseInt(tokenizer.nextToken()) != 0);
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
    }

    /**
     * Returns a string describing this destination point.
     * @return a string representation of the destination point
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// add the specific properties of a destination point
	buff.append('\t');
	buff.append(changeDirection ? 1 : 0);

	return buff.toString();
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_DESTINATION_POINT
     */
    public int getType() {
	return ENTITY_DESTINATION_POINT;
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (8,13)
     */
    protected Point getOrigin() {
	return origin;
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4 if the destination point changes Link's direction, false otherwise
     */
    public int getNbDirections() {
	return changeDirection ? 4 : 0;
    }
    
    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }
    
    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	currentImageDescription.setRectangle(getDirection() * 32, 0, 32, 32);
    }

}
