package editors.map_editor_actions;

import java.awt.Dimension;
import editors.*;

/**
 * Changing the tileset of the map.
 */
public class ActionChangeTileset extends MapEditorAction {

    private String tilesetNameBefore;
    private String tilesetNameAfter;
    private TileOnMapList[] allTilesBefore;
    
    /**
     * Constructor.
     * @param map the map
     * @param tilesetName name of the new tileset
     */
    public ActionChangeTileset(Map map, String tilesetName) {
	super(map);

	this.tilesetNameAfter = tilesetName;
	this.tilesetNameBefore = map.getTilesetName();
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
	map.setTileset(tilesetNameAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setTileset(tilesetNameBefore);
	map.setAllTiles(allTilesBefore);
    }

}
