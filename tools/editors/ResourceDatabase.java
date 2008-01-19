package editors;

import java.io.*;
import java.util.*;

/**
 * This class contains the list of the game resources and their name.
 * Each resource name is associated with an id.
 * This class contains the information of the file game.zsd.
 */
public class ResourceDatabase extends Observable {

    /**
     * The only instance.
     */
    private static ResourceDatabase instance;

    // constants to identify the different kinds of resources
    public static final int RESOURCE_MAP = 0;
    public static final int RESOURCE_TILESET = 1;
    public static final int RESOURCE_MUSIC = 2;
    public static final int RESOURCE_NB = 3;

    /**
     * The resources.
     */
    private Resource[] resources;

    /**
     * Creates the list of the game resources from the file game.zsd.
     */
    public ResourceDatabase() {

	try {
	    resources = new Resource[RESOURCE_NB];

	    resources[RESOURCE_MAP] = new Resource(true);
	    resources[RESOURCE_TILESET] = new Resource(true);
	    resources[RESOURCE_MUSIC] = new Resource(false);
	    load();
	}
	catch (ZSDXException ex) {
	    System.err.println("Cannot read the game resource database: " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Returns the only instance.
     * @return the only instance
     */
    public static ResourceDatabase getInstance() {

	if (instance == null) {
	    instance = new ResourceDatabase();
	}

	return instance;
    }

    /**
     * Reads the file game.zsd, i.e. the list of the game resources and their name.
     * @throws ZSDXException if the file could not be read or contains an error
     */
    public void load() throws ZSDXException {
	
	int lineNumber = 0;

	try {
	    File dbFile = Configuration.getInstance().getResourceDatabaseFile();
	    BufferedReader buff = new BufferedReader(new FileReader(dbFile));

	    for (Resource resource: resources) {
		resource.clear();
	    }

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
		    resources[resourceType].setElementName(id, name);
		}

		line = buff.readLine();
	    }
	    buff.close();
	}
	catch (NoSuchElementException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": Value expected");
	}
	catch (ZSDXException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": " + ex.getMessage());
	}
	catch (NumberFormatException ex) {
	    throw new ZSDXException("Line " + lineNumber + ": Integer expected");
	}
	catch (IOException ex) {
	    throw new ZSDXException(ex.getMessage());
	}
    }
    
    /**
     * Saves the list of the game resources and their names into the file game.zsd.
     * @throws ZSDXException if the file could not be written
     */
    public void save() throws ZSDXException {

	try {
	    
	    File dbFile = Configuration.getInstance().getResourceDatabaseFile();
	    PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(dbFile)));
	    Iterator<String> it;
	    
	    // save each resource
	    for (int i = 0; i < RESOURCE_NB; i++) {
		it = resources[i].iterator();

		while (it.hasNext()) {
		    String id = it.next();
		    String name = resources[i].getElementName(id);
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
	catch (IOException ex) {
	    throw new ZSDXException(ex.getMessage());
	}
    }
}
