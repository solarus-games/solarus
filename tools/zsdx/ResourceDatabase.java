package zsdx;

import java.io.*;
import java.util.*;

/**
 * This class contains the list of the game resources and their name in the project.
 * Each resource name is associated with an id.
 * This class contains the information of the file game.zsd.
 */
public class ResourceDatabase extends Observable {

    // constants to identify the different kinds of resources
    public static final int RESOURCE_MAP = 0;
    public static final int RESOURCE_TILESET = 1;
    public static final int RESOURCE_MUSIC = 2;
    public static final int RESOURCE_SPRITE = 3;
    public static final int RESOURCE_NB = 4;
    
    public static final String fileName = "project_db.zsd";

    /**
     * The project.
     */
    private Project project;
    
    /**
     * The resources.
     */
    private Resource[] resources;

    /**
     * Creates the list of the game resources from the file project_db.zsd.
     */
    public ResourceDatabase(Project project) {

	this.project = project;
	
	resources = new Resource[RESOURCE_NB];

	resources[RESOURCE_MAP] = new Resource(true);
	resources[RESOURCE_TILESET] = new Resource(true);
	resources[RESOURCE_MUSIC] = new Resource(false);
	resources[RESOURCE_SPRITE] = new Resource(false);
    }

    /**
     * Returns a resource.
     * @param resourceType type of the resource to get: RESOURCE_MAP, RESOURCE_MUSIC, etc.
     * @return the resource of this type
     */
    public Resource getResource(int resourceType) {
	return resources[resourceType];
    }

    /**
     * Clears all resources.
     */
    public void clear() {

	for (int i = 0; i < RESOURCE_NB; i++) {
	    getResource(i).clear();
	}
    }
    
    /**
     * Reads the file project_db.zsd of the project, i.e. the list
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
		    int resourceType = Integer.parseInt(tokenizer.nextToken());

		    if (resourceType < 0 || resourceType >= RESOURCE_NB) {
			throw new ZSDXException("Unknown resource type '" + resourceType + "'");
		    }
		    
		    String id = tokenizer.nextToken();
		    String name = tokenizer.nextToken();
		    getResource(resourceType).setElementName(id, name);
		}

		line = buff.readLine();
	    }
	    buff.close();
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("game.zsd: Line " + lineNumber + ": Value expected");
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("game.zsd: Line " + lineNumber + ": Integer expected");
	}
	catch (ZSDXException ex) {
	    throw new ZSDXException("game.zsd: Line " + lineNumber + ": " + ex.getMessage());
	}
    }
    
    /**
     * Saves the list of the game resources and their names into the file game.zsd.
     * @throws IOException if the file could not be written
     */
    public void save() throws IOException {

	try {
	    
	    File dbFile = project.getResourceDatabaseFile();
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(dbFile)));
	    Iterator<String> it;
	    
	    // save each resource
	    for (int i = 0; i < RESOURCE_NB; i++) {
		Resource resource = getResource(i);
		it = resource.iterator();

		while (it.hasNext()) {
		    String id = it.next();
		    String name = resource.getElementName(id);
		    out.print(i);
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
