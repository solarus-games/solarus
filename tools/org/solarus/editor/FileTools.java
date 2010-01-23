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
package org.solarus.editor;

import java.io.*;

/**
 * Utility class to make easy some common operations related to the files:
 * their content, their name, their path or their extension.
 */
public class FileTools {

    /**
     * This class should not be instanciated.
     */
    private FileTools() {

    }

    /**
     * Opens a file and looks for a specified line.
     * The line is added to the file if it was not there.
     */
    public static void ensureFileHasLine(String fileName, String lineWanted) throws IOException {

	// read the file to determine whether or not the line is already there
	BufferedReader in = new BufferedReader(new FileReader(fileName));
	String line;
	boolean found = false;
	
	line = in.readLine();
	while (line != null && !found) {
	    found = line.equals(lineWanted);
	    line = in.readLine();
	}
	in.close();

	if (!found) {
	    // the line has not been found: let's add it to the file
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(fileName, true)));
	    out.println(lineWanted);
	    out.close();
	}
    }

    /**
     * Returns the files with a given extension from a specified directory.
     * @param directory the directory to explore (the subdirectories are not explored)
     * @param extension the extension of the files to keep
     */
    public static File[] getFilesWithExtension(String directory, String extension) {
	
	File directoryFile = new File(directory);
	java.io.FileFilter fileFilter = new ExtensionFileFilter(extension);
	File[] files = directoryFile.listFiles(fileFilter);
	return files;
    }

    /**
     * Returns the name of a file without its extension.
     * The path is not included.
     * @param file a file
     * @return name of this file, without the extension
     */
    public static String getFileNameWithoutExtension(File file) {
	String name = file.getName();
	int dotIndex = name.lastIndexOf('.');

	if (dotIndex != -1) {
	    name = name.substring(0, dotIndex);
	}

	return name;
    }
}
