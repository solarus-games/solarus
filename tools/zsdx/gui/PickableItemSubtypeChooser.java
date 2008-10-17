package zsdx.gui;

import zsdx.*;
import javax.swing.*;

/**
 * A combo box to select a type of pickable item.
 */
public class PickableItemSubtypeChooser extends JComboBox {
    
    private static final String[] names = {
	"1 Rupee", "5 Rupees", "20 Rupees",
	"Heart", "Magic flask", "Magic bottle", "Fairy",
	"1 Bomb", "5 Bombs", "10 Bombs", "1 Arrow", "5 Arrows", "10 Arrows",
	"Small Key", "Big Key", "Boss Key", "Piece of Heart", "Heart Container"
    };

    /**
     * Creates a new pickable item type chooser.
     * @param allowRandomAndNone true to include "Random" and "None" in the combo box
     */
    public PickableItemSubtypeChooser(boolean allowRandomAndNone) {
	super();

	// special values: random and none
	if (allowRandomAndNone) {
	    addItem(new KeyValue(PickableItem.PICKABLE_ITEM_RANDOM, "Random"));
	    addItem(new KeyValue(PickableItem.PICKABLE_ITEM_NONE, "None"));
	    setPickableItemSubtype(PickableItem.PICKABLE_ITEM_NONE);
	}

	// normal value
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
    public void setPickableItemSubtype(int type) {

	KeyValue item = new KeyValue(type, null);
	setSelectedItem(item);
    }
}
