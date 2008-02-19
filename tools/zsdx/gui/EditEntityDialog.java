package zsdx.gui;

import java.awt.*;
import java.awt.event.*;

import javax.swing.*;

import zsdx.*;

/**
 * A dialog box with some options to edit a map entity.
 */
public class EditEntityDialog extends JDialog {

    /**
     * The component to edit the entity.
     */
    private EditEntityComponent entityComponent;
    
    /**
     * Constructor.
     */
    public EditEntityDialog(Map map, MapEntity entity) {

	super((Frame) null, "Edit an entity", true);

	Container contentPane = getContentPane();
	contentPane.setLayout(new BoxLayout(contentPane, BoxLayout.PAGE_AXIS));
	setResizable(false);
	
	entityComponent = new EditEntityComponent(map, entity);

	JButton buttonOK = new JButton("OK");
	buttonOK.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		applyModificationsAndDispose();
	    }
	});

	getRootPane().setDefaultButton(buttonOK);

	JButton buttonCancel = new JButton("Cancel");
	Action cancelAction = new AbstractAction() {
	    public void actionPerformed(ActionEvent e) {
		dispose();
	    }
	};

	buttonCancel.addActionListener(cancelAction);

	KeyStroke escapeKeyStroke = KeyStroke.getKeyStroke(KeyEvent.VK_ESCAPE, 0, false);	 
	getRootPane().getInputMap(JComponent.WHEN_IN_FOCUSED_WINDOW).put(escapeKeyStroke, "ESCAPE");
	getRootPane().getActionMap().put("ESCAPE", cancelAction);
	
	GridLayout gridLayout = new GridLayout(1, 2); 
	JPanel buttonPanel = new JPanel(gridLayout);
	gridLayout.setHgap(10);

	buttonPanel.add(buttonCancel);
	buttonPanel.add(buttonOK);

	JPanel bottomPanel = new JPanel();
	bottomPanel.setLayout(new BoxLayout(bottomPanel, BoxLayout.LINE_AXIS));

	bottomPanel.add(Box.createHorizontalGlue());
	bottomPanel.add(buttonPanel);

	add(entityComponent);
	add(Box.createVerticalStrut(20));
	add(bottomPanel);
	add(Box.createVerticalStrut(10));
    }
    
    /**
     * Applies the modifications to the entity and closes the dialog.
     * If the modifications are invalid, an error message is displayed
     * and the dialog is not closed.
     */
    private void applyModificationsAndDispose() {

	try {
	    entityComponent.applyModifications();
	    dispose();
	}
	catch (ZSDXException ex) {
	    WindowTools.errorDialog(ex.getMessage());
	}
    }
}
