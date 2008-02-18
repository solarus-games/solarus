package zsdx;

/**
 * This class represents an moving map entity, i.e. an NPC or an enemy.
 */
public abstract class MovingEntity extends MapEntity {

    // temporary
    protected MovingEntity(Map map) {
	super(map, LAYER_LOW, 0, 0, 0, 0, false);
    }
}
