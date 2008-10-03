package zsdx;

import java.awt.*;
import java.util.*;

/**
 * Represents an item that Link can pick on the map,
 * such as a rupee, a heart, a key, etc.
 */
public class PickableItem extends ActiveEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Pickable item";

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription generalImageDescription =
	new EntityImageDescription("pickable_items.png", 0, 0, 16, 16);
    
    /**
     * Origin point of a pickable item.
     */
    private static final Point origin = new Point(8, 13);
    
    // types of pickable items

    public static final int PICKABLE_ITEM_RANDOM            = -1;   /**< special value to indicate to choose another value
							       * randomly (including PICKABLE_ITEM_NONE) */
    public static final int PICKABLE_ITEM_NONE              = 0;    /**< special value to incicate that there is no pickable item */
    public static final int PICKABLE_ITEM_RUPEE_1           = 1;
    public static final int PICKABLE_ITEM_RUPEE_5           = 2;
    public static final int PICKABLE_ITEM_RUPEE_20          = 3;
    public static final int PICKABLE_ITEM_HEART             = 4;
    public static final int PICKABLE_ITEM_SMALL_MAGIC       = 5;
    public static final int PICKABLE_ITEM_BIG_MAGIC         = 6;
    public static final int PICKABLE_ITEM_FAIRY             = 7;
    public static final int PICKABLE_ITEM_BOMB_1            = 8;
    public static final int PICKABLE_ITEM_BOMB_5            = 9;
    public static final int PICKABLE_ITEM_BOMB_10           = 10;
    public static final int PICKABLE_ITEM_ARROW_1           = 11;
    public static final int PICKABLE_ITEM_ARROW_5           = 12;
    public static final int PICKABLE_ITEM_ARROW_10          = 13;
    public static final int PICKABLE_ITEM_SMALL_KEY         = 14;
    public static final int PICKABLE_ITEM_BIG_KEY           = 15;
    public static final int PICKABLE_ITEM_BOSS_KEY          = 16;
    public static final int PICKABLE_ITEM_PIECE_OF_HEART    = 17;
    public static final int PICKABLE_ITEM_HEART_CONTAINER   = 18;

    // specific fields of a pickable item

    /**
     * Type of pickable item.
     * It cannot be PICKABLE_ITEM_RANDOM or PICKABLE_ITEM_NONE: these
     * values are reserved for items obtained by destroying pots, bush, enemies, etc.
     */
    private int type;
    
    /**
     * A number indicating the savegame variable where this pickable item possession
     * state is saved.
     * It is used only for the pickable that Link can obtain only once and
     * thus that are saved (keys, pieces of hearts, etc.) and ignored for the normal items.
     */
    private int savegameIndex;

    /**
     * Creates a new pickable item at the specified location.
     * By default, the type is a rupee.
     * @param map the map
     * @param x x coordinate of the item
     * @param y y coordinate of the item
     */
    public PickableItem(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);

	// default field values
	type = PICKABLE_ITEM_RUPEE_1;
    }

    /**
     * Creates an existing pickable item from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public PickableItem(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);
	setSizeImpl(16, 16);

	// parse the fields
	try {
	    this.type = Integer.parseInt(tokenizer.nextToken());
	    this.savegameIndex = Integer.parseInt(tokenizer.nextToken());
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
    }

    /**
     * Returns a string describing this pickable item.
     * @return a string representation of the pickable item
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// add the specific properties of a pickable item
	buff.append('\t');
	buff.append(getPickableItemType());
	buff.append('\t');
	buff.append(getSavegameIndex());

	return buff.toString();
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_PICKABLE_ITEM
     */
    public int getType() {
	return ENTITY_PICKABLE_ITEM;
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
	int x = (getPickableItemType() - 1) * 16;
	currentImageDescription.setX(x);
    }

    /**
     * Returns the type of this pickable item.
     * @return the type of pickable item
     */
    public int getPickableItemType() {
	return type;
    }

    /**
     * Sets the properties of this pickable item.
     * @param type the type of pickable item
     * @param savegameIndex the savegame index of this pickable item, or zero if
     * this pickable item is not saved.
     * @throws ZSDXException if the type is incorrect or if the value of
     * savegameIndex doesn't correspond to the specified type
     */
    public void setPickableItem(int type, int savegameIndex) throws ZSDXException {
	
	if (type <= PICKABLE_ITEM_NONE) {
	    throw new ZSDXException("The type of pickable item cannot be 'None' or 'Random'");
	}
	
	this.type = type;
	this.savegameIndex = savegameIndex;
    }
    
    /**
     * Returns the savegame index identifying this pickable item (if any).
     * @return the savegame index where this pickable item is saved,
     * or any value if this pickable item cannot be saved
     */
    public int getSavegameIndex() {
	return savegameIndex;
    }
    
    /**
     * Sets the id identifying this pickable item (if any).
     * @param savegameIndex the savegame index where this pickable item is saved
     */
    public void setSavegameIndex(int savegameIndex) {
	this.savegameIndex = savegameIndex;
    }
}
