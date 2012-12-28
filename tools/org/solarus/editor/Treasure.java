package org.solarus.editor;

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

    private String itemName;            /**< id of the item to give, according to items.dat
                                         * (possibly "_none" or "_random") or null if unspecified. */
    private Integer variant;            /**< variant of this item (null means unspecified). */
    private String savegameVariable;   /**< boolean variable that stores the
                                         * treasure's state (found or not found) in the savegame
                                         * or null for unsaved. */

    /**
     * Creates a treasure.
     * @param itemName Name identifying the treasure to give
     * (possibly "_none" or "_random") or null if unspecified.
     * @param variant Variant of this item (null means unspecified).
     * @param savegameVariable Savegame variable that stores the treasure's state,
     * or null to make the treasure unsaved (null means unsaved).
     */
    public Treasure(String itemName, Integer variant, String savegameVariable) {

        this.itemName = itemName;
        this.variant = variant;
        this.savegameVariable = savegameVariable;
    }

    /**
     * Returns the id of the item to give.
     * @return The id of the item (possibly "_none" or "_random") or null.
     */
    public String getItemName() {
        return itemName;
    }

    /**
     * Sets the id of the item to give.
     * @param itemName The name to set (possibly "_none")
     * or null for unspecified.
     */
    public void setItemName(String itemName) {
        this.itemName = itemName;

        if (itemName.equals(Item.noneId)) {
            variant = null;
        }
        else if (variant == null) {
            variant = 1;
        }
    }

    /**
     * Returns the variant of the item to give.
     * @return The variant (null means unspecified).
     */
    public Integer getVariant() {
        return variant;
    }

    /**
     * Sets the variant of the item to give.
     * @param variant The variant to set (null means unspecified).
     */
    public void setVariant(Integer variant) {
        this.variant = variant;
    }

    /**
     * Returns the savegame variable that stores the treasure's state.
     * @return The savegame variable (null means unsaved).
     */
    public String getSavegameVariable() {
        return savegameVariable;
    }

    /**
     * Sets the savegame variable that stores the treasure's state.
     * @param savegameVariable The savegame variable to set, or -1
     * to make the treasure unsaved (null means unsaved).
     */
    public void setSavegameVariable(String savegameVariable) {
        this.savegameVariable = savegameVariable;
    }
}

