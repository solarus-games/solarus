package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * Represents a sensor that makes the hero jump towards a direction.
 */
public class JumpSensor extends DynamicEntity {

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
     * Minimum jump length allowed.
     */
    public static final int MINIMUM_JUMP_LENGTH = 16;

    /**
     * Creates a new jump sensor at the specified location.
     * @param map the map
     * @param x x coordinate of the entity to create
     * @param y y coordinate of the entity to create
     */
    public JumpSensor(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 32, 8);
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
	return super.isValid() && jumpLength >= 16;	
    }

    /**
     * Changes the size of the entity on the map.
     * This is a redefinition of MapEntity.setSize() to adjust automatically
     * the specified size to the constraints of the jump sensor.
     * @param width width of the entity in pixels
     * @param height height of the entity in pixels
     * @throws MapException if the size is not correct
     */
    public void setSize(int width, int height) throws MapException {

	Dimension adjustedSize = getSizeAdjustedToDirection(width, height);
	super.setSize(adjustedSize.width, adjustedSize.height);
    }

    /**
     * Adjusts the specified size with respect to the current direction's constraints.
     * @param width a width
     * @param height a height
     * @return a new adjusted size
     */
    private Dimension getSizeAdjustedToDirection(int width, int height) {
	// TODO move into a redefinition of checkSize
/*
	if (direction % 2 != 0) {
	    if (width != height) {
		width = height = Math.max(16, Math.max(width, height));
	    }
	}
	else if (direction % 4 == 0) {
	    width = 8;
	}
	else {
	    height = 8;
	}*/
	
	return new Dimension(width, height);
    }

    /**
     * Computes a default size for a specified direction.
     * @param a direction
     * @return a default size for that direction
     */
    private Dimension getDefaultSizeForDirection(int direction) {

	int width, height;
	if (direction % 2 != 0) {
	    width = height = 32;
	}
	else if (direction % 4 == 0) {
	    width = 8;
	    height = 32;
	}
	else {
	    width = 32;
	    height = 8;
	}
	
	return new Dimension(width, height);
    }

    /**
     * Changes the direction of the entity.
     * This is a redefinition of MapEntity.setDirection() to adjust the size to the new direction.
     * @param direction the entity's direction
     * @throws UnsupportedOperationException if the entity has no direction
     * @throws IllegalArgumentException if the direction is invalid
     */
    public void setDirection(int direction) throws UnsupportedOperationException, IllegalArgumentException {
	
	if (direction != getDirection()) {
	    super.setDirection(direction);

	    // if the size is initialized, adjust it to this new direction
	    try {
		if (getWidth() != 0) {
		    Dimension size = getDefaultSizeForDirection(direction);
		    setSize(size.width, size.height);
		}
	    }
	    catch (MapException ex) {
		// should not happen
		System.err.println("Unexpected error: " + ex.getMessage());
		System.exit(1);
	    }
	}
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
     * Returns whether or not the entity can be resized by extending it
     * horizontally or vertically
     * @param direction one of the two main directions (0: horizontal, 1: vertical)
     * @return whether or not the entity can be expanded in that direction
     */
    public boolean isExtensible(int extensionDirection) {

	if (getDirection() % 2 != 0) {
	    return true;
	}

	if (getDirection() % 4 == 0) {
	    return (extensionDirection == 1);
	}

	return (extensionDirection == 0);
    }

    /**
     * Returns whether the entity has to remain square when it is being resized.
     * @return true if the entity must be square
     */
    public boolean mustBeSquare() {
	return direction % 2 != 0;
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
     * @throws MapException if the jump length is not correct
     */
    public void setJumpLength(int jumpLength) throws MapException {
	if (jumpLength < MINIMUM_JUMP_LENGTH) {
	    throw new MapException("The minimum jump length is " + MINIMUM_JUMP_LENGTH + " pixels");
	}
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

	int x = (int) (positionInMap.x * zoom);
	int y = (int) (positionInMap.y * zoom);
	int w = (int) (positionInMap.width * zoom);
	int h = (int) (positionInMap.height * zoom);

	int dx1 = (int) ((positionInMap.x + positionInMap.width / 2 - 8) * zoom);
	int dy1 = (int) ((positionInMap.y + positionInMap.height / 2 - 8) * zoom);
	int dx2 = (int) (dx1 + 16 * zoom);
	int dy2 = (int) (dy1 + 16 * zoom);

	dx1 = (int) (positionInMap.x * zoom);
	dy1 = (int) (positionInMap.y * zoom);
	dx2 = (int) (dx1 + positionInMap.width * zoom);
	dy2 = (int) (dy1 + positionInMap.height * zoom);

	Color fillColor = new Color(48, 184, 208);
	Color borderColor = new Color(144, 224, 240);
	if (direction % 2 == 0) {

	    g.setColor(fillColor);
	    g.fillRect(x, y, w, h);

	    g.setColor(Color.black);
	    g.drawLine(dx1, dy1, dx2 - 1, dy1);
	    g.drawLine(dx1 + 3, dy1 + 3, dx2 - 4, dy1 + 3);
	    g.drawLine(dx1, dy2 - 1, dx2 - 1, dy2 - 1);
	    g.drawLine(dx1 + 3, dy2 - 4, dx2 - 4, dy2 - 4);
	    g.drawLine(dx1, dy1, dx1, dy2 - 1);
	    g.drawLine(dx1 + 3, dy1 + 3, dx1 + 3, dy2 - 4);
	    g.drawLine(dx2 - 1, dy1, dx2 - 1, dy2 - 1);
	    g.drawLine(dx2 - 4, dy1 + 3, dx2 - 4, dy2 - 4);

	    g.setColor(borderColor);
	    g.drawLine(dx1 + 1, dy1 + 1, dx2 - 2, dy1 + 1);
	    g.drawLine(dx1 + 2, dy1 + 2, dx2 - 3, dy1 + 2);
	    g.drawLine(dx1 + 2, dy2 - 3, dx2 - 3, dy2 - 3);
	    g.drawLine(dx1 + 1, dy2 - 2, dx2 - 2, dy2 - 2);
	    g.drawLine(dx1 + 1, dy1 + 1, dx1 + 1, dy2 - 2);
	    g.drawLine(dx1 + 2, dy1 + 2, dx1 + 2, dy2 - 3);
	    g.drawLine(dx2 - 3, dy1 + 2, dx2 - 3, dy2 - 3);
	    g.drawLine(dx2 - 2, dy1 + 1, dx2 - 2, dy2 - 2);
	}
	else {

	    int[] xPoints, yPoints, xPoints2, yPoints2;
	    int nPoints = 5;
	    int thickness = (int) (8 * zoom);

	    switch (getDirection()) {

	    case 1: // right-up
		xPoints = new int[] {dx1, dx2 - 1, dx2 - thickness, dx1, dx1};
		yPoints = new int[] {dy1, dy2 - 1, dy2 - 1, dy1 + thickness - 1, dy1};
		xPoints2 = new int[] {dx1 + 3, dx2 - 8, dx2 - thickness + 1, dx1 + 3, dx1 + 3};
		yPoints2 = new int[] {dy1 + 7, dy2 - 4, dy2 - 4, dy1 + thickness - 2, dy1 + 7};
		break;

	    case 3: // left-up
		xPoints = new int[] {dx2 - 1, dx1, dx1, dx2 - thickness, dx2 - 1};
		yPoints = new int[] {dy1, dy2 - 1, dy2 - thickness, dy1, dy1};
		xPoints2 = new int[] {dx2 - 8, dx1 + 3, dx1 + 3, dx2 - thickness + 1, dx2 - 8};
		yPoints2 = new int[] {dy1 + 3, dy2 - 8, dy2 - thickness + 1, dy1 + 3, dy1 + 3};
		break;

	    case 5: // left-down
		xPoints = new int[] {dx1, dx1 + thickness - 1, dx2 - 1, dx2 - 1, dx1};
		yPoints = new int[] {dy1, dy1, dy2 - thickness, dy2 - 1, dy1};
		xPoints2 = new int[] {dx1 + 7, dx1 + thickness - 2, dx2 - 4, dx2 - 4, dx1 + 7};
		yPoints2 = new int[] {dy1 + 3, dy1 + 3, dy2 - thickness + 1, dy2 - 8, dy1 + 3};
		break;

	    case 7: // right-down
		xPoints = new int[] {dx2 - 1, dx1, dx1 + thickness - 1, dx2 - 1, dx2 - 1};
		yPoints = new int[] {dy1, dy2 - 1, dy2 - 1, dy1 + thickness - 1, dy1};
		xPoints2 = new int[] {dx2 - 4, dx2 - 4, dx1 + thickness - 2, dx1 + 7, dx2 - 4};
		yPoints2 = new int[] {dy1 + 7, dy1 + thickness - 2, dy2 - 4, dy2 - 4, dy1 + 7};
		break;

	    default:
		throw new IllegalArgumentException("Unexpected error: unknown jump direction " + getDirection());
	    }
	    g.setColor(borderColor);
	    g.fillPolygon(xPoints, yPoints, nPoints);
	    g.setColor(Color.black);
	    g.drawPolyline(xPoints, yPoints, nPoints);
	    g.setColor(fillColor);
	    g.fillPolygon(xPoints2, yPoints2, nPoints);
	    g.setColor(Color.black);
	    g.drawPolyline(xPoints2, yPoints2, nPoints);
	}
    }
}
