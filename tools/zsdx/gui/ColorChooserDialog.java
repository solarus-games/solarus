package zsdx.gui;

import java.awt.*;
import javax.swing.*;

/**
 * A simple dialog with a JColorChooser.
 */
public class ColorChooserDialog extends OkCancelDialog {

    /**
     * The JColorChooser component.
     */
    private JColorChooser colorChooser;
    
    /**
     * The color selected by the user.
     */
    private Color selectedColor;

    /**
     * Constructor.
     */
    public ColorChooserDialog() {
	super("Please pick a color", false);
    }

    /**
     * Takes into account the color choice made by the user.
     */
    protected void applyModifications() {
	this.selectedColor = colorChooser.getColor();
    }

    /**
     * Creates and returns the component to show in the dialog box.
     * @return the component to show in the dialog box
     */
    protected JComponent createComponent() {
	colorChooser = new JColorChooser();
	return colorChooser;
    }

    /**
     * Displays the dialog box and returns the selected color.
     * @param initialColor initial color of the JColorChooser
     * @return the color selected by the user, or null if the dialog was cancelled.
     */
    public Color display(Color initialColor) {
	colorChooser.setColor(initialColor);
	super.display();

	return selectedColor;
    }

}
