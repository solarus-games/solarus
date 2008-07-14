package zsdx;

import java.util.StringTokenizer;

/**
 * This class represents an interactive map entity, such as a chest, a door, a switch...
 */
public abstract class InteractiveEntity extends MapEntity {

    /**
     * Creates a new interactive entity.
     * @param map the map
     * @param layer layer of the entity
     * @param x x position of the entity on the map
     * @param y y position of the entity on the map
     * @param width width of the entity
     * @param height height of the entity
     */
    protected InteractiveEntity(Map map, int layer, int x, int y,
				int width, int height) {
	super(map, layer, x, y, width, height, true);
    }
    
    /**
     * Creates an existing interactive entity from a string.
     * @param map the map
     * @param tokenizer the string tokenizer, which has already parsed the type of entity
     * but not yet the common properties
     * @throws ZSDXException if there is a syntax error in the string
     */
    public InteractiveEntity(Map map, StringTokenizer tokenizer) throws ZSDXException {
	super(map, tokenizer);
    }


}
