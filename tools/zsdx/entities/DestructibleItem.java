package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * Represents an entity that Link can destroy (lift and throw or cut)
 * and that can hide a pickable item.
 */
public class DestructibleItem extends DynamicEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("destructible_items.png", 0, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 16, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 32, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 48, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 64, 0, 16, 16),
	new EntityImageDescription("destructible_items.png", 80, 0, 16, 16)
    };

    /**
     * Origin point of a destructible item.
     */
    private static final Point origin = new Point(8, 13);

    /**
     * Subtypes of destructible items.
     */
    public enum Subtype implements EntitySubtype {
	POT,
	SKULL,
	BUSH,
	STONE_SMALL_WHITE,
	STONE_SMALL_BLACK,
	GRASS;

	public static String[] names = {
	    "Pot",
	    "Skull",
	    "Bush",
	    "Small white stone",
	    "Small black stone",
	    "Grass"
	};

	public int getId() {
	    return ordinal();
	}

	public static Subtype get(int id) {
	    return values()[id];
	}
    };

    // specific fields of a destructible item

    /**
     * Type of pickable item that appears when Link lifts
     * the destructible item.
     */
    private PickableItem.Subtype pickableItemSubtype;

    /**
     * The index where the pickable item is saved, used only for the pickable
     * items that are saved (keys, pieces of hearts, etc.).
     */
    private int pickableItemSavegameVariable;

    /**
     * Creates a new destructible item at the specified location.
     * By default, the subtype is a pot and the pickable item is random. 
     * @param map the map
     * @param x x coordinate of the item
     * @param y y coordinate of the item
     */
    public DestructibleItem(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);

	subtype = Subtype.POT;
	pickableItemSubtype = PickableItem.Subtype.RANDOM;
	pickableItemSavegameVariable = -1;
    }

    /**
     * Creates an existing destructible item from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the subtype of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public DestructibleItem(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);
	setSizeImpl(16, 16);

	// parse the fields
	try {
	    this.pickableItemSubtype = PickableItem.Subtype.get(Integer.parseInt(tokenizer.nextToken()));
	    this.pickableItemSavegameVariable = Integer.parseInt(tokenizer.nextToken());
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Integer expected");
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("A value is missing");
	}
    }

    /**
     * Returns a string describing this destructible item.
     * @return a string representation of the destructible item
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// add the specific properties of a destructible item
	buff.append('\t');
	buff.append(getPickableItemSubtype().getId());
	buff.append('\t');
	buff.append(getPickableItemSavegameVariable());

	return buff.toString();
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
	int x = getSubtypeId() * 16;
	currentImageDescription.setX(x);
    }

    /**
     * Returns the subtype of pickable item that appears when Link
     * lifts the destructible item.
     * @return the subtype of pickable item
     */
    public PickableItem.Subtype getPickableItemSubtype() {
	return pickableItemSubtype;
    }

    /**
     * Returns the index where the pickable item attached to
     * this destructible item is saved (if any).
     * @return the savegame index of the pickable item
     */
    public int getPickableItemSavegameVariable() {
	return pickableItemSavegameVariable;
    }
    
    /**
     * Sets the pickable item that appears when Link
     * lifts the destructible item.
     * @param subtype the subtype of pickable item
     * @param savegameVariable savegame index where the pickable item is saved
     */
    public void setPickableItem(PickableItem.Subtype pickableItemSubtype, int savegameVariable) {

	this.pickableItemSubtype = pickableItemSubtype;
	this.pickableItemSavegameVariable = savegameVariable;
	setChanged();
	notifyObservers();
    }
}
