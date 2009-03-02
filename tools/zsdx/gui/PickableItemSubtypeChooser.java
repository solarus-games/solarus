package zsdx.gui;

import zsdx.entities.PickableItem.Subtype;

/**
 * A combo box to select a type of pickable item.
 */
public class PickableItemSubtypeChooser extends EnumerationChooser<Subtype> {
    
    /**
     * Creates a new pickable item type chooser.
     * @param allowRandomAndNone true to include "Random" and "None" in the combo box
     */
    public PickableItemSubtypeChooser(boolean allowRandomAndNone) {
	super(Subtype.class);

	// special values: random and none
	if (!allowRandomAndNone) {
	    removeItemAt(0);
	    removeItemAt(0);
	}
    }
}
