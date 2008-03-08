package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import zsdx.*;

/**
 * Abstract class to make a modal dialog box with the standard OK and Cancel button,
 * and optionnaly, an Apply button.
 */
public abstract class OkCancelDialog extends JDialog {

    /**
     * The panel containing the buttons.
     */
    private JPanel bottomPanel;
    
    /**
     * Title of the optional border.
     */
    private String borderTitle;
    
    /**
     * True if the user applied some modifications, false if he cancelled.
     */
    private boolean appliedModifications;

    /**
     * Creates a new dialog box.
     * @param title the text displayed in the dialog box title bar
     * @param showApplyButton true to add an "Apply" button, false
     * to show only the "OK" and the "Cancel" button
     */
    public OkCancelDialog(String title, boolean showApplyButton) {

	super((Frame) null, title, true);
	
	this.appliedModifications = false;

	Container contentPane = getContentPane();
	contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.PAGE_AXIS));
	setResizable(false);

	// OK button
	JButton buttonOK = new JButton("OK");
	buttonOK.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		tryToApplyModifications(true);
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
		    tryToApplyModifications(false);
		}
	    });
	    
	    buttonPanel.add(buttonApply);
	}

	bottomPanel = new JPanel();
	bottomPanel.setLayout(new BoxLayout(bottomPanel, BoxLayout.LINE_AXIS));

	bottomPanel.add(Box.createHorizontalGlue());
	bottomPanel.add(buttonPanel);
    }

    /**
     * Adds a titled border with the specified title.
     * This method must be called before setComponent().
     * @param title the title
     */
    protected void setBorderTitle(String title) {
	this.borderTitle = title;
    }
    
    /**
     * Sets the component to show in the dialog box.
     * The constructor of a subclasses must call this method to set what they
     * want to display.
     * They don't have to care about the buttons (OK, Cancel, Apply).
     * param component the component to show in the dialog box.
     * The dialog box will be empty until this method is called.
     */
    protected void setComponent(JComponent component) {
	
	JPanel panel = new JPanel();
	panel.add(component); // create an intermediate panel to give it the possible title border
	
	if (borderTitle != null) {
	    panel.setBorder(BorderFactory.createTitledBorder(borderTitle));
	}
		
	getContentPane().add(panel);
	getContentPane().add(Box.createVerticalStrut(20));
	getContentPane().add(bottomPanel);
	getContentPane().add(Box.createVerticalStrut(10));
	
    }
    
    /**
     * Takes into account the modifications made by the user in the dialog box.
     * This method is called when the user clicks on OK (or presses the Enter key),
     * just before the dialog box is closed, or when he clicks on Apply.
     * The subclasses must implement this method and do whatever they have to do
     * to apply the user choices. They can throw a ZSDXException which will show
     * an error message.
     * @throws ZSDXException if the user has made incorrect modifications
     */
    protected abstract void applyModifications() throws ZSDXException;

    /**
     * Takes into account the modifications made by the user in the dialog box.
     * This method is called when the user clicks on OK (or presses the Enter key),
     * just before the dialog box is closed, or when he clicks on Apply.
     * This method called applyModifications() and show an error message if the
     * modifications are incorrect.
     * @param closeWindow true to close the window if there is no error
     */
    private void tryToApplyModifications(boolean closeWindow) {
	try {
	    applyModifications();
	    this.appliedModifications = true;
	    if (closeWindow) {
		dispose();
	    }
	}
	catch (ZSDXException ex) {
	    GuiTools.errorDialog(ex.getMessage());
	}
    }
    
    /**
     * Displays the dialog box.
     * @return true if the user applied modifications, false if he cancelled
     */
    public boolean display() {
	setLocationRelativeTo(null);
 	pack();
 	setVisible(true);
 	
 	return appliedModifications;
    }
}
