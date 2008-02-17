package zsdx;

/**
 * A couple (key, value).
 * It can be used as a combo box item:
 * The key is the id of the item, and the value is the name displayed
 * in the combo box.
 */
public class KeyValue {

    private String key;
    private String value;

    /**
     * Constructor.
     * @param key the key
     * @param value the value
     */
    public KeyValue(String key, String value) {

	if (key == null) {
	    throw new NullPointerException("The key is null");
	}

	this.key = key;
	this.value = value;
    }
    
    /**
     * Returns the key.
     * @return the key
     */
    public String getKey() {
	return key;
    }

    /**
     * Returns the value.
     * @return the value
     */
    public String getValue() {
	return value;
    }
    
    /**
     * Returns a string representation of the object.
     * @return the value
     */
    public String toString() {
	return value;
    }

    /**
     * Compares two key-value objects.
     * @param o another key-value object
     * @return true if the two objects have the same key (the value is ignored)
     */
    public boolean equals(Object o) {

	KeyValue keyValue = (KeyValue) o;

	return keyValue != null && key.equals(keyValue.getKey());
    }
}
