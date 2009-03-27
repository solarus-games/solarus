package zsdx.entities;

import zsdx.*;

/**
 * Exception thrown if a tile placed on a map references a non-existing
 * tile pattern in the tileset.
 */
public class NoSuchTilePatternException extends MapException {

    /**
     * Constructor.
     * @param message error message
     */
    public NoSuchTilePatternException(String message) {
	super(message);
    }
    
}
