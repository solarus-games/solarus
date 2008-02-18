package zsdx.gui;

import javax.swing.*;
import zsdx.*;

/**
 * A combo box to selected a layer.
 */
public class LayerChooser extends JComboBox {

    /**
     * Creates a new layer chooser.
     */
    public LayerChooser() {
	super();
	addItem(new KeyValue(Integer.toString(MapEntity.LAYER_LOW), "Low"));
	addItem(new KeyValue(Integer.toString(MapEntity.LAYER_INTERMEDIATE), "Intermediate"));
	addItem(new KeyValue(Integer.toString(MapEntity.LAYER_HIGH), "High"));
    }
    
    /**
     * Selects a layer in the list.
     * @param layer the layer to make selected
     */
    public void setLayer(int layer) {

	KeyValue item = new KeyValue(Integer.toString(layer), null);
	setSelectedItem(item);
    }
    
    /**
     * Returns the currently selected layer.
     * @return the selected layer
     */
    public int getLayer() {
	
	KeyValue item = (KeyValue) getSelectedItem();
	return Integer.parseInt(item.getKey());
    }
}
