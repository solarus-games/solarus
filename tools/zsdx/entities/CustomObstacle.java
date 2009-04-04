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
 * A custom obstacle is an invisible entity that is an obstacle for some other types of entities.
 */
public class CustomObstacle extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("miscellaneous_entities.png", 0, 32, 32, 32)
    };

    /**
     * Resizable image of a custom obstacle.
     */
    private static Image resizableObstacleImage;

    /**
     * Constructor.
     * @param map the map
     */
    public CustomObstacle(Map map) throws MapException {
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
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("stopsHero", false);
	setProperty("stopsEnemies", false);
	setProperty("stopsNPCs", false);
	setProperty("stopsBlocks", false);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	if (!getBooleanProperty("stopsHero") &&
		!getBooleanProperty("stopsEnemies") &&
		!getBooleanProperty("stopsNPCs") &&
		!getBooleanProperty("stopsBlocks")) {
	    throw new MapException("This entity should be obstacle for at least one type of entity to have an effect");
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

	if (resizableObstacleImage == null) {
	    resizableObstacleImage = Project.getEditorImage("resizable_custom_obstacle.png");
	}

	int x1 = (int) (positionInMap.x * zoom);
	int y1 = (int) (positionInMap.y * zoom);
	int w = (int) (positionInMap.width * zoom);
	int h = (int) (positionInMap.height * zoom);
	int x2 = x1 + w;
	int y2 = y1 + h;
	int step = (int) (8 * zoom);

	for (int x = x1; x < x2; x += step) {
	    for (int y = y1; y < y2 ; y += step) {
		g.drawImage(resizableObstacleImage, x, y, x + step, y + step, 0, 0, 8, 8, null);
	    }
	}
	drawEntityOutline(g, zoom, new Color(240, 142, 142));
    }
}
