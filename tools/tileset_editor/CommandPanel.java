package tileset_editor;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.io.*;

/**
 * This component provides buttons to create, edit or delete a tile
 * and shows a text field to set ZSDX root directory.
 */
public class CommandPanel extends JPanel {

    /**
     * Text field containing the root directory of ZSDX. 
     */
    private JTextField textFieldPath;

    /**
     * Constructor.
     */
    public CommandPanel() {
	super(new BorderLayout());
	
	// create the panel
	JPanel configurationPanel = new JPanel(new FlowLayout());
	configurationPanel.setBorder(BorderFactory.createTitledBorder("Configuration"));

	// create the components and put them into the panel
	configurationPanel.add(new JLabel("ZSDX root directory:"));
	textFieldPath = new JTextField(20);
	configurationPanel.add(textFieldPath);
	JButton buttonBrowse = new JButton("Browse...");
	configurationPanel.add(buttonBrowse);

	// add the action listener to the button
	buttonBrowse.addActionListener(new ActionListener() {

		public void actionPerformed(ActionEvent e) {
		    String path = askZsdxRootPath();
		    if (path != null) {
			textFieldPath.setText(path);
		    }
		}

	    });

	/* We put a default path into the text field.
	 * Assuming that we are in the directory 'tools',
	 * ZSDX should be the parent directory.
	 */
	File defaultDirectory = new File("..");
	try {
	    textFieldPath.setText(defaultDirectory.getCanonicalPath());
	}
	catch (IOException e) {
	    // just left the text field blank if we couldn't get the default path
	}

	add(configurationPanel, BorderLayout.WEST);
    }

    /**
     * Asks to the user the root directory of ZSDX.
     * @return ZSDX root path, or null if the user didn't select any directory
     */
    public String askZsdxRootPath() {
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
	
	return path;
    }
}
