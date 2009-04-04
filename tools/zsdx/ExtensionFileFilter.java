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
