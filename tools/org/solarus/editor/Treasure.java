package org.solarus.editor;

import java.util.StringTokenizer;

/**
 * Represents a treasure that the player may receive.
 * A treasure may come from a chest, a shop item, an enemy, a destructible item,
 * a pickable item or a script. 
 * It is composed of the name identifying the item to give, the variant of this item
 * (as an item can have several variants)
 * and an optional savegame variable.
 * The name and the variant should respect the definition of the items
 * in the file items.dat.
 */
public class Treasure {

    private String itemName;		/**< id of the item to give, according to items.dat
					 * (possibly "_none" or "_random") */
    private int variant;		/**< variant of this item */
    private int savegameVariable;	/**< index of the boolean variable that stores the
  					 * treasure's state (found or not found) in the savegame
  					 * (-1 if it is not saved) */

    /**
     * Creates a treasure.
     * @param itemName name identifying the treasure to give (possibly "_none" or "_random")
     * @param variant variant of this item
     * @param savegameVariable savegame variable that stores the treasure's state,
     * or -1 to make the treasure unsaved
     */
    public Treasure(String itemName, int variant, int savegameVariable) {

	this.itemName = itemName;
	this.variant = variant;
	this.savegameVariable = savegameVariable;
    }

    /**
     * Parses a treasure from a string.
     * @param description a string containing the item name, the variant and the savegame
     * variable in that order (separated by whitespaces or tabulations)
     */
    public Treasure(String description) {
	
	StringTokenizer tokenizer = new StringTokenizer(description);
	itemName = tokenizer.nextToken();
	variant = Integer.parseInt(tokenizer.nextToken());
	savegameVariable = Integer.parseInt(tokenizer.nextToken());
    }

    /**
     * Returns a string representation of this treasure.
     * The string has the same format as the parameter of the constructor.
     * @return a string describing this treasure 
     */
    public String toString() {
	return itemName + "\t" + variant + "\t" + savegameVariable;
    }

    /**
     * Returns the id of the item to give.
     * @return the id of the item (possibly "_none" or "_random")
     */
    public String getItemName() {
        return itemName;
    }

    /**
     * Sets the id of the item to give.
     * @param itemName the name to set (possibly "_none" or "_random")
     */
    public void setItemName(String itemName) {
        this.itemName = itemName;
    }

    /**
     * Returns the variant of the item to give.
     * @return the variant
     */
    public int getVariant() {
        return variant;
    }

    /**
     * Sets the variant of the item to give.
     * @param variant the variant to set
     */
    public void setVariant(int variant) {
        this.variant = variant;
    }

    /**
     * Returns the savegame variable that stores the treasure's state.
     * @return the savegame variable, or -1 if the treasure is not saved
     */
    public int getSavegameVariable() {
        return savegameVariable;
    }

    /**
     * Sets the savegame variable that stores the treasure's state.
     * @param savegameVariable the savegame variable to set, or -1
     * to make the treasure unsaved
     */
    public void setSavegameVariable(int savegameVariable) {
        this.savegameVariable = savegameVariable;
    }
}
