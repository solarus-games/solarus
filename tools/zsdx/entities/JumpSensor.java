package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * Represents a sensor that makes the hero jump towards a direction.
 */
public class JumpSensor extends ActiveEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Jump sensor";

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("miscellaneous_entities.png", 0, 0, 32, 32)
    };

    /**
     * Length of the jump in pixels.
     */
    private int jumpLength;

    /**
     * Creates a new jump sensor at the specified location.
     * @param map the map
     * @param x x coordinate of the entity to create
     * @param y y coordinate of the entity to create
     */
    public JumpSensor(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 8);
	this.jumpLength = 56;
	setDirection(6);
    }

    /**
     * Creates an existing jump sensor from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public JumpSensor(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);

	try {
	    this.jumpLength = Integer.parseInt(tokenizer.nextToken());
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
    }

    /**
     * Returns a string describing this jump sensor.
     * @return a string representation of the entity
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// add the specific properties of this kind of entity
	buff.append('\t');
	buff.append(jumpLength);

	return buff.toString();
    }

    /**
     * Checks the entity validity. An entity must be valid before it is saved.
     * @return true if the entity is valid
     */
    public boolean isValid() {
	return super.isValid() && jumpLength > 0;	
    }

    /**
     * Checks whether the specified size of the entity is correct.
     * This is a redefinition of MapEntity.checkSize because the size of a jump sensor
     * is constrained depending on its direction.
     * @param width the width to check
     * @param height the height to check
     * @throws MapException if the size is not correct
     */
    protected void checkSize(int width, int height) throws MapException {

	if (direction % 2 != 0) {
	    if (width != height) {
		throw new MapException("The bounding rectangle of a diagonal jump sensor must be square");
	    }
	}
	else if (direction % 4 == 0) {
	    if (width != 8) {
		throw new MapException("The width of a vertical jump sensor must be 8 pixels");
	    }
	}
	else {
	    if (height != 8) {
		throw new MapException("The height of a horizontal jump sensor must be 8 pixels");
	    }
	}
    }

    /**
     * Changes the size of the entity on the map.
     * This is a redefinition of MapEntity.setSize to adjust automatically
     * the specified size to the constraints of the jump sensor.
     * @param width width of the entity in pixels
     * @param height height of the entity in pixels
     * @throws MapException if the entity is not resizable,
     * or the size specified is lower than or equal to zero,
     * or the size specified is not divisible by 8
     */
    public void setSize(int width, int height) throws MapException {

	if (direction % 2 != 0) {
	    if (width != height) {
		width = height = Math.min(width, height);
	    }
	}
	else if (direction % 4 == 0) {
	    width = 8;
	}
	else {
	    height = 8;
	}

	super.setSize(width, height);
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_JUMP_SENSOR
     */
    public int getType() {
	return ENTITY_JUMP_SENSOR;
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 8
     */
    public int getNbDirections() {
	return 8;
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Returns whether or not the entity is resizable.
     * @return true
     */
    public boolean isResizable() {
	return true;
    }

    /**
     * Returns the length of the jump in pixels.
     * @return the jump length in pixels
     */
    public int getJumpLength() {
        return jumpLength;
    }

    /**
     * Sets the length of the jump.
     * @param jumpLength the jump length in pixels
     */
    public void setJumpLength(int jumpLength) {
        this.jumpLength = jumpLength;
	setChanged();
	notifyObservers();
    }

    /**
     * Draws the entity on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {
	// TODO
    }
}
