package zsdx;

import java.awt.*;
import java.util.*;

/**
 * Represents an entity that Link can lift, carry and throw:
 * a pot, a bush, etc.
 */
public class TransportableItem extends InteractiveEntity {

    // types of pickable items

    public static final int TRANSPORTABLE_ITEM_POT               = 0;
    public static final int TRANSPORTABLE_ITEM_SKULL             = 1;
    public static final int TRANSPORTABLE_ITEM_BUSH              = 2;
    public static final int TRANSPORTABLE_ITEM_STONE_WHITE_SMALL = 3;
    public static final int TRANSPORTABLE_ITEM_STONE_WHITE_BIG   = 4;
    public static final int TRANSPORTABLE_ITEM_STONE_BLACK_SMALL = 5;
    public static final int TRANSPORTABLE_ITEM_STONE_BLACK_BIG   = 6;

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
     * A number identifying the pickable item, used only for the pickable
     * items that Link can obtain only once (keys, pieces of hearts, etc.).
     */
    private int uniqueId;

    /**
     * Creates a new transportable item at the specified location.
     * @param map the map
     * @param x x coordinate of the item
     * @param y y coordinate of the item
     */
    public TransportableItem(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);

	type = TRANSPORTABLE_ITEM_POT;
	pickableItemType = PickableItem.PICKABLE_ITEM_RANDOM;
	uniqueId = 0;
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
	    this.uniqueId = Integer.parseInt(tokenizer.nextToken());
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

	// TODO add the specific properties of a transportable item
	buff.append('\t');
	buff.append(getTransportableItemType());
	buff.append('\t');
	buff.append(getPickableItemType());
	buff.append('\t');
	buff.append(getUniqueId());

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
     * Returns the number of directions of the entity.
     * @return 0 (a transportable item has no direction)
     */
    public int getNbDirections() {
	return 0;
    }
    
    /**
     * Returns whether the entity has an identifier.
     * @return false
     */
    public boolean hasName() {
	return false;
    }

    /**
     * Returns whether or not the entity is resizable.
     * A transportable item is not resizable.
     * @return false
     */
    public boolean isResizable() {
	return false;
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
	return type;
    }

    /**
     * Sets the type of this pickable item that appears when Link
     * lifts the transportable item.
     * @param type the type of pickable item
     */
    public void setPickableItemType(int type) {
	this.type = type;
    }
    
    /**
     * Returns the id identifying the pickable item attached to
     * this transportable item (if any).
     * @return the unique id of this pickable item, or zero if
     * this pickable item is not unique.
     */
    public int getUniqueId() {
	return uniqueId;
    }
    
    /**
     * Sets the id identifying the pickable item attached to
     * this transportable item (if any).
     * @param uniqueId the unique id of this pickable item, or zero if
     * this pickable item is not unique.
     */
    public void setUniqueId(int uniqueId) {
	this.uniqueId = uniqueId;
    }
    
    /**
     * Draws the transportable item on the map editor.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {
	// TODO
    }

}
