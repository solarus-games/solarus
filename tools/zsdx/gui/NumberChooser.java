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
	this(0, 0, Integer.MAX_VALUE);
    }

    /**
     * Creates a new number chooser with the specified properties.
     * @param initialValue initial value of the field
     * @param minimum the minimum value
     * @param maximum the maximum value
     */
    public NumberChooser(int initialValue, int minimum, int maximum) {
	super();
	
	// set the size and the number of columns
	setMinimumSize(new Dimension(80, 25));
	JSpinner.DefaultEditor editor = (JSpinner.DefaultEditor) getEditor();
	editor.getTextField().setColumns(3);

	setStepSize(1);
	setMinimum(minimum);
	setMaximum(maximum);
	setNumber(initialValue);
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
     * Returns the minimum value of the field.
     * @return the minimum value
     */
    public int getMinimum() {

	SpinnerNumberModel spinnerModel = (SpinnerNumberModel) getModel();
	return ((Integer) spinnerModel.getMinimum()).intValue();
    }

    /**
     * Sets the minimum value of the field.
     * @param minimum the minimum value
     */
    public void setMinimum(int minimum) {

	SpinnerNumberModel spinnerModel = (SpinnerNumberModel) getModel();
	spinnerModel.setMinimum(minimum);
    }

    /**
     * Returns the maximum value of the field.
     * @return the maximum value
     */
    public int getMaximum() {

	SpinnerNumberModel spinnerModel = (SpinnerNumberModel) getModel();
	return ((Integer) spinnerModel.getMaximum()).intValue();
    }

    /**
     * Sets the maximum value of the field.
     * @param maximum the minimum value
     */
    public void setMaximum(int maximum) {

	SpinnerNumberModel spinnerModel = (SpinnerNumberModel) getModel();
	spinnerModel.setMaximum(maximum);
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
	number = Math.max(getMinimum(), Math.min(getMaximum(), number));
	setValue(new Integer(number));
    }
}
