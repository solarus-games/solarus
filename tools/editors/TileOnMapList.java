package editors;

import java.util.*;

/**
 * A linked list of TileOnMap.
 */
public class TileOnMapList extends LinkedList<TileOnMap> {

    /**
     * Version number of the class serialization.
     */
    public static final long serialVersionUID = 1L;
   
    /**
     * Constructor.
     */
    public TileOnMapList() {
	super();
    }

    /**
     * Copy constructor.
     */
    public TileOnMapList(TileOnMapList other) {
	super(other);
    }

}
