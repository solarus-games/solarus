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
package zsdx.entities;

import java.awt.*;
import zsdx.*;

/**
 * Represents a sensor that makes the hero jump towards a direction.
 */
public class JumpSensor extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("miscellaneous_entities.png", 0, 0, 32, 32)
    };

    /**
     * Minimum jump length allowed.
     */
    public static final int MINIMUM_JUMP_LENGTH = 16;

    /**
     * Creates a new jump sensor.
     * @param map the map
     */
    public JumpSensor(Map map) throws MapException {
	super(map, 32, 8);
	setDirection(6);
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
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("jumpLength", 40);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	int jumpLength = getIntegerProperty("jumpLength");
	if (jumpLength < MINIMUM_JUMP_LENGTH) {
	    throw new MapException("The minimum jump length is " + MINIMUM_JUMP_LENGTH + " pixels");
	}

	if (jumpLength % 8 != 0) {
	    throw new MapException("The jump length must be a multiple of 8 pixels");
	}
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
