package tileset_editor;

/**
 * Exception thrown if the user tries to make something wrong in a tileset.
 */
public class TilesetException extends Exception {

    /**
     * Constructor.
     * @param message error message
     */
    public TilesetException(String message) {
	super(message);
    }
    
}
