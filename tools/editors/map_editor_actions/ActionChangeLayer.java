package editors.map_editor_actions;

import editors.*;

/**
 * Changing the layer of some tiles.
 */
public class ActionChangeLayer extends MapEditorAction {

    private TileOnMapList tiles;
    private int[] layersBefore; // the layers before the action can be different
    private int layerAfter;

    /**
     * Constructor.
     * @param map the map
     * @param tiles the tile for which the layer will be changed
     * @param layer the layer to set
     */
    public ActionChangeLayer(Map map, TileOnMapList tiles, int layer) {
	super(map);
	this.tiles = new TileOnMapList(tiles);
	this.layerAfter = layer;

	this.layersBefore = new int[tiles.size()];
	int i = 0;
	for (TileOnMap tile: tiles) {
	    this.layersBefore[i] = tile.getLayer();
	    i++;
	}
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	for (TileOnMap tile: tiles) {
	    map.tileSetLayer(tile, layerAfter);
	}
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	int i = 0;
	for (TileOnMap tile: tiles) {
	    map.tileSetLayer(tile, layersBefore[i]);
	    i++;
	}
    }

}
