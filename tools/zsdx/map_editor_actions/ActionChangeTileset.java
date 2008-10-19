package zsdx.map_editor_actions;

import zsdx.*;
import zsdx.entities.*;

/**
 * Changing the tileset of the map.
 */
public class ActionChangeTileset extends MapEditorAction {

    private String tilesetIdBefore;
    private String tilesetIdAfter;
    private MapEntities[] allEntitiesBefore;
    
    /**
     * Constructor.
     * @param map the map
     * @param tilesetId id of the new tileset
     */
    public ActionChangeTileset(Map map, String tilesetId) {
	super(map);

	this.tilesetIdAfter = tilesetId;
	this.tilesetIdBefore = map.getTilesetId();
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
    public void execute() throws ZSDXException {
	map.setTileset(tilesetIdAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	map.setTileset(tilesetIdBefore);
	map.setAllEntities(allEntitiesBefore);
    }

}
