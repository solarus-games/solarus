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

import java.awt.*;
import org.solarus.editor.*;

/**
 * Represents a door to open with a key or with an event.
 */
public class Door extends MapEntity {

    /**
     * Subtypes of doors.
     */
    public enum Subtype implements EntitySubtype {
	CLOSED,
	SMALL_KEY,
	SMALL_KEY_BLOCK,
	BIG_KEY,
	BOSS_KEY,
	WEAK,
	VERY_WEAK,
	WEAK_INVISIBLE,
	;

	public static final String[] humanNames = {
	    "Closed",
	    "Use small key",
	    "Use small key (block)",
	    "Use big key",
	    "Use boss key",
	    "Weak",
	    "Very weak",
	    "Weak (invisible)",
	};

	public static Subtype get(int id) {
	    return values()[id];
	}

	public int getId() {
	    return ordinal();
	}

	public boolean mustBeSaved() {
	  return this != CLOSED;
	}
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("door.png", 8, 48, 16, 16),
	new EntityImageDescription("door.png", 40, 48, 16, 16),
	new EntityImageDescription("door.png", 0, 0, 16, 16),
	new EntityImageDescription("door.png", 72, 48, 16, 16),
	new EntityImageDescription("door.png", 104, 48, 16, 16),
	new EntityImageDescription("door.png", 136, 48, 16, 16),
	new EntityImageDescription("door.png", 136, 48, 16, 16),
	new EntityImageDescription("door.png", 136, 48, 16, 16),
    };

    /**
     * X coordinate of the door in the tileset entities image for each kind of entity
     * and for the direction top.
     */
    private static final int[] imageX = { 0, 64, 16, 96, 128, 192, 192, 192 };
        

    /**
     * Creates a new door.
     * @param map the map
     */
    public Door(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
      if (subtype != Subtype.WEAK) {
	return new Point(0, 0);
      }
      
      return (getDirection() % 2 == 0) ? new Point (0, -8) : new Point(-8, 0);
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }
    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
	return 4;
    }

    /**
     * Returns whether this entity is an obstacle.
     * @return Obstacle.OBSTACLE
     */
    public Obstacle getObstacle() {
	return Obstacle.OBSTACLE;
    }

    /**
     * Changes the direction of the entity.
     * @param direction the entity's direction
     * @throws UnsupportedOperationException if the entity has no direction
     * @throws IllegalArgumentException if the direction is invalid
     */
    public void setDirection(int direction) throws UnsupportedOperationException, IllegalArgumentException {
      super.setDirection(direction);
      setDoorSize();
    }

    /**
     * Sets the subtype of this entity.
     * @param subtype the subtype of entity
     */
    public void setSubtype(EntitySubtype subtype) throws MapException {
	int x = getX();
	int y = getY();

	super.setSubtype(subtype);

	setDoorSize();
	setPositionInMap(x, y);

	setChanged();
	notifyObservers();
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {

      String fileName = Project.getTilesetEntitiesImageFile(map.getTileset().getId()).getName();
      currentImageDescription.setImageFileName("tilesets/" + fileName, false);
      currentImageDescription.setSize(getWidth(), getHeight());

      if (getSubtype() == Subtype.SMALL_KEY_BLOCK) {
        currentImageDescription.setXY(16, 0);
      }
      else {
	int x = imageX[getSubtype().ordinal()];
	int y = 0;
	switch (getDirection()) {

	  case 0:
	    x = x / 2 + 112;
	    y = 16;
	    break;

	  case 1:
	    y = 48;
	    break;

	  case 2:
	    x = x / 2;
	    y = 16;
	    break;

	  case 3:
            y = 64;
	    break;

	}
	if (getSubtype() == Subtype.WEAK) {
	  if (getDirection() % 2 == 0) {
	    y += 8;
	  }
	  else {
	    x += 8;
	  }
	}
	currentImageDescription.setXY(x, y);
      }
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("savegameVariable", -1);
	setDirection(1);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	int savegameVariable = getIntegerProperty("savegameVariable");
	if (savegameVariable < -1 || savegameVariable >= 32768) {
	    throw new MapException("Invalid door savegame variable");
	}

	if (savegameVariable == -1 && mustBeSaved()) {
	  throw new MapException("This kind of door must be saved");
	}
    }

    /**
     * Sets the appropriate size of the door depending on its subtype and its direction.
     */
    private void setDoorSize() {

      if (getSubtype() == Subtype.SMALL_KEY_BLOCK || getSubtype() == Subtype.WEAK) {
	setSizeImpl(16, 16);
      }
      else if (getDirection() % 2 == 0) {
	setSizeImpl(16, 32);
      }
      else {
	setSizeImpl(32, 16);
      }
    }

    /**
     * Returns whether this door's state must be saved.
     * @return true if this door's state must be saved
     */
    private boolean mustBeSaved() {
      return subtype != Subtype.CLOSED;
    }
}

