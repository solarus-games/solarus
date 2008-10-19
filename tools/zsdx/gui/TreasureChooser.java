package zsdx.gui;

import javax.swing.*;
import zsdx.entities.*;

/**
 * A combo box to selected the content of a treasure.
 */
public class TreasureChooser extends JComboBox {

    /**
     * Creates a new layer chooser.
     */
    public TreasureChooser() {
	super();

	for (TreasureContent content: TreasureContent.values()) {	    
	    addItem(new KeyValue(content.getIndex(), content.name()));
	}
    }
    
    /**
     * Returns the currently selected treasure content.
     * @return the selected treasure content
     */
    public TreasureContent getContent() {

	KeyValue item = (KeyValue) getSelectedItem();
	return TreasureContent.get(Integer.parseInt(item.getKey()));
    }
    
    /**
     * Selects a treasure content in the list.
     * @param content the treasure content to make selected
     */
    public void setContent(TreasureContent content) {

	KeyValue item = new KeyValue(content.getIndex(), null);
	setSelectedItem(item);
    }    
}
