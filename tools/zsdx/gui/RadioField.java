package zsdx.gui;

import java.awt.*;
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
	super(new GridLayout(1, options.length));

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
     * Returns the index of the radio button currently selected.
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
	
	return i - 1;
    }
    
    /**
     * Select one of the radio buttons.
     * @param index the index of the radio button to make selected
     */
    public void setSelectedIndex(int index) {
	this.buttons[index].setSelected(true);
    }
}
