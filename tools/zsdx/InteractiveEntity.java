package zsdx;

/**
 * This class represents an interactive map entity, such as a chest, a door, a switch...
 */
public abstract class InteractiveEntity extends MapEntity {

    /**
     * Constructor.
     * @param map the map
     * @param layer layer of the entity
     * @param x x position of the entity on the map
     * @param y y position of the entity on the map
     * @param width width of the entity
     * @param height height of the entity
     * @param computeDefaultName true if this is a new entity, false otherwise
     */
    protected InteractiveEntity(Map map, int layer, int x, int y,
	    int width, int height, boolean computeDefaultName) {
	super(map, layer, x, y, width, height, computeDefaultName);
    }
}
