package zsdx.entities;

import zsdx.*;

/**
 * Exception thrown if the user tries to make something wrong in a tileset.
 */
public class TilesetException extends ZSDXException {

    /**
     * Constructor.
     * @param message error message
     */
    public TilesetException(String message) {
	super(message);
    }
    
}
