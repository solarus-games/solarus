package editors;

/**
 * Options indicating how to display the map in the map view of the map editor.
 * This options affect only the map editor, not the game.
 * This class allows to set:
 * - what layers are displayed
 * - show or not the obstacle or non obstacle entities
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
     * Tells whether or not the non obstacle entities are shown,
     * and the same thing for the obstacle entities.
     */
    private boolean[] showObstacles;

    /**
     * True to render the transparency, false to replace the transparent pixels
     * by a background color.
     * The transparency seems to make the program much slower with my Linux.
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
     * @return true if the entities of this layer are shown, false otherwise
     */
    public boolean getShowLayer(int layer) {
	return showLayers[layer];
    }

    /**
     * Sets the layers shown.
     * @param showLowLayer true to show the entities of the low layer
     * @param showIntermediateLayer true to show the entities of the low layer
     * @param showHighLayer true to show the entities of the high layer
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
     * Returns whether or not the entities without and with obstacles are shown.
     * @return an array of two elements, the first one is true if the entities without
     * obstacle are shown, the second one represents the same thing for the obstacle entities.
     */
    public boolean[] getShowObstacles() {
	return showObstacles;
    }

    /**
     * Returns whether or not the entities with or without obstacles are shown.
     * @param obstacle the type of obstacle to consider (MapEntity.OBSTACLE_NONE or MapEntity.OBSTACLE)
     * @return true if the corresponding entities are show, false otherwise
     */
    public boolean getShowObstacle(int obstacle) {
	return showObstacles[obstacle];
    }

    /**
     * Sets whether or not the entities with or without obstacle are shown.
     * @param showNonObstacleEntities true to show the entities without obstacles
     * @param showObstacleEntities true to show the entities with obstacles
     */
    public void setShowObstacles(boolean showNonObstacleEntities, boolean showObstacleEntities) {
	showObstacles[MapEntity.OBSTACLE_NONE] = showNonObstacleEntities;
	showObstacles[MapEntity.OBSTACLE] = showObstacleEntities;
	mapView.repaint();
    }
    
    /**
     * Sets whether or not the entities with or without obstacles are shown.
     * @param obstacle the entities to show or not: MapEntity.OBSTACLE_NONE or MapEntity.OBSTACLE
     * @param show true to make these entities visible, false otherwise
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
     * Returns whether or not an entity is shown, depending on its type,
     * its layer and its obstacle property.
     * @param entity the entity considered
     * @return true if this entity is shown with the current options
     */
    public boolean isEntityShown(MapEntity entity) {
	int layer = entity.getLayer();
	int obstacle = entity.getObstacle();

	if (obstacle > MapEntity.OBSTACLE) {
	    obstacle = MapEntity.OBSTACLE;
	}

	return showLayers[layer] && showObstacles[obstacle];
    }
}
