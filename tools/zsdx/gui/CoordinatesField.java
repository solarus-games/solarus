package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * This components shows two text fields to set the coordinates (or the size) of something.
 */
public class CoordinatesField extends JPanel {

    // subcomponents
    private JTextField fieldX;
    private JTextField fieldY;
    private JButton buttonSet;
	
    /**
     * Constructor.
     * @param showButtonSet true to add a button "Set" next to the two text fields
     * (this button will have any action listener you add by calling addActionListener())
     */
    public CoordinatesField(boolean showButtonSet) {
	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	fieldX = new JTextField(3);
	fieldY = new JTextField(3);

	Dimension size = new Dimension(40, 25);
	fieldX.setMinimumSize(size);
	fieldY.setMinimumSize(size);

	size = new Dimension(5, 0);
	add(fieldX);
	add(Box.createRigidArea(size));
	add(new JLabel("x"));
	add(Box.createRigidArea(size));
	add(fieldY);

	if (showButtonSet) {
	    buttonSet = new JButton("Set");
	    add(Box.createRigidArea(new Dimension(5, 0)));
	    add(buttonSet);
	}
	
	setCoordinates(0, 0);
	setEnabled(false);
    }
    
    /**
     * Adds an action listener to the two text fields.
     * @param listener the action listener to add
     */
    public void addActionListener(ActionListener listener) {
	fieldX.addActionListener(listener);
	fieldY.addActionListener(listener);
	
	if (buttonSet != null) {
	    buttonSet.addActionListener(listener);
	}
    }
    
    /**
     * Enables or disables the two text fields.
     * @param enable true to make the text fields enabled, false to disable them
     */
    public void setEnabled(boolean enable) {
	fieldX.setEnabled(enable);
	fieldY.setEnabled(enable);

	if (buttonSet != null) {
	    buttonSet.setEnabled(enable);
	}
    }
    
    /**
     * Returns the value of the X field.
     * @return x
     */
    private int getXValue() {
	return Integer.parseInt(fieldX.getText());
    }

    /**
     * Returns the value of the Y field.
     * @return y
     */
    private int getYValue() {
	return Integer.parseInt(fieldY.getText());
    }
    
    /**
     * Returns the two coordinates values in the fields.
     * @return the coordinates entered
     */
    public Dimension getCoordinates() {
	return new Dimension(getXValue(), getYValue());
    }

    /**
     * Changes the value of the X field.
     * @param x the new x value
     */
    public void setXValue(int x) {	
	fieldX.setText(Integer.toString(x));
    }

    /**
     * Changes the value of the Y field.
     * @param y the new y value
     */
    public void setYValue(int y) {	
	fieldY.setText(Integer.toString(y));
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
    public void setCoordinates(Dimension d) {
	setCoordinates(d.width, d.height);
    }
}
