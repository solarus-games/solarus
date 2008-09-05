package zsdx;

import java.awt.*;
import javax.swing.*;
import java.util.*;

/**
 * Represents a chest placed on the map.
 */
public class Chest extends InteractiveEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Chest";

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription generalImageDescription =
	new EntityImageDescription("chest.png", 0, 0, 16, 16);

    // data fields
    private boolean bigChest;
    private TreasureContent content;
    private int amount;
    private int savegameIndex;
    
    /**
     * Creates a new chest at the specified location with some default properties.
     * @param map the map
     * @param x x coordinate of the chest
     * @param y y coordinate of the chest
     */
    public Chest(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);
	
	// default field values
	bigChest = false;
	content = TreasureContent.NOTHING;
	amount = 1;
	savegameIndex = -1;
    }

    /**
     * Creates an existing chest from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public Chest(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);
	
	try {
	    this.bigChest = (Integer.parseInt(tokenizer.nextToken()) != 0);
	    this.content = TreasureContent.get(Integer.parseInt(tokenizer.nextToken()));
	    this.amount = Integer.parseInt(tokenizer.nextToken());
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
     * Returns a string describing this chest.
     * @return a string representation of the chest
     */
    public String toString() {

	StringBuffer buff = new StringBuffer();

	// get the common part of the string
	buff.append(super.toString());

	// add the specific properties of a chest
	buff.append('\t');
	buff.append(isBigChest() ? 1 : 0);
	buff.append('\t');
	buff.append(getContent().getIndex());
	buff.append('\t');
	buff.append(getAmount());
	buff.append('\t');
	buff.append(getSavegameIndex());

	return buff.toString();
    }

    /**
     * Returns whether this a big chest or a normal chest.
     * @return true if this is a big chest
     */
    public boolean isBigChest() {
        return bigChest;
    }

    /**
     * Sets whether this a big chest or a normal chest.
     * @param bigChest true to make a big chest
     */
    public void setBigChest(boolean bigChest) {
        this.bigChest = bigChest;
    }

    /**
     * Returns the content of this chest.
     * @return content the content
     */
    public TreasureContent getContent() {
        return content;
    }

    /**
     * Sets the content of this chest.
     * @param content the new content
     */
    public void setContent(TreasureContent content) {
        this.content = content;
    }
    
    /**
     * Returns the amount of the treasure (only for some kinds of contents).
     * @return amount the amount
     */
    public int getAmount() {
        return amount;
    }

    /**
     * Sets the amount of the treasure (only for some kinds of contents).
     * @param amount the new amount
     */
    public void setAmount(int amount) {
        this.amount = amount;
    }

    /**
     * Returns the index of the boolean value for this chest.
     * @return the index of this chest's boolean value in the savegame
     */
    public int getSavegameIndex() {
        return savegameIndex;
    }

    /**
     * Sets the index of the boolean value for this chest.
     * @param savegameIndex the index of this chest's boolean value in the savegame
     */
    public void setSavegameIndex(int savegameIndex) {
        this.savegameIndex = savegameIndex;
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_CHEST
     */
    public int getType() {
	return ENTITY_CHEST;
    }
    
    /**
     * Returns whether this entity is an obstacle.
     * @return true
     */
    public int getObstacle() {
	return OBSTACLE;
    }

    /**
     * Checks the entity validity. An entity must be valid before it is saved.
     * @return true if the entity is valid
     */
    public boolean isValid() {
	return super.isValid()
	&& amount >= 1 && savegameIndex >= 0 && savegameIndex < 32768;
    }

    /**
     * Updates the description of the image currently representing the entity.
     */
    public void updateImageDescription() {
	if (isBigChest()) {
	    currentImageDescription.setRectangle(0, 16, 32, 24);
	}
	else {
	    currentImageDescription.setRectangle(0, 0, 16, 16);
	}
    }

}
