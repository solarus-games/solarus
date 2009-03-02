package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * Represents an item that Link can pick on the map,
 * such as a rupee, a heart, a key, etc.
 */
public class PickableItem extends DynamicEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Pickable item";

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions =
	{new EntityImageDescription("pickable_items.png", 0, 0, 16, 16)};
    
    /**
     * Origin point of a pickable item.
     */
    private static final Point origin = new Point(8, 13);
    
    /**
     * Subtypes of pickable items.
     */
    public enum Subtype {
	RANDOM          (-1), /**< special value to indicate to choose another value
	                       * randomly (including PICKABLE_ITEM_NONE) */
	NONE            (0),  /**< special value to incicate that there is no pickable item */
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

	public static final String[] names = {
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
    }

    // specific fields of a pickable item

    /**
     * Type of pickable item.
     * It cannot be PICKABLE_ITEM_RANDOM or PICKABLE_ITEM_NONE: these
     * values are reserved for items obtained by destroying pots, bush, enemies, etc.
     */
    private Subtype subtype;
    
    /**
     * A number indicating the savegame variable where this pickable item possession
     * state is saved.
     * It is used only for the pickable that Link can obtain only once and
     * thus that are saved (keys, pieces of hearts, etc.) and ignored for the normal items.
     */
    private int savegameVariable;

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
	subtype = Subtype.RUPEE_1;
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
	    this.subtype = Subtype.get(Integer.parseInt(tokenizer.nextToken()));
	    this.savegameVariable = Integer.parseInt(tokenizer.nextToken());
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
	buff.append(getSubtypeIndex());
	buff.append('\t');
	buff.append(getSavegameVariable());

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
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	int x = (getSubtypeIndex() - 1) * 16;
	currentImageDescription.setX(x);
    }

    /**
     * Returns the type of this pickable item.
     * @return the type of pickable item
     */
    public Subtype getSubtype() {
	return subtype;
    }

    /**
     * Returns the type index of this pickable item.
     * @return the type index of pickable item
     */
    public int getSubtypeIndex() {
	return subtype.getId();
    }

    /**
     * Sets the properties of this pickable item.
     * @param subtype the subtype of pickable item
     * @param savegameVariable the savegame variable of this pickable item, or zero if
     * this pickable item is not saved.
     * @throws ZSDXException if the type is incorrect or if the value of
     * savegameVariable doesn't correspond to the specified type
     */
    public void setPickableItem(Subtype subtype, int savegameVariable) throws ZSDXException {
	
	if (subtype == Subtype.NONE || subtype == Subtype.RANDOM) {
	    throw new ZSDXException("The type of pickable item cannot be 'None' or 'Random'");
	}
	
	this.subtype = subtype;
	this.savegameVariable = savegameVariable;
    }

    /**
     * Returns the savegame variable identifying this pickable item (if any).
     * @return the savegame variable where this pickable item is saved,
     * or any value if this pickable item cannot be saved
     */
    public int getSavegameVariable() {
	return savegameVariable;
    }

    /**
     * Sets the variable where this pickable item is saved (if any).
     * @param savegameIndex the savegame variable where this pickable item is saved
     */
    public void setSavegameVariable(int savegameVariable) {
	this.savegameVariable = savegameVariable;
    }
}
