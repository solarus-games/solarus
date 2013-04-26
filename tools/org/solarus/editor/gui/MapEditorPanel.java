/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.gui;

import java.awt.*;
import java.util.*;
import javax.swing.*;
import org.solarus.editor.*;
import org.solarus.editor.Map;
import org.solarus.editor.entities.*;

/**
 * Components that allows to edit a map.
 */
public class MapEditorPanel extends AbstractEditorPanel {

    /**
     * The current map.
     */
    private Map map;

    // components
    private MapPropertiesView mapPropertiesView;
    private TilePicker tilePicker;
    private MapView mapView;

    /**
     * Creates a new map editor without map.
     */
    public MapEditorPanel(EditorWindow parentEditor) {
        setLayout(new BorderLayout());
        Project.addProjectObserver(this);
        this.parentEditor = parentEditor;

        // left panel : the map properties and the tile picker
        mapPropertiesView = new MapPropertiesView();
        mapPropertiesView.setMinimumSize(new Dimension(
                0, mapPropertiesView.getPreferredSize().height));
        tilePicker = new TilePicker();

        JSplitPane leftPanel = new JSplitPane(
                JSplitPane.VERTICAL_SPLIT, mapPropertiesView, tilePicker);
        leftPanel.setContinuousLayout(true);
        leftPanel.resetToPreferredSizes();

        mapView = new MapView(this);
        JScrollPane mapViewScroller = new JScrollPane(getMapView());

        MapViewMouseCoordinates mapViewMouseCoordinates =
                new MapViewMouseCoordinates(getMapView());

        MapViewHeader mapViewHeader = new MapViewHeader(getMapView());

        JPanel rightPanel = new JPanel(new BorderLayout());
        rightPanel.add(mapViewHeader, BorderLayout.NORTH);
        rightPanel.add(mapViewScroller, BorderLayout.CENTER);
        rightPanel.add(mapViewMouseCoordinates, BorderLayout.SOUTH);

        //leftPanel.setMinimumSize(new Dimension(0, 0));
        //rightPanel.setMinimumSize(new Dimension(0, 0));
        JSplitPane rootPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, leftPanel, rightPanel);
        rootPanel.setContinuousLayout(true);
        rootPanel.setDividerLocation(350);
        rootPanel.resetToPreferredSizes();

        add(rootPanel);
        setMap(null);
    }

    /**
     * Creates a new map editor with an existing map.
     */
    public MapEditorPanel(EditorWindow parentEditor, Map map) {
        this(parentEditor);
        setMap(map);
    }

    /**
     * This method is called just after a project is loaded.
     */
    public void currentProjectChanged() {
        if (getMap() != null) {
            closeMap();  // Close the map that was open with the previous project.
        }
    }

    /**
     * Returns the human-readable name of the resource open in the editor.
     * @return The name of the map.
     */
    public String getResourceName() {
        return map == null ? "" : "Map: " + getMap().getName();
    }

    /**
     * Returns the id of the resource open in the editor.
     * @return The id of the map.
     */
    public String getResourceId() {
        return getMap().getId();
    }

    /**
     * Sets the current map. This method is called when the user opens a map,
     * closes the map, or creates a new one.
     * @param map The new map, or null if there is no map loaded.
     */
    private void setMap(Map map) {

        // if there was already a map, remove its observers
        if (this.getMap() != null) {
            this.getMap().deleteObservers();
            this.getMap().getEntitySelection().deleteObservers();
        }

        this.map = map;

        // Notify the children views.
        mapPropertiesView.setMap(map);
        tilePicker.setMap(map);
        getMapView().setMap(map);

        if (map != null) {
            // Observe the history and the selection to enable or disable the items.
            map.getHistory().addObserver(parentEditor);
            map.getEntitySelection().addObserver(parentEditor);
            this.parentEditor.update(null, null);
        }
    }

    /**
     * This function is called when the history changes.
     * @param o the history
     * @param obj additional parameter
     */
    public void update(Observable o, Object obj) {
        this.parentEditor.update(o, obj);
    }

    /**
     * This function is called when the user wants to close the current map.
     * If the map is not saved, we propose to save it.
     * @return false if the user cancelled
     */
    public boolean checkCurrentFileSaved() {
        boolean result = true;

        if (getMap() != null && !map.getHistory().isSaved()) {
            int answer = JOptionPane.showConfirmDialog(this,
                    "The map has been modified. Do you want to save it?",
                    "Save the modifications",
                    JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            if (answer == JOptionPane.YES_OPTION) {
                saveMap();
            } else if (answer == JOptionPane.CANCEL_OPTION) {
                result = false;
            }
        }

        return result;
    }

    /**
     * Creates a new map in the project and loads it in this editor.
     */
    public void newMap() {

        if (getMap() != null) {
            throw new IllegalStateException("A map is already open in this editor");
        }

        try {
            String mapId = JOptionPane.showInputDialog(null, "Please enter the ID of your new map",
                    "Map ID", JOptionPane.QUESTION_MESSAGE);

            if (mapId != null) {
                if (Project.getResource(ResourceType.MAP).exists(mapId)) {
                    throw new MapException("A map already exists with the ID '" + mapId + "'");
                }

                Map map = new Map(mapId);
                map.addObserver(parentEditor);
                setMap(map);
            }
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Cannot create the map: " + ex.getMessage());
        }
    }

    /**
     * Lets the user choose a map to open and loads it in this editor.
     */
    protected void openMap() {

        if (getMap() != null) {
            throw new IllegalStateException("A map is already open in this editor");
        }

        ResourceChooserDialog dialog = new ResourceChooserDialog(ResourceType.MAP);
        dialog.setLocationRelativeTo(MapEditorPanel.this);
        dialog.pack();
        dialog.setVisible(true);
        String mapId = dialog.getSelectedId();

        if (mapId.length() == 0) {
            return;
        }

        try {
            if (!Project.getResource(ResourceType.MAP).exists(mapId)) {
                throw new MapException("Map with ID '" + mapId + "' does not exist");
            }

            Map map = new Map(mapId);
            map.addObserver(parentEditor);
            if (map.badTiles()) {
                GuiTools.warningDialog("Some tiles of the map have been removed because they don't exist in the tileset.");
            }
            setMap(map);
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Could not load the map: " + ex.getMessage());
        }
    }

    /**
     * Closes the current map.
     */
    private void closeMap() {
        if (!checkCurrentFileSaved()) {
            return;
        }
        setMap(null);
    }

    /**
     * Saves the resource managed in this editor.
     */
    public void save() {
        saveMap();
    }

    /**
     * Saves the current map.
     */
    protected void saveMap() {
        try {
            getMap().save();
        }
        catch (InvalidEntityException ex) {
            // the map contains an invalid entity
            String name = ex.getEntity().getName();
            GuiTools.warningDialog(
                    "Cannot not save the map for now: there is an invalid entity." +
                    "\n  Type of entity: " + ex.getEntity().getType().getHumanName() +
                    (name == null ? "" : ("\n  Name: '" + ex.getEntity().getName() + "'")) +
                    "\n  " + ex.getMessage() +
                    "\nClick OK to fix the entity now.");
            EditEntityDialog dialog = new EditEntityDialog( getMap(), ex.getEntity());
            dialog.display();
        } catch (QuestEditorException ex) {
            // another problem occurred
            GuiTools.errorDialog("Could not save the map: " + ex.getMessage());
        }
    }

    /**
     * @return The map.
     */
    public Map getMap() {
        return map;
    }

    /**
     * @return The map view.
     */
    protected MapView getMapView() {
        return mapView;
    }
}
