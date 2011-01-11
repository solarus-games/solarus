package org.solarus.editor;

/**
 * Enumeration to identify the different kinds of resources of a quest.
 */
public enum ResourceType {

    MAP(true, "Map"),
    TILESET(true, "Tileset"),
    MUSIC(false, "Music"),
    SPRITE(false, "Sprite"),
    SOUND(false, "Sound"),
    ITEM(false, "Item"),
    ENEMY(false, "Enemy");

    /**
     * Indicates that the id of elements from this resource type is an integer
     * incremented automatically.
     */
    private boolean idAutoIncremented;

    /**
     * The name of the resource type - added for the GUI
     */
    private String name;

    /**
     * Creates a new resource type.
     * @param idAutoIncremented true if the id of elements from this
     * resource type have to be incremented automatically.
     * @param name name of the resource type
     */
    private ResourceType(boolean idAutoIncremented, String name) {
	this.idAutoIncremented = idAutoIncremented;
        this.name = name;
    }

    /**
     * Returns the value with the specified id.
     * @param id id of the value to get
     * @return the value with this id
     * @throws ZSDXException if there is no resource type with this id
     */
    public static ResourceType get(int id) throws ZSDXException {

	try {
	    return values()[id];
	}
	catch (ArrayIndexOutOfBoundsException ex) {
	    throw new ZSDXException("Unknown resource type '" + id + "'");
	}
    }

    /**
     * Returns the id of this value.
     * @return the id
     */
    public int getId() {
	return ordinal();
    }

    /**
     * Returns whether the id of elements from this resource type is an integer
     * incremented automatically.
     * @return true if the id is incremented automatically
     */
    public boolean isIdAutoIncremented() {
	return idAutoIncremented;
    }

    /**
     * Returns the name of the resource type
     * This name will be used in the GUI in the ResourceChooser
     * @return the name of the resource type
     */
    public String getName() {
        return name;
    }
}
