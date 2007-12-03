package editors.map_editor_actions;

import editors.*;

/**
 * Moving some tiles on the map.
 */
public class ActionMoveTiles extends MapEditorAction {

    private TileOnMapList tilesToMove;
    private int dx;
    private int dy;

    /**
     * Creates an action to move one tile.
     * @param map the map
     * @param tileToMove the tile to remove
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     */
    public ActionMoveTiles(Map map, TileOnMap tileToMove, int dx, int dy) {
	super(map);
	this.tilesToMove = new TileOnMapList();
	this.tilesToMove.add(tileToMove);
	this.dx = dx;
	this.dy = dy;
    }

    /**
     * Creates an action to move several tiles.
     * @param map the map
     * @param tilesToMove the tiles to move
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     */
    public ActionMoveTiles(Map map, TileOnMapList tilesToMove, int dx, int dy) {
	super(map);
	this.tilesToMove = new TileOnMapList(tilesToMove);
	this.dx = dx;
	this.dy = dy;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	map.moveTiles(tilesToMove, dx, dy);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	map.moveTiles(tilesToMove, -dx, -dy);
    }

}
