package zsdx.map_editor_actions;

import java.util.List;
import java.util.LinkedList;
import zsdx.*;
import zsdx.entities.*;

/**
 * Removing some entities from the map.
 */
public class ActionRemoveEntities extends MapEditorAction {

    private LinkedList<MapEntity> entitiesToRemove;

    /**
     * Creates an action to remove one entity.
     * @param map the map
     * @param entityToRemove the entity to remove (this object will be copied)
     */
    public ActionRemoveEntities(Map map, MapEntity entityToRemove) {
	super(map);
	this.entitiesToRemove = new LinkedList<MapEntity>();
	this.entitiesToRemove.add(entityToRemove);
    }

    /**
     * Creates an action to remove several entities.
     * @param map the map
     * @param entitiesToRemove the entities to remove
     */
    public ActionRemoveEntities(Map map, List<MapEntity> entitiesToRemove) {
	super(map);
	this.entitiesToRemove = new LinkedList<MapEntity>(entitiesToRemove);
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	for (MapEntity entity: entitiesToRemove) {
	    map.removeEntity(entity);
	}
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	for (MapEntity entity: entitiesToRemove) {
	    map.addEntity(entity);
	}
    }
}
