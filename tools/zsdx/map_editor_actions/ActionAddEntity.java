package zsdx.map_editor_actions;

import zsdx.*;

/**
 * Adding an entity on the map.
 */
public class ActionAddEntity extends MapEditorAction {

    private MapEntity entityAdded;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to add
     */
    public ActionAddEntity(Map map, MapEntity entity) {
	super(map);
	this.entityAdded = entity;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.addEntity(entityAdded);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.removeEntity(entityAdded);
    }

}
