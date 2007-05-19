package tileset_editor;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.io.*;

/**
 * This component provides a text field to set ZSDX root directory.
 */
public class ConfigurationPanel extends JPanel {

    /**
     * Text field containing the root directory of ZSDX. 
     */
    private JTextField textFieldPath;

    /**
     * Constructor.
     */
    public ConfigurationPanel() {
	super();

 	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	// create the panel
	setBorder(BorderFactory.createTitledBorder("Configuration"));

	// create the components and put them into the panel
	textFieldPath = new JTextField(50);
	JButton buttonBrowse = new JButton("Change...");

	add(new JLabel("ZSDX root directory:"));
	add(Box.createRigidArea(new Dimension(5, 0)));
	add(textFieldPath);
	add(Box.createRigidArea(new Dimension(5, 0)));
	add(buttonBrowse);

	// add the action listener to the button
	buttonBrowse.addActionListener(new ActionListener() {

		public void actionPerformed(ActionEvent e) {
		    askZsdxRootPath();
		}

	    });

	/* We put a default path into the text field.
	 * Assuming that we are in the directory 'tools',
	 * ZSDX should be the parent directory.
	 */
	File defaultDirectory = new File("..");
	try {
	    String path = defaultDirectory.getCanonicalPath();
	    Tileset.setZsdxRootPath(path);
	    textFieldPath.setText(path);
	}
	catch (IOException e) {
	    // just left the text field blank if we couldn't get the default path
	}
    }

    /**
     * Asks to the user the root directory of ZSDX.
     * @return ZSDX root path, or null if the user didn't select any directory
     */
    private void askZsdxRootPath() {
	String path = null;

	JFileChooser fileChooser = new JFileChooser(textFieldPath.getText());
	fileChooser.setDialogTitle("Please select the root directory of ZSDX");
	fileChooser.setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
	fileChooser.setDragEnabled(false);
	fileChooser.setMultiSelectionEnabled(false);
	int result = fileChooser.showDialog(null, "Select");

	if (result == JFileChooser.APPROVE_OPTION) {
	    try {
		path = fileChooser.getSelectedFile().getCanonicalPath();
	    }
	    catch (IOException e) {
	    }
	}

	if (path != null) {
	    Tileset.setZsdxRootPath(path);
	    textFieldPath.setText(path);
	}
    }
}
