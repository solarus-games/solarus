package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 * This components shows two text fields to set the coordinates (or the size) of something.
 */
public class CoordinatesField extends JPanel {

    // subcomponents
    private JSpinner fieldX;
    private JSpinner fieldY;
    
    /**
     * Constructor.
     */
    public CoordinatesField() {
	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	fieldX = new JSpinner();
	fieldY = new JSpinner();

	Dimension size = new Dimension(80, 25);
	fieldX.setMinimumSize(size);
	fieldY.setMinimumSize(size);

	JSpinner.DefaultEditor editor = (JSpinner.DefaultEditor) fieldX.getEditor();
	editor.getTextField().setColumns(3);
	editor = (JSpinner.DefaultEditor) fieldY.getEditor();
	editor.getTextField().setColumns(3);

	size = new Dimension(5, 0);
	add(fieldX);
	add(Box.createRigidArea(size));
	add(new JLabel("x"));
	add(Box.createRigidArea(size));
	add(fieldY);

	setCoordinates(0, 0);
	setEnabled(false);
    }
        
    /**
     * Enables or disables the two text fields.
     * @param enable true to make the text fields enabled, false to disable them
     */
    public void setEnabled(boolean enable) {
	fieldX.setEnabled(enable);
	fieldY.setEnabled(enable);
    }
    
    /**
     * Adds an action listener to the two number fields.
     * @param actionListener the action listener to add
     */
    public void addActionListener(final ActionListener actionListener) {

	// add the action listener to the two text fields
	JSpinner.DefaultEditor editor = (JSpinner.DefaultEditor) fieldX.getEditor();
	editor.getTextField().addActionListener(actionListener);

	editor = (JSpinner.DefaultEditor) fieldY.getEditor();
	editor.getTextField().addActionListener(actionListener);

	// create a corresponding change listener to the arrows
	ChangeListener changeListener = new ChangeListener() {
	    public void stateChanged(ChangeEvent arg0) {
		actionListener.actionPerformed(null);		
	    }
	};
	fieldX.addChangeListener(changeListener);
	fieldY.addChangeListener(changeListener);
    }

    /**
     * Sets the difference between two elements of the sequences
     * @param dx the difference between two elements of the X field sequence
     * @param dy the difference between two elements of the Y field sequence
     */
    public void setStepSize(int dx, int dy) {
	SpinnerNumberModel spinnerModel = (SpinnerNumberModel) fieldX.getModel();
	spinnerModel.setStepSize(dx);
	
	spinnerModel = (SpinnerNumberModel) fieldY.getModel();
	spinnerModel.setStepSize(dy);
    }
    
    /**
     * Sets the minimum values of the two fields.
     * If the current values are lower than the minimum, they are updated.
     * @param x minimum x value
     * @param y minimum y value
     */
    public void setMinimum(int x, int y) {
	SpinnerNumberModel spinnerModel = (SpinnerNumberModel) fieldX.getModel();
	spinnerModel.setMinimum(x);
	
	spinnerModel = (SpinnerNumberModel) fieldY.getModel();
	spinnerModel.setMinimum(y);
	
	if (getXValue() < x) {
	    setXValue(x);
	}
	
	if (getYValue() < y) {
	    setYValue(y);
	}
    }
    
    /**
     * Returns the value of the X field.
     * @return x
     */
    private int getXValue() {
	Number n = (Number) fieldX.getValue();
	return n.intValue();
    }

    /**
     * Returns the value of the Y field.
     * @return y
     */
    private int getYValue() {
	Number n = (Number) fieldY.getValue();
	return n.intValue();
    }

    /**
     * Returns the two coordinates values in the fields.
     * @return the coordinates entered
     */
    public Point getCoordinates() {
	return new Point(getXValue(), getYValue());
    }

    /**
     * Changes the value of the X field.
     * @param x the new x value
     */
    public void setXValue(int x) {
	fieldX.setValue(new Integer(x));
    }

    /**
     * Changes the value of the Y field.
     * @param y the new y value
     */
    public void setYValue(int y) {
	fieldY.setValue(new Integer(y));
    }

    /**
     * Changes the two coordinates values.
     * @param x the new x value
     * @param y the new y value
     */
    public void setCoordinates(int x, int y) {
	setXValue(x);
	setYValue(y);
    }

    /**
     * Changes the two coordinates values.
     * @param d the new coordinates
     */
    public void setCoordinates(Point p) {
	setCoordinates(p.x, p.y);
    }
}
