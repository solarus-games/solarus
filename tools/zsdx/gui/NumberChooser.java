package zsdx.gui;

import java.awt.*;
import javax.swing.*;

/**
 * A component to select an integer number.
 */
public class NumberChooser extends JSpinner {

    /**
     * Creates a new number chooser with the default properties.
     */
    public NumberChooser() {
	super();
	
	// set the size and the number of columns
	setMinimumSize(new Dimension(80, 25));
	JSpinner.DefaultEditor editor = (JSpinner.DefaultEditor) getEditor();
	editor.getTextField().setColumns(3);
	
	setMinimum(0);
	setMaximum(Integer.MAX_VALUE);
	setStepSize(1);
	setNumber(0);
    }

    /**
     * Sets the difference between two elements of the sequences
     * @param stepSize the difference between two elements of the field sequence
     */
    public void setStepSize(int stepSize) {
	SpinnerNumberModel spinnerModel = (SpinnerNumberModel) getModel();
	spinnerModel.setStepSize(stepSize);
    }

    /**
     * Sets the minimum value of the field.
     * @param minimum the minimum value
     */
    public void setMinimum(int minimum) {

	SpinnerNumberModel spinnerModel = (SpinnerNumberModel) getModel();
	spinnerModel.setMinimum(minimum);
		
	if (getNumber() < minimum) {
	    setNumber(minimum);
	}
    }

    /**
     * Sets the maximum value of the field.
     * @param maximum the minimum value
     */
    public void setMaximum(int maximum) {

	SpinnerNumberModel spinnerModel = (SpinnerNumberModel) getModel();
	spinnerModel.setMinimum(maximum);
		
	if (getNumber() > maximum) {
	    setNumber(maximum);
	}
    }

    /**
     * Returns the current value of the field.
     * @return the current value of the field
     */
    public int getNumber() {
	Number n = (Number) super.getValue();
	return n.intValue();
    }

    /**
     * Sets the current value of the field.
     * @param number the new value of the field
     */
    public void setNumber(int number) {
	setValue(new Integer(number));
    }
}
