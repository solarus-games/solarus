package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * Represents the destination point of a teletransporter in a map.
 */
public class DestinationPoint extends DynamicEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Destination point";

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("destination_point.png", 32, 0, 32, 32),    // invisible (up)
	new EntityImageDescription("destination_point.png", 128, 0, 16, 16)    // gray
    };

    // subtypes of destination points
    public static final int INVISIBLE         = 0;
    public static final int GRAY              = 1;

    /**
     * Indicates whether the direction of Link is changed when he arrives
     * on this destination. If this boolean is true, then Link' direction is
     * indicated by this entity's direction.
     */
    private boolean changeDirection;

    /**
     * Indicates whether the destination point is visible.
     */
    private int subtype;

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
	this.subtype = -1;
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
	    this.subtype = Integer.parseInt(tokenizer.nextToken());
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
	buff.append(subtype);

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
     * @return 4
     */
    public int getNbDirections() {
	return 4;
    }

    /**
     * Returns the direction of the entity.
     * This is a redefinition of MapEntity.setDirection to handle the special value of -1
     * indicating that the direction is not changed.
     * @return the entity's direction
     */
    public int getDirection() {

	if (!changeDirection) {
	    return -1;
	}

	return super.getDirection();
    }

    /**
     * Changes the direction of the entity.
     * This is a redefinition of MapEntity.setDirection to handle the special value of -1
     * indicating that the direction is not changed.
     * @param direction the entity's direction, or -1
     * @throws UnsupportedOperationException if the entity has no direction
     * @throws IllegalArgumentException if the direction is invalid
     */
    public void setDirection(int direction) throws UnsupportedOperationException, IllegalArgumentException {
	if (direction == -1) {
	    changeDirection = false;
	}
	else {
	    changeDirection = true;
	    super.setDirection(direction);
	}
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Returns the subtype of this entity.
     * @return the subtype of this entity
     */
    public int getSubtypeIndex() {
	return subtype;
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     * @throws MapException if the subtype is not valid
     */
    public void setSubtypeIndex(int subtype) throws MapException {

	if (subtype != this.subtype) {

	    if (this.subtype == -1) {
		setDefaultValues(subtype);
	    }

	    this.subtype = subtype;
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Sets the default options of this destination point for the specified subtype.
     * @param subtype a subtype
     */
    public void setDefaultValues(int subtype) {

	if (subtype == INVISIBLE) {
	    setDirection(1);
	}
	else {
	    setDirection(-1);
	}
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	if (subtype == INVISIBLE) {
	    currentImageDescription.setRectangle(getDirection() * 32, 0, 32, 32);
	}
	else {
	    currentImageDescription.setRectangle(128, 0, 16, 16);	    
	}
    }

}
