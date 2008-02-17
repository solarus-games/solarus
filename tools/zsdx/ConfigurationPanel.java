package zsdx;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.io.*;

/**
 * This component provides a text field to choose ZSDX root directory.
 */
public class ConfigurationPanel extends JPanel {

    /**
     * The configuration object observed.
     */
    private Configuration configuration;

    /**
     * Text field containing the root directory of ZSDX. 
     */
    private JTextField textFieldPath;

    /**
     * Constructor.
     */
    public ConfigurationPanel() {
	super();

	configuration = Configuration.getInstance();

 	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	// create the panel
	setBorder(BorderFactory.createTitledBorder("Configuration"));

	// create the components and put them into the panel
	textFieldPath = new JTextField(20);
	textFieldPath.setEditable(false);
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

	textFieldPath.setText(configuration.getZsdxRootPath());
    }

    /**
     * Asks to the user the root directory of ZSDX.
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
	    textFieldPath.setText(path);
	}
    }

    /**
     * Returns the root path entered by the user.
     * @return ZSDX root path as chosen by the user
     */
    public String getZsdxRootPathEntered() {
	return textFieldPath.getText();
    }
}
