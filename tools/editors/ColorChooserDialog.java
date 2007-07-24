package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * A simple dialog with a JColorChooser.
 */
public class ColorChooserDialog extends JDialog {

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
	super((Frame) null, "Please pick a color", true);

	setResizable(false);
	((BorderLayout) getLayout()).setHgap(10);

	colorChooser = new JColorChooser();

	JButton buttonOK = new JButton("OK");
	buttonOK.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    selectedColor = colorChooser.getColor();
		    dispose();
		}
	    });

	JButton buttonCancel = new JButton("Cancel");
	buttonCancel.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    selectedColor = null;
		    dispose();
		}
	    });

	GridLayout gridLayout = new GridLayout(1, 2); 
	JPanel buttonPanel = new JPanel(gridLayout);
	gridLayout.setHgap(10);

	buttonPanel.add(buttonOK);
	buttonPanel.add(buttonCancel);

	JPanel bottomPanel = new JPanel();
	bottomPanel.setLayout(new BoxLayout(bottomPanel, BoxLayout.LINE_AXIS));

	bottomPanel.add(Box.createHorizontalGlue());
	bottomPanel.add(buttonPanel);

	add(colorChooser, BorderLayout.CENTER);
	add(bottomPanel, BorderLayout.SOUTH);
    }

    /**
     * Displays the dialog box and returns the selected color.
     * @param initialColor initial color of the JColorChooser
     * @return the color selected by the user, or null if the dialog was cancelled.
     */
    public Color display(Color initialColor) {
	colorChooser.setColor(initialColor);
	pack();
	setVisible(true);

	return selectedColor;
    }
}
