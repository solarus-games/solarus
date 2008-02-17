package zsdx.map_editor_actions;

import java.util.LinkedList;

import zsdx.*;

/**
 * Bringing some entities to back.
 */
public class ActionBringToBack extends MapEditorAction {

    private LinkedList<MapEntity> entities;
    private MapEntities[] allEntitiesBefore;
    
    /**
     * Constructor.
     * @param map the map
     * @param entities the entities to bring to back
     */
    public ActionBringToBack(Map map, LinkedList<MapEntity> entities) {
	super(map);
	
	this.entities = new LinkedList<MapEntity>(entities);
	this.allEntitiesBefore = new MapEntities[MapEntity.LAYER_NB];

	// copy allEntities
	MapEntities[] allEntities = map.getAllEntities();
	for (int i = 0; i < MapEntity.LAYER_NB; i++ ) {
	    allEntitiesBefore[i] = new MapEntities(allEntities[i]);
	}
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.bringToBack(entities);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setAllEntities(allEntitiesBefore);
    }

}
