package zsdx.map_editor_actions;

import java.util.List;
import java.util.LinkedList;
import zsdx.*;
import zsdx.entities.*;

/**
 * Adding some entities on the map.
 */
public class ActionAddEntities extends MapEditorAction {

    private LinkedList<MapEntity> entitiesToAdd;

    /**
     * Creates an action to add an entity.
     * @param map the map
     * @param entity the entity to add
     */
    public ActionAddEntities(Map map, MapEntity entityToAdd) {
	super(map);
	this.entitiesToAdd = new LinkedList<MapEntity>();
	this.entitiesToAdd.add(entityToAdd);
    }

    /**
     * Creates an action to add several entities.
     * @param map the map
     * @param entitiesToAdd the entities to add
     */
    public ActionAddEntities(Map map, List<MapEntity> entitiesToAdd) {
	super(map);
	this.entitiesToAdd = new LinkedList<MapEntity>(entitiesToAdd);
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	for (MapEntity entity: entitiesToAdd) {
	    entity.ensureNameIsUnique();
	    map.addEntity(entity);
	}
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	for (MapEntity entity: entitiesToAdd) {
	    map.removeEntity(entity);
	}
    }

}
