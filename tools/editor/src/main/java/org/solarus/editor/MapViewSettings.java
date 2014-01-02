/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor;

import org.solarus.editor.entities.*;
import java.util.*;

/**
 * Options indicating how to display the map in the map view of the map editor.
 * This options affect only the map editor, not the game.
 * This class allows to set:
 * - which layers are displayed,
 * - which entities are displayed,
 * - whether transparency is rendered,
 * - whether the grid is displayed.
 */
public class MapViewSettings extends Observable {

    /**
     * The map these view settings belong to.
     */
    private Map map;

    /**
     * Zoom of the map view.
     * The possible values are 0.25, 0.5, 1, 2 (default).
     */
    private double zoom;

    /**
     * Tells whether or not each layer is shown.
     */
    private boolean[] showLayers;

    /**
     * True to render the transparency, false to replace the transparent pixels
     * by a background color.
     */
    private boolean showTransparency;

    /**
     * Tells whether or not the grid is shown.
     */
    private boolean showGrid;

    /**
     * Whether or not each type of entity is shown.
     */
    private HashMap<EntityType, Boolean> showEntityTypes;

    /**
     * Size of a square of the grid (16 pixels by default).
     */
    private int gridSize;

    /**
     * Constructor.
     * @param map The map.
     */
    public MapViewSettings(Map map) {
        this.map = map;
        this.zoom = 2.0;
        this.showLayers = new boolean[] {true, true, true};
        this.showTransparency = true;
        this.showGrid = false;
        this.gridSize = 16;
        this.showEntityTypes = new HashMap<EntityType, Boolean>();
        for (EntityType entityType: EntityType.values()) {
            this.showEntityTypes.put(entityType, true);
        }
    }

    /**
     * Returns the map these view settings apply to.
     * @return The map.
     */
    public Map getMap() {
        return map;
    }

    /**
     * Returns the zoom of the map view.
     * @return the zoom
     */
    public double getZoom() {
        return zoom;
    }

    /**
     * Sets the zoom of the map view.
     * @param zoom the zoom
     */
    public void setZoom(double zoom) {
        if (zoom != this.zoom) {
            double oldZoom = this.zoom;
            this.zoom = zoom;
            setChanged();
            notifyObservers(new ChangeInfo("zoom", oldZoom, zoom));
        }
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
    public boolean getShowLayer(Layer layer) {
        return showLayers[layer.getId()];
    }

    /**
     * Sets whether or not a layer is shown.
     * @param layer a layer
     * @param show true to make the layer visible, false otherwise
     */
    public void setShowLayer(Layer layer, boolean show) {
        if (show != showLayers[layer.getId()]) {
            showLayers[layer.getId()] = show;
            setChanged();
            notifyObservers();
        }
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
        if (showTransparency != this.showTransparency) {
            this.showTransparency = showTransparency;
            setChanged();
            notifyObservers();
        }
    }

    /**
     * Returns whether or not the grid is rendered.
     * @return true if the grid is shown.
     */
    public boolean getShowGrid() {
        return showGrid;
    }

    /**
     * Sets whether or not the gird is rendered.
     * @param showGrid true to show the grid.
     */
    public void setShowGrid(boolean showGrid) {
        if (showGrid != this.showGrid) {
            this.showGrid = showGrid;
            setChanged();
            notifyObservers();
        }
    }

    /**
     * Returns whether or not a type of entity is shown.
     * @param entityType A type of entity.
     * @return true if these entities are shown.
     */
    public boolean getShowEntityType(EntityType entityType) {
        return showEntityTypes.get(entityType);
    }

    /**
     * Sets whether or not a type of entity is shown.
     * @param entityType A type of entity.
     * @return true to show these entities.
     */
    public void setShowEntityType(EntityType entityType, boolean show) {
        if (show != showEntityTypes.get(entityType)) {
            showEntityTypes.put(entityType, show);
            setChanged();
            notifyObservers();
            // TODO add MapViewSettingChange parameter to notifyObservers()
        }
    }

    /**
     * Returns the current grid size in the map view
     * @return The size of a square of the grid in pixels.
     */
    public int getGridSize() {
        return gridSize;
    }

    /**
     * Sets the grid size in the map view.
     * @param gridSize The size of a square of the grid in pixels.
     */
    public void setGridSize(int gridSize) {
        if (gridSize != this.gridSize) {
            this.gridSize = gridSize;
            setChanged();
            notifyObservers();
        }
    }

    /**
     * Returns whether or not an entity is shown, depending on its type,
     * its layer and its obstacle property.
     * @param entity the entity considered
     * @return true if this entity is shown with the current options
     */
    public boolean isEntityShown(MapEntity entity) {

        return getShowLayer(entity.getLayer()) &&
                getShowEntityType(entity.getType());
    }

    /**
     * Info about what has changed for notifyObservers().
     */
    public class ChangeInfo {

        public final String setting;
        public final Object oldValue;
        public final Object newValue;

        public ChangeInfo(String setting, Object oldValue, Object newValue) {
            this.setting = setting;
            this.oldValue = oldValue;
            this.newValue = newValue;
        }
    }
}

