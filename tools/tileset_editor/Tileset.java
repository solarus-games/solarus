package tileset_editor;

import java.util.*;

/**
 * This class describes a tileset.
 */
public class Tileset extends Observable {

    /**
     * Number of tiles in the tileset.
     */
    private int nbTiles;

    /**
     * The tiles.
     */
    private TileImage[] tiles;

    /**
     * Creates a tileset.
     * @param name name of the tileset (existing or new, for example "house")
     */
    public Tileset(String name) {
	super();


    }
}
