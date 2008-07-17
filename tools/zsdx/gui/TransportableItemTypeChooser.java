package zsdx.gui;

import javax.swing.*;

/**
 * A combo box to select a type of transportable item.
 */
public class TransportableItemTypeChooser extends JComboBox {

    private static final String[] names = {
	"Pot", "Skull", "Bush",
	"White stone (small)", "White stone (big)",
	"Black stone (small)", "Black stone (big)"
    };

    /**
     * Creates a new transportable item type chooser.
     */
    public TransportableItemTypeChooser() {
	super();

	for (int i = 0; i < names.length; i++) {
	    addItem(new KeyValue(i - 1, names[i]));
	}
    }
    
    /**
     * Returns the type of transportable item currently selected.
     * @return the type of transportable item currently selected
     */
    public int getTransportableItemType() {

	KeyValue item = (KeyValue) getSelectedItem();
	return Integer.parseInt(item.getKey());
    }
    
    /**
     * Sets the type of transportable item selected.
     * @param type the type of transportable item to make select
     */
    public void setTransportableItemType(int type) {

	KeyValue item = (KeyValue) getSelectedItem();
	setSelectedItem(item);
    }
}
