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

/**
 * A combo box to choose the direction of something.
 */
public class DirectionChooser extends JComboBox {
    
    /**
     * Name of the 4 directions, for the items in the sub menu.
     */
    public static final String[] directionNames4 = {"Right", "Up", "Left", "Down"};

    /**
     * Name of the 8 directions, for the items in the sub menu.
     */
    public static final String[] directionNames8 = {"Right", "Right-Up", "Up", "Left-Up",
		"Left", "Left-Down", "Down", "Right-Down"};

    /**
     * Creates a new direction chooser
     * @param nbDirections the number of directions (4 or 8)
     * @param noDirectionText if not null, this will include an item corresponding
     * to 'no direction' with the specified text
     */
    public DirectionChooser(int nbDirections, String noDirectionText) {
	super();

	if (noDirectionText != null) {
	    addItem(new KeyValue(-1, noDirectionText));
	}

	if (nbDirections == 4) {
	    for (int i = 0; i < 4; i++) {
		addItem(new KeyValue(i, directionNames4[i]));
	    }
	}
	else {
	    for (int i = 0; i < 8; i++) {
		addItem(new KeyValue(i, directionNames8[i]));
	    }
	}
    }

    /**
     * Selects a direction in the list.
     * @param direction the direction to make selected
     */
    public void setDirection(int direction) {

	KeyValue item = new KeyValue(Integer.toString(direction), null);
	setSelectedItem(item);
    }

    /**
     * Returns the currently selected direction.
     * @return the selected direction
     */
    public int getDirection() {

	KeyValue item = (KeyValue) getSelectedItem();
	return Integer.parseInt(item.getKey());
    }
}
