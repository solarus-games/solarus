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
 * Represents an item that Link can pick on the map,
 * such as a rupee, a heart, a key, etc.
 */
public class PickableItem extends MapEntity {

    /**
     * Subtypes of pickable items.
     */
    public enum Subtype implements EntitySubtype {
	RANDOM          (-1), /**< special value to indicate to choose another value
	                       * randomly (including PICKABLE_ITEM_NONE) */
	NONE            (0),  /**< special value to indicate that there is no pickable item */
	RUPEE_1         (1),
	RUPEE_5         (2),
	RUPEE_20        (3),
	HEART           (4),
	SMALL_MAGIC     (5),
	BIG_MAGIC       (6),
	FAIRY           (7),
	BOMB_1          (8),
	BOMB_5          (9),
	BOMB_10         (10),
	ARROW_1         (11),
	ARROW_5         (12),
	ARROW_10        (13),
	SMALL_KEY       (14),
	BIG_KEY         (15),
	BOSS_KEY        (16),
	PIECE_OF_HEART  (17),
	HEART_CONTAINER (18);

	public static final String[] humanNames = {
	    "Random",
	    "None",
	    "Green rupee",
	    "Blue rupee",
	    "Red rupee",
	    "Heart",
	    "Small magic",
	    "Big magic",
	    "Fairy",
	    "1 bomb",
	    "5 bombs",
	    "10 bombs",
	    "1 arrow",
	    "5 arrows",
	    "10 arrows",
	    "Small key",
	    "Big key",
	    "Boss key",
	    "Piece of heart",
	    "Heart container"
	};

	private int id;

	/**
	 * Creates a subtype.
	 * @param id id of the subtype
	 */
	private Subtype(int id) {
	    this.id = id;
	}

	/**
	 * Returns the subtype with the specified id
	 * @param id id of the subtype to get
	 * @return the subtype with this id
	 */
	public static Subtype get(int id) {
	    return values()[id + 1];
	}

	/**
	 * Returns the id of the subtype
	 * @return the id
	 */
	public int getId() {
	    return id;
	}

	/**
	 * Returns whether this subtype of pickable item is saved.
	 * @return true if this subtype of pickable item is saved
	 */
	public boolean isSaved() {
	    return id >= SMALL_KEY.getId();
	}

	/**
	 * Returns whether this subtype of pickable item is available only in dungeons.
	 * @return true if this subtype of pickable item is available only in dungeons
	 */
	public boolean isOnlyInDungeon() {
	    return this == BIG_KEY || this == BOSS_KEY;
	}
    }

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions =
	new EntityImageDescription[Subtype.values().length];

    static {
	for (int i = 2; i < generalImageDescriptions.length; i++) {
	    generalImageDescriptions[i] = new EntityImageDescription("pickable_items.png", 16 * (i - 2), 0, 16, 16);
	}
    }

    /**
     * Origin point of a pickable item.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * Creates a new pickable item.
     * @param map the map
     */
    public PickableItem(Map map) throws MapException {
	super(map, 16, 16);
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (8,13)
     */
    protected Point getOrigin() {
	return origin;
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	int x = (getSubtypeId() - 1) * 16;
	currentImageDescription.setX(x);
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void setPropertiesDefaultValues() throws MapException {
	setProperty("savegameVariable", -1);
	setSubtype(Subtype.RUPEE_1);
    }

    /**
     * Checks the specific properties.
     * @throws MapException if a property is not valid
     */
    public void checkProperties() throws MapException {

	if (subtype == Subtype.NONE || subtype == Subtype.RANDOM) {
	    throw new MapException("The subtype of a pickable item cannot be 'None' or 'Random'");
	}

	int savegameVariable = getIntegerProperty("savegameVariable");
	if (savegameVariable < -1 || savegameVariable >= 32768) {
	    throw new MapException("Invalid savegame variable");
	}

	boolean saved = ((Subtype) subtype).isSaved();
	if (saved && savegameVariable == -1) {
	    throw new MapException("This pickable item must be saved");
	}

	if (!saved && savegameVariable != -1) {
	    throw new MapException("This pickable item cannot be saved");
	}

	boolean inDungeon = map.isInDungeon();
	boolean mustBeInDungeon = ((Subtype) subtype).isOnlyInDungeon();
	if (mustBeInDungeon && !inDungeon) {
	    throw new MapException("This pickable item is available only in a dungeon");
	}

	if ((Subtype) subtype == Subtype.SMALL_KEY && !map.hasSmallKeys()) {
	    throw new MapException("The small keys are not enabled in this map");
	}
    }
}
