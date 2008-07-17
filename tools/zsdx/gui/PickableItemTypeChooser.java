package zsdx.gui;

import zsdx.*;
import javax.swing.*;

/**
 * A combo box to select a type of pickable item.
 */
public class PickableItemTypeChooser extends JComboBox {
    
    private static final String[] names = {
	"1 Rupee", "5 Rupees", "20 Rupees",
	"Heart", "Magic flask", "Magic bottle", "Fairy",
	"1 Bomb", "5 Bombs", "10 Bombs", "1 Arrow", "5 Arrows", "10 Arrows"
    };

    /**
     * Creates a new pickable item type chooser.
     * @param allowNone true to include "None" in the combo box
     */
    public PickableItemTypeChooser(boolean allowNone) {
	super();
	
	// random
	addItem(new KeyValue(PickableItem.PICKABLE_ITEM_RANDOM, "Random"));
	
	// none
	if (allowNone) {
	    addItem(new KeyValue(PickableItem.PICKABLE_ITEM_NONE, "None"));
	    setPickableItemType(PickableItem.PICKABLE_ITEM_NONE);
	}

	// normal items
	for (int i = 0; i < names.length; i++) {
	    addItem(new KeyValue(i + 1, names[i]));
	}
    }
    
    /**
     * Returns the type of pickable item currently selected.
     * @return the type of pickable item currently selected
     */
    public int getPickableItemType() {

	KeyValue item = (KeyValue) getSelectedItem();
	return Integer.parseInt(item.getKey());
    }
    
    /**
     * Sets the type of pickable item selected.
     * @param type the type of pickable item to make select
     */
    public void setPickableItemType(int type) {

	KeyValue item = new KeyValue(type, null);
	setSelectedItem(item);
    }
}
