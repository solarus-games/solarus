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
 * A teletransporter is an entity that moves the hero to a destination point
 * on another map or the same map.
 */
public class Teletransporter extends MapEntity {

    /**
     * Subtypes of teletransporters.
     */
    public enum Subtype implements EntitySubtype {
	INVISIBLE,
	YELLOW,
	BLUE;

	public static final String[] humanNames = {
	  "Invisible", "Yellow", "Blue"  
	};

	public int getId() {
	    return ordinal();
	}

	public static Subtype get(int id) {
	    return values()[id];
	}
    }

    /**
     * Unitary size of a teletransporter.
     */
    private static final Dimension unitarySize = new Dimension(16, 16);

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("teletransporter.png", 0, 0, 32, 32), // invisible
	new EntityImageDescription("teletransporter.png", 32, 0, 16, 16), // yellow
    };

    /**
     * Resizable image of a teletransporter.
     */
    private static Image resizableTeletransporterImage;

    /**
     * Creates a new teletransporter.
     * @param map the map
     */
    public Teletransporter(Map map) throws MapException {
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
     * @return true if the entity is resizable
     */
    public boolean isResizable() {
	return !initialized || subtype == Subtype.INVISIBLE;
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
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     * @throws MapException if the subtype is not valid
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {

	if (subtype != this.subtype) {
	    if (isResizable()) {
		setSize(16, 16);
	    }
	    super.setSubtype(subtype);
	}
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("transition", Transition.FADE.getId());
	setProperty("destinationMapId", map.getId());
	setProperty("destinationPointName", "");
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	String destinationMapId = getProperty("destinationMapId");
	String destinationPointName = getProperty("destinationPointName");

	if (destinationMapId.length() == 0) {
	    throw new MapException("You must choose a destination map");
	}

	if (destinationPointName.length() == 0) {
	    throw new MapException("You must choose a destination point on the destination map");
	}

	if (destinationMapId.equals(map.getId()) &&
	    destinationPointName.equals("_same")) {
	    throw new MapException("Cannot teleport to the same point on the same map");
	}
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	if (subtype == Subtype.YELLOW) {
	    currentImageDescription.setRectangle(32, 0, 16, 16);
	}
	else if (subtype == Subtype.BLUE) {
	    currentImageDescription.setRectangle(32, 16, 16, 16);
	}
    }

    /**
     * Draws the teletransporter on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

	if (subtype == Subtype.INVISIBLE) {
	    if (resizableTeletransporterImage == null) {
		resizableTeletransporterImage = Project.getEditorImage("resizable_teletransporter.png");
	    }

	    int x = (int) (positionInMap.x * zoom);
	    int y = (int) (positionInMap.y * zoom);
	    int w = (int) (positionInMap.width * zoom);
	    int h = (int) (positionInMap.height * zoom);

	    g.setColor(new Color(240, 200, 80));
	    g.fillRect(x, y, w, h);

	    int dx1 = (int) ((positionInMap.x + positionInMap.width / 2 - 8) * zoom);
	    int dy1 = (int) ((positionInMap.y + positionInMap.height / 2 - 8) * zoom);
	    int dx2 = (int) (dx1 + 16 * zoom);
	    int dy2 = (int) (dy1 + 16 * zoom);

	    g.drawImage(resizableTeletransporterImage, dx1, dy1, dx2, dy2, 0, 0, 32, 32, null);

	    drawEntityOutline(g, zoom, new Color(240, 215, 142));
	}
	else {
	    super.paint(g, zoom, showTransparency);
	}
    }
}
