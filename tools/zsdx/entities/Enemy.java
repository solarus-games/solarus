package zsdx.entities;

import java.awt.*;
import java.util.*;
import zsdx.*;
import zsdx.Map;

/**
 * Represents an enemy placed on a map.
 */
public class Enemy extends DynamicEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = {
	new EntityImageDescription("enemies.png", 0, 4, 16, 16)
    };

    /**
     * Description of the image representing currently the entity.
     */
    public static final EntityImageDescription[] currentImageDescriptions = {
	new EntityImageDescription("enemies.png", 0, 0, 16, 32)
    };

    /**
     * Origin point of each type of enemy.
     */
    private static final Point[] origins = {
	new Point(8, 29)
    };

    /**
     * Size of each type of enemy.
     */
    private static final Dimension[] sizes = {
	new Dimension(16, 32)
    };

    /**
     * Subtypes of enemies.
     */
    public enum Subtype implements EntitySubtype {
	SIMPLE_GREEN_SOLDIER;

	public static final String[] humanNames = {
	    "Simple green soldier"
	};

	public int getId() {
	    return ordinal();
	}

	public static Subtype get(int id) {
	    return values()[id];
	}
    }

    /**
     * Enemy ranks.
     */
    public enum Rank {
	NORMAL,
	MINIBOSS,
	BOSS;

	public static final String[] humanNames = {"Normal", "Miniboss", "Boss"};
    }

    /**
     * Rank of the enemy: normal, miniboss or boss
     */
    private Rank rank;

    /**
     * The variable where the enemy is saved, used only for enemies
     * that are saved.
     */
    private int savegameVariable;

    /**
     * Type of pickable item that appears when the enemy is killed.
     */
    private PickableItem.Subtype pickableItemSubtype;

    /**
     * The variable where the pickable item is saved, used only for the pickable
     * items that are saved (keys, pieces of hearts, etc.).
     */
    private int pickableItemSavegameVariable;

    /**
     * Creates a new enemy at the specified location.
     * By default, the subtype is a simple green soldier and the pickable item is random. 
     * @param map the map
     * @param x x coordinate of the entity
     * @param y y coordinate of the entity
     */
    public Enemy(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);

	subtype = Subtype.SIMPLE_GREEN_SOLDIER;
	rank = Rank.NORMAL;
	savegameVariable = -1;
	pickableItemSubtype = PickableItem.Subtype.RANDOM;
	pickableItemSavegameVariable = -1;

	setDirection(3);
	Dimension size = sizes[getSubtypeId()];
	setSizeImpl(size.width, size.height);
    }

    /**
     * Creates an existing enemy from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the subtype of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public Enemy(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);

	// parse the fields
	try {
	    this.rank = Rank.values()[Integer.parseInt(tokenizer.nextToken())];
	    this.savegameVariable = Integer.parseInt(tokenizer.nextToken());
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
     * Returns a string describing this enemy.
     * @return a string representation of the enemy
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// add the specific properties of an enemy
	buff.append('\t');
	buff.append(getRank().ordinal());
	buff.append('\t');
	buff.append(getSavegameVariable());
	buff.append('\t');
	buff.append(getPickableItemSubtype().getId());
	buff.append('\t');
	buff.append(getPickableItemSavegameVariable());

	return buff.toString();
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return the coordinates of the origin point of the entity
     */
    protected Point getOrigin() {
	return origins[getSubtypeId()];
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
	return 4;
    }

    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }

    /**
     * Returns the subtype of this entity.
     * @return the subtype
     */
    public Subtype getSubtype() {
	return (Subtype) super.getSubtype();
    }

    /**
     * Sets the subtype of this enemy.
     * @param subtype the subtype of enemy
     */
    public void setSubtype(Subtype subtype) {
	super.setSubtype(subtype);

	Dimension size = sizes[getSubtypeId()];
	setSizeImpl(size.width, size.height);

	setChanged();
	notifyObservers();
    }

    /**
     * Returns the rank of this enemy.
     * @return the rank
     */
    public Rank getRank() {
	return rank;
    }

    /**
     * Sets the rank of this enemy.
     * @param rank the rank
     */
    public void setRank(Rank rank) {
	this.rank = rank;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the variable where the enemy is saved (if any)
     * @return the savegame variable of the enemy
     */
    public int getSavegameVariable() {
	return savegameVariable;
    }

    /**
     * Returns the variable where the enemy is saved (if any)
     * @param savegameVariable the savegame variable of the enemy
     */
    public void setSavegameVariable(int savegameVariable) {
	this.savegameVariable = savegameVariable;
	setChanged();
	notifyObservers();
    }

    /**
     * Returns the subtype of pickable item that appears when
     * the enemy is killed
     * @return the subtype of pickable item
     */
    public PickableItem.Subtype getPickableItemSubtype() {
	return pickableItemSubtype;
    }

    /**
     * Returns the variable where the pickable item attached to
     * this enemy is saved (if any).
     * @return the savegame variable of the pickable item
     */
    public int getPickableItemSavegameVariable() {
	return pickableItemSavegameVariable;
    }
    
    /**
     * Sets the pickable item that appears when the enemy is killed.
     * @param subtype the subtype of pickable item
     * @param savegameVariable savegame variable where the pickable item is saved
     */
    public void setPickableItem(PickableItem.Subtype type, int savegameVariable) {

	this.pickableItemSubtype = type;
	this.pickableItemSavegameVariable = savegameVariable;
	setChanged();
	notifyObservers();
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	currentImageDescription = currentImageDescriptions[getSubtypeId()];
    }

    /**
     * Checks the entity validity. An entity must be valid before it is saved.
     * @return true if the entity is valid
     */
    public boolean isValid() {
	return super.isValid() && (rank == Rank.NORMAL || savegameVariable >= 0);
    }

}
