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
 * Represents a entity that the hero can push or pull.
 */
public class Block extends MapEntity {

  /**
   * The sprite representing this block.
   */
  private Sprite sprite;

  /**
   * Limitations of moves for blocks.
   */
  public enum MaximumMoves {
    NONE,
    ONE,
    INFINITE;

    public static final String[] humanNames = {
      "Cannot move",
      "One move only",
      "Infinite moves",
    };

    public static MaximumMoves get(int id) {
      return values()[id];
    }

    public int getId() {
      return ordinal();
    }
  }

  /**
   * Description of the default image representing this kind of entity.
   */
  public static final EntityImageDescription[] generalImageDescriptions = {
    new EntityImageDescription("miscellaneous_entities.png", 56, 0, 16, 16),
  };

  /**
   * Origin point of a block.
   */
  private static final Point origin = new Point(8, 13);

  /**
   * Creates a new block.
   * @param map the map
   */
  public Block(Map map) throws MapException {
    super(map, 16, 16);
    setDirection(-1);
  }

  /**
   * Returns the coordinates of the origin point of the entity.
   * @return the coordinates of the origin point of the entity
   */
  protected Point getOrigin() {

    return origin;
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
   * Returns whether this entity can have the special direction value -1
   * indicating that no direction is set.
   * @return true
   */
  public boolean canHaveNoDirection() {
    return true;
  }

  /**
   * Returns the text to display in the direction chooser for the 'no direction' option.
   * @return the text to display in the direction chooser for the 'no direction' option
   */
  public String getNoDirectionText() {
    return "Any";
  }

  /**
   * Returns whether this entity is an obstacle.
   * @return Obstacle.OBSTACLE
   */
  public Obstacle getObstacle() {
    return Obstacle.OBSTACLE;
  }

  /**
   * Sets the default values of all properties specific to the current entity type.
   */
  public void setPropertiesDefaultValues() throws MapException {
    setProperty("sprite", "entities/block");
    setProperty("canBePushed", true);
    setProperty("canBePulled", false);
    setProperty("maximumMoves", MaximumMoves.ONE.getId());
  }

  /**
   * Sets a property specific to this kind of entity.
   * @param name name of the property
   * @param value value of the property
   */
  public void setProperty(String name, String value) throws MapException {

    super.setProperty(name, value);

    if (name.equals("sprite")) {

      if (value.length() > 0) {
	sprite = new Sprite(value, map);
      }
      else {
	sprite = null;
      }
    }
  }

  /**
   * Checks the specific properties.
   * @throws MapException if a property is not valid
   */
  public void checkProperties() throws MapException {

    if (!isSpriteNameValid(getProperty("sprite"))) {
      throw new MapException("Invalid sprite name: '" + getProperty("sprite") + "'");
    }
  }

  /**
   * Returns whether the specified sprite name id is valid
   * @param sprite_name a sprite name
   * @return true if it is valid
   */
  private boolean isSpriteNameValid(String sprite_name) {
    return sprite_name != null
      && sprite_name.length() != 0
      && (sprite_name.charAt(0) != '_' || sprite_name.equals("_none"));
  }

  /**
   * Draws this entity on the map editor.
   * @param g graphic context
   * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
   * @param showTransparency true to make transparent pixels,
   * false to replace them by a background color
   */
  public void paint(Graphics g, double zoom, boolean showTransparency) {

    if (sprite == null) {
      // sprite not set yet: display a default block icon
      super.paint(g, zoom, showTransparency);
    }
    else {
      // display the appropriate sprite
      sprite.paint(g, zoom, showTransparency,
	  getX(), getY(), null, 0, 0);
    }
  }
}
