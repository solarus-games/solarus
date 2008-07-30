package zsdx;

import java.awt.*;
import javax.swing.*;
import java.util.*;

/**
 * Represents an entry point in a map.
 */
public class MapEntrance extends InteractiveEntity {

    /**
     * Name of this kind of entity.
     */
    public static final String entityTypeName = "Entrance";
    
    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription generalImageDescription =
	new EntityImageDescription("map_entrance_1.png", 0, 0, 32, 32);

    /**
     * Origin point of an entrance.
     */
    private static final Point origin = new Point(8, 13);
    
    /**
     * Icons of an entrance for each direction. 
     */
    private static ImageIcon[] icons;

    // load the icons
    static {
	String path = "zsdx/images/";

	icons = new ImageIcon[4];
	for (int i = 0; i < 4; i++) {
	    icons[i] = new ImageIcon(path + "map_entrance_" + i + ".png");
	}
    }
    
    /**
     * Creates a new map entrance at the specified location.
     * @param map the map
     * @param x x coordinate of the entrance
     * @param y y coordinate of the entrance
     */
    public MapEntrance(Map map, int x, int y) {
	super(map, LAYER_LOW, x, y, 16, 16);
	setDirection(1);
    }

    /**
     * Creates an existing map entrance from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public MapEntrance(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);
	
	setSizeImpl(16, 16);
    }

    /**
     * Returns an integer identifying the kind of entity.
     * @return MapEntity.ENTITY_ENTRANCE
     */
    public int getType() {
	return ENTITY_ENTRANCE;
    }

    /**
     * Returns the coordinates of the origin point of the entity.
     * @return (8,13)
     */
    protected Point getOrigin() {
	return origin;
    }

    /**
     * Returns the number of possible directions of the entity.
     * @return 4
     */
    public int getNbDirections() {
	return 4;
    }
    
    /**
     * Returns whether the entity has an identifier.
     * @return true
     */
    public boolean hasName() {
	return true;
    }
}
