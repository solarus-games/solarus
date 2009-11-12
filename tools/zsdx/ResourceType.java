package zsdx;

/**
 * Enumeration to identify the different kinds of resources of a project.
 */
public enum ResourceType {

    MAP(true),
    TILESET(true),
    MUSIC(false),
    SPRITE(false),
    SOUND(false);

    /**
     * Indicates that the id of elements from this resource type is an integer
     * incremented automatically.
     */
    private boolean idAutoIncremented;

    /**
     * Creates a new resource type.
     * @param idAutoIncremented true if the id of elements from this
     * resource type have to be incremented automatically.
     */
    private ResourceType(boolean idAutoIncremented) {
	this.idAutoIncremented = idAutoIncremented;
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
}
