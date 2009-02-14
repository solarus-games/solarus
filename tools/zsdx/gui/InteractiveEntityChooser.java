package zsdx.gui;

import javax.swing.*;
import zsdx.entities.InteractiveEntity.Subtype;

/**
 * A combo box to select a type of interactive entity.
 */
public class InteractiveEntityChooser extends JComboBox {

    private static final String[] names = {
	"Custom", "Non-playing character", "Sign", "Fill bottle with water"
    };

    /**
     * Creates a new interactive entity chooser.
     */
    public InteractiveEntityChooser() {
	super();

	for (int i = 0; i < names.length; i++) {
	    addItem(new KeyValue(Subtype.values()[i].name(), names[i]));
	}
    }

    /**
     * Returns the subtype of interactive entity currently selected.
     * @return the subtype of interactive entity currently selected
     */
    public Subtype getInteractiveEntitySubtype() {

	KeyValue item = (KeyValue) getSelectedItem();
	return Subtype.valueOf(item.getKey());
    }

    /**
     * Sets the subtype of interactive entity selected.
     * @param type the subtype of interactive entity to make selected
     */
    public void setInteractiveEntitySubtype(Subtype subtype) {

	KeyValue item = new KeyValue(subtype.name(), null);
	setSelectedItem(item);
    }
}
