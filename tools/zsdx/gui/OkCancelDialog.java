package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * Abstract class to make a modal dialog box with the standard OK and Cancel button,
 * and optionnaly, an Apply button.
 */
public abstract class OkCancelDialog extends JDialog {

    /**
     * Creates a new dialog box.
     * @param title the text displayed in the dialog box title bar
     * @param showApplyButton true to add an "Apply" button, false
     * to show only the "OK" and the "Cancel" button
     */
    public OkCancelDialog(String title, boolean showApplyButton) {

	super((Frame) null, title, true);

	Container contentPane = getContentPane();
	contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.PAGE_AXIS));
	setResizable(false);

	// OK button
	JButton buttonOK = new JButton("OK");
	buttonOK.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		applyModifications();
		dispose();
	    }
	});
	
	// handle the Enter key
	getRootPane().setDefaultButton(buttonOK);

	// Cancel button
	JButton buttonCancel = new JButton("Cancel");
	Action cancelAction = new AbstractAction() {
	    public void actionPerformed(ActionEvent e) {
		dispose();
	    }
	};
	buttonCancel.addActionListener(cancelAction);

	// handle the Escape key
	KeyStroke escapeKeyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0, false);	 
	getRootPane().getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(escapeKeyStroke, "ESCAPE");
	getRootPane().getActionMap().put("ESCAPE", cancelAction);

	// put the buttons in a component
	int nbButtons = showApplyButton ? 3 : 2;
	GridLayout gridLayout = new GridLayout(1, nbButtons); 
	JPanel buttonPanel = new JPanel(gridLayout);
	gridLayout.setHgap(10);

	buttonPanel.add(buttonOK);
	buttonPanel.add(buttonCancel);
	
	if (showApplyButton) {
	    
	    // Apply button
	    JButton buttonApply = new JButton("Apply");
	    buttonApply.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    applyModifications();
		}
	    });
	    
	    buttonPanel.add(buttonApply);
	}

	JPanel bottomPanel = new JPanel();
	bottomPanel.setLayout(new BoxLayout(bottomPanel, BoxLayout.LINE_AXIS));

	bottomPanel.add(Box.createHorizontalGlue());
	bottomPanel.add(buttonPanel);

	add(getComponent());
	add(Box.createVerticalStrut(20));
	add(bottomPanel);
	add(Box.createVerticalStrut(10));
    }
    
    /**
     * Returns the component to show in the dialog box.
     * The subclasses must implement this method and return whatever they
     * want to display.
     * They don't have to care about the buttons (OK, Cancel, Apply).
     * This method is called by the constructor when creating the dialob box content.
     * @return the component to show in the dialog box
     */
    protected abstract JComponent getComponent();
    
    /**
     * Takes into account the modifications made by the user in the dialog box.
     * This method is called when the user clicks on OK (or presses the Enter key),
     * just before the dialog box is closed, or when he clicks on Apply.
     */
    protected abstract void applyModifications();
    
    /**
     * Displays the dialog box.
     */
    public void display() {
	setLocationRelativeTo(null);
 	pack();
 	setVisible(true);	
    }
}
