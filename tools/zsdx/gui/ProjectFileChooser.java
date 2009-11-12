/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
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