package editors.map_editor_actions;

import editors.*;

/**
 * Removing some tiles from the map.
 */
public class ActionRemoveTiles extends MapEditorAction {

    private TileOnMapList tilesToRemove;

    /**
     * Creates an action to remove one tile.
     * @param map the map
     * @param tileToRemove the tile to remove
     */
    public ActionRemoveTiles(Map map, TileOnMap tileToRemove) {
	super(map);
	this.tilesToRemove = new TileOnMapList();
	this.tilesToRemove.add(tileToRemove);
    }

    /**
     * Creates an action to remove several tiles.
     * @param map the map
     * @param tilesToRemove the tiles to remove
     */
    public ActionRemoveTiles(Map map, TileOnMapList tilesToRemove) {
	super(map);
	this.tilesToRemove = new TileOnMapList(tilesToRemove);
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	for (TileOnMap tile: tilesToRemove) {
	    map.removeTile(tile);
	}
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	for (TileOnMap tile: tilesToRemove) {
	    map.addTile(tile);
	}
    }

}
