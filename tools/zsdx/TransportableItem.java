package zsdx;

import java.awt.*;
import java.util.*;

/**
 * Represents an entity that Link can lift, carry and throw:
 * a pot, a bush, etc.
 */
public class TransportableItem extends InteractiveEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Transportable item";

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription generalImageDescription =
	new EntityImageDescription("transportable_items.png", 0, 0, 16, 16);

    /**
     * Origin point of a transportable item.
     */
    private static final Point origin = new Point(8, 13);
    
    // types of transportable items

    public static final int TRANSPORTABLE_ITEM_POT               = 0;
    public static final int TRANSPORTABLE_ITEM_SKULL             = 1;
    public static final int TRANSPORTABLE_ITEM_BUSH              = 2;
    public static final int TRANSPORTABLE_ITEM_STONE_SMALL_WHITE = 3;
    public static final int TRANSPORTABLE_ITEM_STONE_SMALL_BLACK = 4;
    public static final int TRANSPORTABLE_ITEM_STONE_BIG_WHITE   = 5;
    public static final int TRANSPORTABLE_ITEM_STONE_BIG_BLACK   = 6;

    // specific fields of a transportable item

    /**
     * Type of transportable item.
     */
    private int type;

    /**
     * Type of pickable item that appears when Link lifts
     * the transportable item.
     */
    private int pickableItemType;
    
    /**
     * The index where the pickable item is saved, used only for the pickable
     * items that are saved (keys, pieces of hearts, etc.).
     */
    private int pickableItemSavegameIndex;

    /**
     * Creates a new transportable item at the specified location.
     * By default, the type is a pot and the pickable item is random. 
     * @param map the map
     * @param x x coordinate of the item
     * @param y y coordinate of the item
     */
    public TransportableItem(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);

	type = TRANSPORTABLE_ITEM_POT;
	pickableItemType = PickableItem.PICKABLE_ITEM_RANDOM;
	pickableItemSavegameIndex = -1;
    }

    /**
     * Creates an existing transportable item from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public TransportableItem(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);
	setSizeImpl(16, 16);

	// parse the fields
	try {
	    this.type = Integer.parseInt(tokenizer.nextToken());
	    this.pickableItemType = Integer.parseInt(tokenizer.nextToken());
	    this.pickableItemSavegameIndex = Integer.parseInt(tokenizer.nextToken());
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
    }

    /**
     * Returns a string describing this transportable item.
     * @return a string representation of the transportable item
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// add the specific properties of a transportable item
	buff.append('\t');
	buff.append(getTransportableItemType());
	buff.append('\t');
	buff.append(getPickableItemType());
	buff.append('\t');
	buff.append(getPickableItemSavegameIndex());

	return buff.toString();
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_TRANSPORTABLE_ITEM
     */
    public int getType() {
	return ENTITY_TRANSPORTABLE_ITEM;
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (8,13)
     */
    protected Point getOrigin() {
	return origin;
    }

    /**
     * Returns the entity's obstacle property.
     * @return OBSTACLE
     */
    public int getObstacle() {
	return OBSTACLE;
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	int x = getTransportableItemType() * 16;
	currentImageDescription.setX(x);
    }

    /**
     * Returns the type of this transportable item.
     * @return the type of transportable item
     */
    public int getTransportableItemType() {
	return type;
    }

    /**
     * Sets the type of this transportable item.
     * @param type the type of transportable item
     */
    public void setTransportableItemType(int type) {
	this.type = type;
    }
    
    /**
     * Returns the type of pickable item that appears when Link
     * lifts the transportable item.
     * @return the type of pickable item
     */
    public int getPickableItemType() {
	return pickableItemType;
    }

    /**
     * Returns the index where the pickable item attached to
     * this transportable item is saved (if any).
     * @return the savegame index of the pickable item
     */
    public int getPickableItemSavegameIndex() {
	return pickableItemSavegameIndex;
    }
    
    /**
     * Sets the pickable item that appears when Link
     * lifts the transportable item.
     * @param type the type of pickable item
     * @param savegameIndex savegame index where the pickable item is saved,
     * @throws ZSDXException if the value of savegameIndex doesn't correspond
     * to the specified pickable item type
     */
    public void setPickableItem(int type, int savegameIndex) throws ZSDXException {

	this.pickableItemType = type;
	this.pickableItemSavegameIndex = savegameIndex;
    }
}
