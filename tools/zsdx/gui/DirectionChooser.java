package zsdx.gui;

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
     */
    public DirectionChooser(int nbDirections) {
	super();
	
	if (nbDirections == 4) {
	    for (int i = 0; i < 4; i++) {
		addItem(new KeyValue(Integer.toString(i), directionNames4[i]));
	    }
	}
	else {
	    for (int i = 0; i < 8; i++) {
		addItem(new KeyValue(Integer.toString(i), directionNames8[i]));
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
