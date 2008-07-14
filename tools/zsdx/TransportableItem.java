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
    
    /**
     * Creates a new transportable item at the specified location.
     * @param map the map
     * @param x x coordinate of the item
     * @param y y coordinate of the item
     */
    public TransportableItem(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);

	// TODO default field values
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

	// TODO parse the fields
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
