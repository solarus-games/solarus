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
import java.util.*;

/**
 * This class contains the list of the game resources and their name in the project.
 * Each resource name is associated with an id.
 * This class stores the information of the file project_db.dat.
 */
public class ResourceDatabase extends Observable {

    /**
     * Name of the file storing the resource database.
     */
    public static final String fileName = "project_db.dat";

    /**
     * The project.
     */
    private Project project;
    
    /**
     * The resources.
     */
    private Resource[] resources;

    /**
     * Creates the list of the game resources from the file project_db.dat.
     */
    public ResourceDatabase(Project project) {

	this.project = project;
	this.resources = new Resource[ResourceType.values().length];
	for (ResourceType resourceType: ResourceType.values()) {
	    resources[resourceType.getId()] = new Resource(resourceType.isIdAutoIncremented());
	}
    }

    /**
     * Returns a resource.
     * @param resourceType type of the resource to get
     * @return the resource of this type
     */
    public Resource getResource(ResourceType resourceType) {
	return resources[resourceType.getId()];
    }

    /**
     * Clears all resources.
     */
    public void clear() {
	for (ResourceType resourceType: ResourceType.values()) {
	    getResource(resourceType).clear();
	}
    }
    
    /**
     * Reads the file project_db.dat of the project, i.e. the list
     * of the game resources and their name.
     * @throws ZSDXException if the file contains an error
     * @throws IOException if the file could not be loaded
     */
    public void load() throws ZSDXException, IOException {
	
	int lineNumber = 0;

	try {
	    clear();

	    File dbFile = project.getResourceDatabaseFile();
	    BufferedReader buff = new BufferedReader(new FileReader(dbFile));

	    String line = buff.readLine();
	    while (line != null) {

		lineNumber++;

		if (line.length() != 0 && line.charAt(0) != '#') {
		    // skip the comments and the empty lines

		    StringTokenizer tokenizer = new StringTokenizer(line, "\t");
		    ResourceType resourceType = ResourceType.get(Integer.parseInt(tokenizer.nextToken()));

		    String id = tokenizer.nextToken();
		    String name = tokenizer.nextToken();
		    getResource(resourceType).setElementName(id, name);
		}

		line = buff.readLine();
	    }
	    buff.close();
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException(fileName + " line " + lineNumber + ": Value expected");
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException(fileName + " line " + lineNumber + ": Integer expected");
	}
	catch (ZSDXException ex) {
	    throw new ZSDXException(fileName + " line " + lineNumber + ": " + ex.getMessage());
	}
    }
    
    /**
     * Saves the list of the game resources and their names into the file project_db.dat.
     * @throws IOException if the file could not be written
     */
    public void save() throws IOException {

	try {
	    
	    File dbFile = project.getResourceDatabaseFile();
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(dbFile)));
	    Iterator<String> it;
	    
	    // save each resource
	    for (ResourceType resourceType: ResourceType.values()) {
		Resource resource = getResource(resourceType);
		it = resource.iterator();

		while (it.hasNext()) {
		    String id = it.next();
		    String name = resource.getElementName(id);
		    out.print(resourceType.getId());
		    out.print('\t');
		    out.print(id);
		    out.print('\t');
		    out.print(name);
		    out.println();
		}
	    }
	    out.close();
	}
	catch (ZSDXException ex) {
	    System.err.println("Unexpected error: " + ex.getMessage());
	    ex.printStackTrace();
	    System.exit(1);
	}
    }
}
