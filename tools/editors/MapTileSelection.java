package editors;

import java.util.*;

/**
 * Represents the tiles selected in the map editor.
 */
public class MapTileSelection extends Observable {

    /**
     * The selected tiles.
     */
    private TileOnMapVector tiles;

    /**
     * The map.
     */
    private Map map;

    /**
     * Constructor.
     * @param map the map
     */
    public MapTileSelection(Map map) {
	this.map = map;
	this.tiles = new TileOnMapVector();
    }

    /**
     * Returns a selected tile.
     * @param index index of the tile to get
     * @return the tile at this index
     */
    public TileOnMap getTile(int index) {
	return tiles.get(index);
    }

    /**
     * Returns whether or not a tile is selected.
     * @param tile a tile
     * @return true if the tile is selected, false otherwise
     */
    public boolean isSelected(TileOnMap tile) {
	return tiles.contains(tile);
    }

    /**
     * Returns the number of tiles selected.
     * @return the number of tiles selected.
     */
    public int getNbTilesSelected() {
	return tiles.size();
    }

    /**
     * Returns whether or not the selection is empty.
     * @return true if there is no tile selected, false otherwise
     */
    public boolean isEmpty() {
	return tiles.size() == 0;
    }

    /**
     * Selects a tile.
     * @param tile the tile to select
     */
    public void select(TileOnMap tile) {
	if (!isSelected(tile)) {
	    tiles.add(tile);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Unselects a tile.
     * @param tile the tile to unselect
     */
    public void unSelect(TileOnMap tile) {
	if (tiles.contains(tile)) {
	    tiles.remove(tile);
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Unselects all tiles.
     */
    public void unSelectAll() {
	tiles.clear();
	setChanged();
	notifyObservers();
    }

    /**
     * Changes the selection state of a tile.
     * @param tile the tile to select or unselect
     */
    public void switchSelection(TileOnMap tile) {
	if (isSelected(tile)) {
	    unSelect(tile);
	}
	else {
	    select(tile);
	}
    }

    /**
     * Removes the selected tiles from the map.
     */
    public void removeFromMap() {

	if (tiles.size() > 0) {

	    while (tiles.size() > 0) {
		TileOnMap tile = tiles.firstElement();
		tiles.remove(0);
		map.removeTile(tile);
	    }
	    
	    setChanged();
	    notifyObservers();
	}
    }

    /**
     * Selects the first tile under a point of the map, in the three layers,
     * starting with the high layer.
     * @param x x of the point
     * @param y y of the point
     * @return true if a tile was found and selected, false otherwise
     */
    public boolean select(int x, int y) {

	return select(Tile.LAYER_ABOVE, x, y)
	    || select(Tile.LAYER_INTERMEDIATE, x, y)
	    || select(Tile.LAYER_BELOW, x, y);

    }

    /**
     * Selects the first tile under a point of the map, in a specified layer.
     * @param layer a layer: Tile.LAYER_BELOW, Tile.LAYER_INTERMEDIATE or Tile.LAYER_ABOVE
     * @param x x of the point
     * @param y y of the point
     * @return true if a tile was found and selected, false otherwise
     */
    public boolean select(int layer, int x, int y) {

	TileOnMapVector tileVector = map.getTiles(layer);
	for (int i = tileVector.size() - 1; i >= 0; i--) {

	    TileOnMap tile = tileVector.get(i);
	    if (tile.containsPoint(x, y)) {
		select(tile);
		return true;
	    }
	}

	return false;
    }

    /**
     * Changes the layer of the selected tiles. You should call this method instead of
     * calling directly TileOnMap.setLayer() because the tiles of the 3 layers are
     * stored in 3 different arrays.
     * @param layer the new layer
     */
    public void setLayer(int layer) {

	Enumeration<TileOnMap> e = tiles.elements();
	while (e.hasMoreElements()) {
	    
	    TileOnMap tile = e.nextElement();
	    map.tileSetLayer(tile, layer);
	}
    }
}
