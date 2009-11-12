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

import java.awt.event.*;
import javax.swing.*;

/**
 * A component with a group of radio options.
 */
public class RadioField extends JPanel {
    
    private JRadioButton[] buttons;

    /**
     * Creates a radio field with the specified radio options.
     * The first one is initially made selected.
     * @param options the name of the options
     */
    public RadioField(String ... options) {
	super();

	this.buttons = new JRadioButton[options.length];

	ButtonGroup buttonGroup = new ButtonGroup();
	int i = 0;

	for (String option: options) {
	    JRadioButton button = new JRadioButton(option);

	    add(button);
	    buttonGroup.add(button);

	    if (i == 0) {
		button.setSelected(true);
	    }

	    buttons[i] = button;
	    i++;
	}
    }

    /**
     * Returns the index of the radio button currently selected,
     * or -1 if no button is selected
     * @return the index of the radio button currently selected
     */
    public int getSelectedIndex() {

	int i;
	boolean found = false;

	for (i = 0; i < buttons.length && !found; i++) {
	    if (buttons[i].isSelected()) {
		found = true;
	    }
	}

	if (found) {
	    return i - 1;
	}
	return -1;
    }

    /**
     * Select one of the radio buttons.
     * @param index the index of the radio button to make selected
     */
    public void setSelectedIndex(int index) {
	this.buttons[index].setSelected(true);
    }

    /*
     * Unselects all the radio buttons.
     * (does not seem to work)
    public void unselectAll() {
	for (JRadioButton button: buttons) {
	    button.setSelected(false);
	}
    }
    */

    /**
     * Enables or disables the component.
     * @param enable true to enable, false to disable
     */
    public void setEnabled(boolean enable) {
	for (JRadioButton button: buttons) {
	    button.setEnabled(enable);
	}
    }

    /**
     * Adds an action listener to the radio buttons.
     * @param listener the listener to add
     */
    public void addActionListener(ActionListener listener) {
	for (JRadioButton button: buttons) {
	    button.addActionListener(listener);
	}
    }
}
