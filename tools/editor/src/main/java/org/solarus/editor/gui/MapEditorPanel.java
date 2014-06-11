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
package org.solarus.editor.gui;

import java.awt.*;
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
    private MapViewHeader mapViewHeader;

    /**
     * Creates a map editor with the specified map.
     * @param mainWindow The main window of the quest editor.
     * @param mapId Id of the map to open.
     * @throws QuestEditorException If the map could not be loaded.
     */
    public MapEditorPanel(EditorWindow mainWindow, String mapId)
        throws QuestEditorException {

        super(mainWindow, getEditorId(mapId));

        map = new Map(mapId);

        setLayout(new BorderLayout());

        // Left panel : the map properties and the tile picker.
        mapPropertiesView = new MapPropertiesView(this);
        mapPropertiesView.setMinimumSize(new Dimension(
                0, mapPropertiesView.getPreferredSize().height));
        tilePicker = new TilePicker();

        JSplitPane leftPanel = new JSplitPane(
                JSplitPane.VERTICAL_SPLIT, mapPropertiesView, tilePicker);
        leftPanel.setContinuousLayout(true);

        mapView = new MapView(this);
        JScrollPane mapViewScroller = new ViewScroller(mapView, mapView);

        MapViewMouseCoordinates mapViewMouseCoordinates =
                new MapViewMouseCoordinates(getMapView());

        mapViewHeader = new MapViewHeader(map, getMapView());

        JPanel rightPanel = new JPanel(new BorderLayout());
        rightPanel.add(mapViewHeader, BorderLayout.NORTH);
        rightPanel.add(mapViewScroller, BorderLayout.CENTER);
        rightPanel.add(mapViewMouseCoordinates, BorderLayout.SOUTH);
        rightPanel.setMinimumSize(new Dimension(0, 0));

        JSplitPane rootPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, leftPanel, rightPanel);
        rootPanel.setContinuousLayout(true);
        rootPanel.setDividerLocation(350);

        add(rootPanel);

        if (map.badTiles()) {
            GuiTools.warningDialog("Some tiles of the map have been removed because they don't exist in the tileset.");
        }

        // Notify the children views.
        mapPropertiesView.setMap(map);
        tilePicker.setMap(map);
        mapView.setMap(map);

        // Observe the history and the selection to enable or disable menu items.
        map.getHistory().addObserver(getMainWindow());
        map.getEntitySelection().addObserver(getMainWindow());

        mapPropertiesView.update(null, null);
    }

    /**
     * Returns the id of any map editor that edits the specified map.
     * @param mapId Id of a map.
     * @return Id of an editor that edits this map.
     */
    public static String getEditorId(String mapId) {
        return Project.getMapFile(mapId).getAbsolutePath();
    }

    /**
     * Returns the human-readable name of the resource open in the editor.
     * @return The name of the map.
     */
    public String getTitle() {
        return map == null ? "" : "Map: " + map.getName();
    }

    /**
     * Returns the id of the resource open in the editor.
     * @return The id of the map.
     */
    public String getResourceId() {
        return getMap().getId();
    }

    /**
     * This function is called when the user wants to close the current map.
     * If the map is not saved, we propose to save it.
     * @return false if the user canceled.
     */
    public boolean checkCurrentFileSaved() {
        boolean result = true;

        Map map = getMap();
        if (map != null && !map.getHistory().isSaved()) {
            int answer = JOptionPane.showConfirmDialog(this,
                    "Map '" + map.getName() + "' has been modified. Do you want to save it?",
                    "Save the modifications",
                    JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            if (answer == JOptionPane.YES_OPTION) {
                save();
            } else if (answer == JOptionPane.CANCEL_OPTION) {
                result = false;
            }
        }

        return result;
    }

    /**
     * Saves the resource managed in this editor.
     */
    public void save() {

        try {
            getMap().save();
        }
        catch (InvalidEntityException ex) {
            // The map contains an invalid entity.
            String name = ex.getEntity().getName();
            GuiTools.warningDialog(
                    "Cannot not save the map for now: there is an invalid entity." +
                    "\n  Type of entity: " + ex.getEntity().getType().getHumanName() +
                    (name == null ? "" : ("\n  Name: '" + ex.getEntity().getName() + "'")) +
                    "\n  " + ex.getMessage() +
                    "\nClick OK to fix the entity now.");
            EditEntityDialog dialog = new EditEntityDialog(getMap(), ex.getEntity());
            dialog.setLocationRelativeTo(getMapView());
            dialog.display();
        } catch (QuestEditorException ex) {
            // Another problem occurred.
            GuiTools.errorDialog("Could not save the map: " + ex.getMessage());
        }
    }

    /**
     * Closes this editor without confirmation.
     */
    @Override
    public void close() {

        map.deleteObservers();
        map.getEntitySelection().deleteObservers();
        map.getHistory().deleteObservers();

        mapPropertiesView.setMap(null);
        tilePicker.setMap(null);
        mapView.setMap(null);
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
    public MapView getMapView() {
        return mapView;
    }
}

