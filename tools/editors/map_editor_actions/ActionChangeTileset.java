package editors.map_editor_actions;

import editors.*;

/**
 * Changing the tileset of the map.
 */
public class ActionChangeTileset extends MapEditorAction {

    private int tilesetIdBefore;
    private int tilesetIdAfter;
    private TileOnMapList[] allTilesBefore;
    
    /**
     * Constructor.
     * @param map the map
     * @param tilesetId id of the new tileset
     */
    public ActionChangeTileset(Map map, int tilesetId) {
	super(map);

	this.tilesetIdAfter = tilesetId;
	this.tilesetIdBefore = map.getTilesetId();
	this.allTilesBefore = new TileOnMapList[Tile.LAYER_NB];

	// copy allTiles
	TileOnMapList[] allTiles = map.getAllTiles();
	for (int i = 0; i < Tile.LAYER_NB; i++ ) {
	    allTilesBefore[i] = new TileOnMapList(allTiles[i]);
	}
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.setTileset(tilesetIdAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setTileset(tilesetIdBefore);
	map.setAllTiles(allTilesBefore);
    }

}
