package editors.map_editor_actions;

import editors.*;
import java.util.LinkedList;

/**
 * Changing the direction of some entities.
 */
public class ActionChangeDirection extends MapEditorAction {

    private LinkedList<MapEntity> entities;
    private int[] directionsBefore; // the directions before the action can be different
    private int directionAfter;

    /**
     * Constructor.
     * @param map the map
     * @param entities the entities for which the direction will be changed
     * @param direction the direction to set
     */
    public ActionChangeDirection(Map map, LinkedList<MapEntity> entities, int direction) {
	super(map);
	this.entities = new LinkedList<MapEntity>(entities);
	this.directionAfter = direction;

	this.directionsBefore = new int[entities.size()];
	int i = 0;
	for (MapEntity entity: entities) {
	    this.directionsBefore[i] = entity.getDirection();
	    i++;
	}
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	for (MapEntity entity: entities) {
	    map.setEntityDirection(entity, directionAfter);
	}
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	int i = 0;
	for (MapEntity entity: entities) {
	    map.setEntityDirection(entity, directionsBefore[i]);
	    i++;
	}
    }

}
