package tileset_editor;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.io.*;
import java.util.*;

/**
 * This component provides a text field to set ZSDX root directory.
 */
public class ConfigurationPanel extends JPanel implements Observer {

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
     * @param tileset the tileset, to be notified when
     * ZSDX root path is changed.
     */
    public ConfigurationPanel() {
	super();

	configuration = Configuration.getInstance();
	configuration.addObserver(this);

 	setLayout(new BoxLayout(this, BoxLayout.X_AXIS));

	// create the panel
	setBorder(BorderFactory.createTitledBorder("Configuration"));

	// create the components and put them into the panel
	textFieldPath = new JTextField(50);
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

	update(null, null);
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
	    configuration.setZsdxRootPath(path);
	}
    }

    /**
     * This method is called when the configuration is changed.
     * The component is updated.
     */
    public void update(Observable o, Object obj) {
	textFieldPath.setText(configuration.getZsdxRootPath());
    }
}
