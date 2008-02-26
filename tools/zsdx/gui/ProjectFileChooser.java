package zsdx.gui;

import java.io.*;
import javax.swing.*;

/**
 * A dialog box to select the path of a project.
 */
public class ProjectFileChooser extends JFileChooser {

    /**
     * Constructor.
     */
    public ProjectFileChooser() {
	super("..");
	setDialogTitle("Please select the root directory of your project");
	setFileSelectionMode(JFileChooser.DIRECTORIES_ONLY);
	setDragEnabled(false);
	setMultiSelectionEnabled(false);
    }

    /**
     * Shows the dialog box and returns the path selected by the user.
     * @return the path selected by the user, or null if the dialog was cancelled
     */
    public String getProjectPath() {

	String path = null;
	int result = showOpenDialog(null);

	if (result == JFileChooser.APPROVE_OPTION) {
	    try {
		path = getSelectedFile().getCanonicalPath();
	    }
	    catch (IOException e) {
	    }
	}

	return path;
    }
}