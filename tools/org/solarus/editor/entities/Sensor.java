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
package org.solarus.editor.entities;

import org.solarus.editor.*;
import java.awt.*;

/**
 * A sensor is a place that triggers something when the hero
 * or other entities walk on it.
 * Unlike switches and teletransportes, the hero cannot avoid a sensor
 * when he is jumping.
 */
public class Sensor extends MapEntity {

    /**
     * Subtypes of sensors.
     * The subtype indicate what happens when the sensor is activated.
     */
    public enum Subtype implements EntitySubtype {
	CUSTOM,                 /**< the script is called when the hero comes on this sensor */
	CHANGE_LAYER,           /**< the hero's layer is changed when he comes on this sensor
	                         * (this layer becomes the sensor's layer) */
	RETURN_FROM_BAD_GROUND; /**< when falling in a hole or in other bad grounds, the hero will
	                         * come back at the location of this sensor */

	public static final String[] humanNames = {
	  "Custom (call the script)",
	  "Change the layer",
	  "Place to return from holes and other bad grounds"
	};

	public int getId() {
	    return ordinal();
	}

	public static Subtype get(int id) {
	    return values()[id];
	}
    }

    /**
     * Origin point of this entity type.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * Unitary size of this entity type.
     */
    private static final Dimension unitarySize = new Dimension(16, 16);

    /**
     * Resizable image of this entity type.
     */
    private static Image resizableImage;

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("sensors.png", 0, 0, 32, 32),  // custom
	new EntityImageDescription("sensors.png", 32, 0, 32, 32), // change layer
	new EntityImageDescription("sensors.png", 64, 0, 32, 32), // return place for holes
    };

    /**
     * Creates a new switch.
     * @param map the map
     */
    public Sensor(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Returns whether or not the entity is currently resizable.
     * @return true
     */
    public boolean isResizable() {
	return subtype != Subtype.RETURN_FROM_BAD_GROUND;
    }

    /**
     * Returns whether two entities of this type can have different
     * sizes, even if they are not resizable.
     * @return true
     */
    public boolean isSizeVariable() {
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
     * Returns the coordinates of the origin point of the entity.
     * @return (8,13)
     */
    protected Point getOrigin() {
	return origin;
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setSubtype(Subtype.CUSTOM);
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     * @throws MapException if the subtype is not valid
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {

	if (subtype != this.subtype) {
	    if (isResizable() && subtype == Subtype.RETURN_FROM_BAD_GROUND) {
		setSize(16, 16);
	    }
	    super.setSubtype(subtype);
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

	if (resizableImage == null) {
	    resizableImage = Project.getEditorImage("resizable_sensors.png");
	}

	int x = (int) (positionInMap.x * zoom);
	int y = (int) (positionInMap.y * zoom);
	int w = (int) (positionInMap.width * zoom);
	int h = (int) (positionInMap.height * zoom);

	g.setColor(new Color(64, 160, 64));
	g.fillRect(x, y, w, h);

	int dx1 = (int) ((positionInMap.x + positionInMap.width / 2 - 8) * zoom);
	int dy1 = (int) ((positionInMap.y + positionInMap.height / 2 - 8) * zoom);
	int dx2 = (int) (dx1 + 16 * zoom);
	int dy2 = (int) (dy1 + 16 * zoom);

	int sx1 = 32 * getSubtypeId();
	int sx2 = sx1 + 32;

	g.drawImage(resizableImage, dx1, dy1, dx2, dy2, sx1, 0, sx2, 32, null);

	drawEntityOutline(g, zoom, new Color(142, 240, 142));
    }
}
