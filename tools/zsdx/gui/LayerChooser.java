/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package zsdx.gui;

import javax.swing.*;
import zsdx.entities.MapEntity;

/**
 * A combo box to selected a layer.
 */
public class LayerChooser extends JComboBox {

    /**
     * Creates a new layer chooser.
     */
    public LayerChooser() {
	super();
	addItem(new KeyValue(MapEntity.LAYER_LOW, "Low"));
	addItem(new KeyValue(MapEntity.LAYER_INTERMEDIATE, "Intermediate"));
	addItem(new KeyValue(MapEntity.LAYER_HIGH, "High"));
    }
    
    /**
     * Returns the currently selected layer.
     * @return the selected layer
     */
    public int getLayer() {
	
	KeyValue item = (KeyValue) getSelectedItem();
	return Integer.parseInt(item.getKey());
    }
    
    /**
     * Selects a layer in the list.
     * @param layer the layer to make selected
     */
    public void setLayer(int layer) {

	KeyValue item = new KeyValue(Integer.toString(layer), null);
	setSelectedItem(item);
    }    
}
