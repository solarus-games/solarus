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
