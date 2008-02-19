package zsdx.gui;

import javax.swing.*;
import zsdx.*;

/**
 * A combo box to selected a type of transition between two maps.
 */
public class TransitionChooser extends JComboBox {

    /**
     * Creates a new transition chooser.
     */
    public TransitionChooser() {
	super();
	addItem(new KeyValue(Integer.toString(Transition.IMMEDIATE), "No transition"));
	addItem(new KeyValue(Integer.toString(Transition.FADE), "Fade in/out"));
    }
    
    /**
     * Selects a transition in the list.
     * @param transition the transition to make selected
     */
    public void setTransition(int transition) {

	KeyValue item = new KeyValue(Integer.toString(transition), null);
	setSelectedItem(item);
    }
    
    /**
     * Returns the currently selected transition.
     * @return the selected transition
     */
    public int getTransition() {
	
	KeyValue item = (KeyValue) getSelectedItem();
	return Integer.parseInt(item.getKey());
    }
}
