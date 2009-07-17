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

import zsdx.*;
import java.awt.*;

/**
 * An special terrain where the hero is moved into a specific direction.
 */
public class ConveyorBelt extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("conveyor_belt.png", 128, 0, 16, 16)
    };

    /**
     * Unitary size of a teletransporter.
     */
    private static final Dimension unitarySize = new Dimension(16, 16);

    /**
     * Origin point of a conveyor belt.
     */
    private static final Point origin = new Point(0, 0);

    /**
     * Creates a new conveyor belt.
     * @param map the map
     */
    public ConveyorBelt(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (0, 0)
     */
    protected Point getOrigin() {
	return origin;
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 8
     */
    public int getNbDirections() {
	return 8;
    }

    /**
     * Returns whether or not the entity is currently resizable.
     * @return true
     */
    public boolean isResizable() {
	return true;
    }

    /**
     * Returns the minimum size of the entity (for a resizable entity).
     * When the entity is resized, its new size must be a multiple of this minimum size.
     * @return (16,16)
     */
    public Dimension getUnitarySize() {
	return unitarySize;
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	currentImageDescription.setRectangle(getDirection() * 16, 0, 16, 16);
    }

    /**
     * Draws the entity on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

	Rectangle rectangle = new Rectangle(positionInMap.x, positionInMap.y,
		unitarySize.width, unitarySize.height);

	for (int i = 0; i < getHeight(); i += unitarySize.height) {
	    rectangle.x = positionInMap.x;
	    for (int j = 0; j < getWidth(); j += unitarySize.width) {
		currentImageDescription.paint(g, zoom, showTransparency, rectangle);
		rectangle.x += unitarySize.width;
	    }
	    rectangle.y += unitarySize.height;
	}
    }
}
