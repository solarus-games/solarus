package zsdx;

import java.util.*;

/**
 * This class lists all elements of a certain resource.
 * A Resource object contain the maps, another one contains the musics, etc...
 */
public class Resource extends Observable {

    /**
     * Id and name of each element.
     */
    private LinkedHashMap<String, String> elements;

    /**
     * True if the id is an auto-incremented integer, false if it is a custom string.
     */
    private boolean autoIncrementId;

    /**
     * Maximum id already assigned to an element (used only if autoIncrementId is true).
     */
    private int maxId;

    /**
     * Creates the resource.
     * @param autoIncrementId true if you want the id to be an auto-incremented integer,
     * false if you prefer a custom string.
     */
    public Resource(boolean autoIncrementId) {

	this.autoIncrementId = autoIncrementId;
	this.maxId = -1;
	this.elements = new LinkedHashMap<String, String>();
    }

    /**
     * Returns an iterator over the resource ids.
     * @return an iterator over the ids
     */
    public Iterator<String> iterator() {
	return elements.keySet().iterator();
    }
    
    /**
     * Returns an array with the id of all elements.
     * @return an array with the id of all elements.
     */
    public String[] getIds() {
	
	String[] ids = new String[elements.size()];
	int i = 0;

	for (String id: elements.keySet()) {
	    ids[i++] = id;
	}

	return ids;
    }
    
    /**
     * Returns whether an element exists.
     * @param id id of the element
     * @return true if it exists in the resource, false otherwise
     */
    public boolean exists(String id) {
	return elements.containsKey(id);
    }

    /**
     * Returns the name of an element from its id.
     * @param id id of an element
     * @return the name of this element
     * @throws ZSDXException if this element doesn't exist
     */
    public String getElementName(String id) throws ZSDXException {

	String name = elements.get(id);

	if (name == null) {
	    throw new ZSDXException("There is no element with id " + id);
	}

	return name;
    }

    /**
     * Changes the name of an element. If the element doesn't exist in the
     * database, it is added.
     * @param id id of an element
     * @param name the name of the element
     * @throws ZSDXException if the id is not valid
     */
    public void setElementName(String id, String name) throws ZSDXException {

	if (id.length() == 0) {
	    throw new ZSDXException("Empty id for element '" + name);
	}

	if (autoIncrementId) {
	    
	    try {
		int intId = Integer.parseInt(id);
		
		if (intId > maxId) {
		    maxId = intId;
		}
	    }
	    catch (NumberFormatException ex) {
		throw new ZSDXException("Invalid id '" + id + "' for element '" + name +
					"': the value should be an integer number");
	    }
	}

	String oldName = elements.get(id);

	if (oldName == null || !name.equals(oldName)) {

	    // the element doesn't exist yet, or its name has just been changed
	    elements.put(id, name);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Removes all elements from the resource.
     */
    public void clear() {
	elements.clear();
    }

    /**
     * Computes an id for a new element. This makes sure the id is not used yet.
     * @return an available id you can assign to a new element
     */
    public String computeNewId() {

	maxId++;

	setChanged();
	notifyObservers();

	return Integer.toString(maxId);
    }
}
