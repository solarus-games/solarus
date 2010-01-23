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
package org.solarus.editor.gui;

import javax.swing.*;
import org.solarus.editor.entities.InteractiveEntity.Subtype;

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
