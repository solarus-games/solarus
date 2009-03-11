package zsdx.map_editor_actions;

import zsdx.*;
import zsdx.entities.*;

/**
 * Replacing an entity by a new one on the map.
 */
public class ActionReplaceEntity extends MapEditorAction {

    private MapEntity oldEntity;
    private MapEntity newEntity;

    /**
     * Creates an action to add an entity.
     * @param map the map
     * @param oldEntity the entity to replace
     * @param newEntity the new entity
     */
    public ActionReplaceEntity(Map map, MapEntity oldEntity, MapEntity newEntity) {
	super(map);
	this.oldEntity = oldEntity;
	this.newEntity = newEntity;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	newEntity.ensureNameIsUnique();
	map.removeEntity(oldEntity);
	map.addEntity(newEntity);
	map.getEntitySelection().unselectAll();
	map.getEntitySelection().select(newEntity);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	map.removeEntity(newEntity);
	map.addEntity(oldEntity);
	map.getEntitySelection().unselectAll();
	map.getEntitySelection().select(oldEntity);
    }

}
