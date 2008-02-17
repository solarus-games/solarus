package zsdx;

import java.io.*;

/**
 * File filter to selected the file with a given extension.
 */
public class ExtensionFileFilter implements FileFilter {

    /**
     * Extension of the files to select.
     */
    private String extension;

    /**
     * Constructor.
     * @param extension extension of the files to select (without the dot)
     */
    public ExtensionFileFilter(String extension) {
	this.extension = extension;
    }

    /**
     * Returns whether or not the file name ends with the extension given
     * as parameter to the constructor.
     */
    public boolean accept(File file) {
	boolean result = false;
	
	String name = file.getName();
	int dotIndex = name.lastIndexOf('.');

	if (dotIndex != -1) {
	    String extension = name.substring(dotIndex + 1);
	    result = extension.equals(this.extension);
	}
	
	return result;
    }
}
