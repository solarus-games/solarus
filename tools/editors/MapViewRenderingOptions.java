package editors;

/**
 * Options indicating how to display the map in the map view of the map editor.
 * This options affect only the map editor, not the game.
 * This class allows to set:
 * - what layers are displayed
 * - show or not the obstacle or non obstacle tiles
 * - show the transparency
 */
public class MapViewRenderingOptions {

    /**
     * The map view affected by these options.
     */
    private MapView mapView;

    /**
     * Tells whether or not each layer is shown.
     */
    private boolean[] showLayers;

    /**
     * Tells whether or not the non obstacle tiles are shown,
     * and the same thing for the obstacle tiles.
     */
    private boolean[] showObstacles;

    /**
     * True to render the transparency, false to replace the transparent pixels
     * by a background color.
     * The transparency seems to make the program much slower with Linux.
     */
    private boolean showTransparency;

    /**
     * Constructor.
     */
    public MapViewRenderingOptions(MapView mapView) {
	this.mapView = mapView;
	this.showLayers = new boolean[] {true, true, true};
	this.showObstacles = new boolean[] {true, true};
	this.showTransparency = true;
    }

    /**
     * Returns the map.
     * @return the map
     */
    public Map getMap() {
	return mapView.getMap();
    }

    /**
     * Returns whether or not each layer is shown.
     * @return for each layer, true if it is shown and false otherwise
     */
    public boolean[] getShowLayers() {
	return showLayers;
    }

    /**
     * Returns whether or not a layer is shown.
     * @param layer a layer
     * @return true if the tiles of this layer are show, false otherwise
     */
    public boolean getShowLayer(int layer) {
	return showLayers[layer];
    }

    /**
     * Sets the layers shown.
     * @param showLowLayer true to show the tiles of the low layer
     * @param showIntermediateLayer true to show the tiles of the low layer
     * @param showHighLayer true to show the tiles of the high layer
     */
    public void setShowLayers(boolean showLowLayer, boolean showIntermediateLayer, boolean showHighLayer) {
	showLayers[MapEntity.LAYER_LOW] = showLowLayer;
	showLayers[MapEntity.LAYER_INTERMEDIATE] = showIntermediateLayer;
	showLayers[MapEntity.LAYER_HIGH] = showHighLayer;
	mapView.repaint();
    }

    /**
     * Sets whether or not a layer is shown.
     * @param layer a layer
     * @param show true to make the layer visible, false otherwise
     */
    public void setShowLayer(int layer, boolean show) {
	showLayers[layer] = show;
	mapView.repaint();
    }

    /**
     * Returns whether or not the tiles without and with obstacles are shown.
     * @return an array of two elements, the first one is true if the tiles without
     * obstacle are shown, the second one represents the same thing for the obstacle tiles.
     */
    public boolean[] getShowObstacles() {
	return showObstacles;
    }

    /**
     * Returns whether or not the tiles with or without obstacles are shown.
     * @param obstacle the type of obstacle to consider (Tile.OBSTACLE_NONE or Tile.OBSTACLE)
     * @return true if the corresponding tiles are show, false otherwise
     */
    public boolean getShowObstacle(int obstacle) {
	return showObstacles[obstacle];
    }

    /**
     * Sets whether or not the tiles with or without obstacle are shown.
     * @param showNonObstacleTiles true to show the tiles without obstacles
     * @param showObstacleTiles true to show the tiles with obstacles
     */
    public void setShowObstacles(boolean showNonObstacleTiles, boolean showObstacleTiles) {
	showObstacles[Tile.OBSTACLE_NONE] = showNonObstacleTiles;
	showObstacles[Tile.OBSTACLE] = showObstacleTiles;
	mapView.repaint();
    }
    
    /**
     * Sets whether or not the tiles with or without obstacles are shown.
     * @param obstacle the tiles to show or not: Tile.OBSTACLE_NONE or Tile.OBSTACLE
     * @param show true to make these tiles visible, false otherwise
     */
    public void setShowObstacle(int obstacle, boolean show) {
	showObstacles[obstacle] = show;
	mapView.repaint();
    }

    /**
     * Returns true if the transparency is rendered, i.e. if transparent pixels are
     * not drawn.
     * @return true if the transparency is shown, false if the transparent pixels
     * are replaced by a background color.
     */
    public boolean getShowTransparency() {
	return showTransparency;
    }

    /**
     * Sets whether or not the transparency is rendred.
     * @param showTransparency true to show the transparency (i.e. to make the transparent
     * pixels not drawn), false to replace the transparent pixels by a background color.
     *
     */
    public void setShowTransparency(boolean showTransparency) {
	this.showTransparency = showTransparency;
	mapView.repaint();
    }

    /**
     * Returns whether or not a tile is shown, depending on its layer and its obstacle property.
     * @param tileset the tileset
     * @param tile the tile considered
     * @return true if this tile is shown with the current options
     */
    public boolean isTileShown(Tileset tileset, TileOnMap tile) {
	int layer = tile.getLayer();
	int obstacle = tileset.getTile(tile.getTileId()).getObstacle();

	if (obstacle > Tile.OBSTACLE) {
	    obstacle = Tile.OBSTACLE;
	}

	return showLayers[layer] && showObstacles[obstacle];
    }
}
