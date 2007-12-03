package editors.map_editor_actions;

import editors.*;

/**
 * Bringing some tiles to back.
 */
public class ActionBringToBack extends MapEditorAction {

    private TileOnMapList tiles;
    private TileOnMapList[] allTilesBefore;
    
    /**
     * Constructor.
     * @param map the map
     * @param tiles the tiles to bring to back
     */
    public ActionBringToBack(Map map, TileOnMapList tiles) {
	super(map);
	
	this.tiles = new TileOnMapList(tiles);
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
	map.bringToBack(tiles);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setAllTiles(allTilesBefore);
    }

}
