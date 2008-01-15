package editors;

/**
 * Exception thrown if the user tries to make something wrong in a map.
 */
public class MapException extends ZSDXException {

    /**
     * Constructor.
     * @param message error message
     */
    public MapException(String message) {
	super(message);
    }
    
}
