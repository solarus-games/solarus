package editors.map_editor_actions;

import editors.*;

/**
 * Adding a tile on the map.
 */
public class ActionAddTile extends MapEditorAction {

    private TileOnMap tileAdded;

    /**
     * Constructor.
     * @param map the map
     * @param tile the tile to add
     */
    public ActionAddTile(Map map, TileOnMap tileToAdd) {
	super(map);
	this.tileAdded = tileToAdd;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.addTile(tileAdded);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.removeTile(tileAdded);
    }

}
