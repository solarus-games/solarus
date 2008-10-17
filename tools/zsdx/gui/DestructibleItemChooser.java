package zsdx.gui;

import javax.swing.*;

/**
 * A combo box to select a type of destructible item.
 */
public class DestructibleItemChooser extends JComboBox {

    private static final String[] names = {
	"Pot", "Skull", "Bush",
	"Small white stone", "Small black stone", "Grass"
    };

    /**
     * Creates a new destructible item type chooser.
     */
    public DestructibleItemChooser() {
	super();

	for (int i = 0; i < names.length; i++) {
	    addItem(new KeyValue(i, names[i]));
	}
    }

    /**
     * Returns the type of destructible item currently selected.
     * @return the type of destructible item currently selected
     */
    public int getDestructibleItemSubtype() {

	KeyValue item = (KeyValue) getSelectedItem();
	return Integer.parseInt(item.getKey());
    }

    /**
     * Sets the type of destructible item selected.
     * @param type the type of destructible item to make select
     */
    public void setDestructibleItemSubtype(int subtype) {

	KeyValue item = new KeyValue(subtype, null);
	setSelectedItem(item);
    }
}
