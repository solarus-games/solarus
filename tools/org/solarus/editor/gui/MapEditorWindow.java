/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
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
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import org.solarus.editor.*;
import org.solarus.editor.Map;
import org.solarus.editor.entities.*;

/**
 * Main window of the map editor.
 */
public class MapEditorWindow extends AbstractEditorWindow implements Observer, ProjectObserver {


    private EditorWindow parentEditor;
    /**
     * The current map.
     */
    private Map map;
    // components
    private MapPropertiesView mapPropertiesView;
    private TilePicker tilePicker;
    private MapView mapView;
    // menu items memorized to enable them later
//    private JMenu menuMap;
//    private JMenu menuEdit;
//    private JMenu menuCreate;
//    private JMenuItem menuItemClose;
//    private JMenuItem menuItemSave;
//    private JMenuItem menuItemUndo;
//    private JMenuItem menuItemRedo;
//    private JMenuItem menuItemCut;
//    private JMenuItem menuItemCopy;
//    private JMenuItem menuItemPaste;

    /**
     * Creates a new window.
     * @param quest name of a quest to load, or null to show a dialog to select the quest
     */
    public MapEditorWindow(String quest, EditorWindow parentEditor) {
        setLayout(new BorderLayout());
        //setTitle("Solarus - Map Editor");
        Project.addProjectObserver(this);
        this.parentEditor = parentEditor;
        // set a nice look and feel
        GuiTools.setLookAndFeel();

        // left panel : the map properties and the tile picker

        mapPropertiesView = new MapPropertiesView();
//	mapPropertiesView.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

        tilePicker = new TilePicker();

        //mapPropertiesView.setMinimumSize(new Dimension(0, 0));
        JSplitPane leftPanel = new JSplitPane(JSplitPane.VERTICAL_SPLIT, mapPropertiesView, tilePicker);
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
     * This method is called just after a project is loaded.
     */
    public void currentProjectChanged() {
        //menuMap.setEnabled(true);
        if (getMap() != null) {
            closeMap(); // close the map that was open with the previous project
        }
    }

    /**
     * Give the name of the resource opened in the editor
     * @return the name of the map
     */
    public String getResourceName() {
        return "Map "+getMap().getName();
    }

    /**
     * Give the id of the resource opened in the editor
     * @return the id of the map
     */
    public String getResourceId() {
        return getMap().getId();
    }

    /**
     * Sets the current map. This method is called when the user opens a map,
     * closes the map, or creates a new one.
     * @param map the new map, or null if there is no map loaded
     */
    private void setMap(Map map) {
        // if there was already a map, remove its observers
        if (this.getMap() != null) {
            this.getMap().deleteObservers();
            this.getMap().getEntitySelection().deleteObservers();
        }
        this.map = map;
        

        // notify the views
        mapPropertiesView.setMap(map);
        tilePicker.setMap(map);
        getMapView().setMap(map);

        if (map != null) {
            // observe the history and the selection to enable or disable the items
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
                saveMap();//new ActionListenerSave().actionPerformed(null);
            } else if (answer == JOptionPane.CANCEL_OPTION) {
                result = false;
            }
        }

        return result;
    }

    /**
     * Creates a new map in the project and sets it as the current map.
     */
    protected void newMap() {
        if (!checkCurrentFileSaved()) {
            return;
        }
        try {
            Map map = new Map();
            map.addObserver(parentEditor);
            setMap(map);
        } catch (ZSDXException ex) {
            GuiTools.errorDialog("Cannot create the map: " + ex.getMessage());
        }
    }

    /**
     * Loads a map of the project ans sets it as the current map.
     */
    protected void openMap() {

        if (!checkCurrentFileSaved()) {
            return;
        }

        ResourceChooserDialog dialog = new ResourceChooserDialog(ResourceType.MAP);
        dialog.setLocationRelativeTo(MapEditorWindow.this);
        dialog.pack();
        dialog.setVisible(true);
        String mapId = dialog.getSelectedId();

        if (mapId.length() == 0) {
            return;
        }

        try {
            Map map = new Map(mapId);
            map.addObserver(parentEditor);
            if (map.badTiles()) {
                GuiTools.warningDialog("Some tiles of the map have been removed because they don't exist in the tileset.");
            }
            setMap(map);
        } catch (ZSDXException ex) {
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
     * Saves the current map.
     */
    protected void saveMap() {
        try {
            getMap().save();
        } catch (InvalidEntityException ex) {
            // the map contains an invalid entity
            GuiTools.warningDialog("Could not save the map: " + ex.getMessage() + " Click OK to edit the invalid entity now.");
            EditEntityDialog dialog = new EditEntityDialog( getMap(), ex.getEntity());
            dialog.display();
        } catch (ZSDXException ex) {
            // another problem occured
            GuiTools.errorDialog("Could not save the map: " + ex.getMessage());
        }
    }

    /**
     * @return the map
     */
    protected Map getMap() {
        return map;
    }

    /**
     * @return the mapView
     */
    protected MapView getMapView() {
        return mapView;
    }


}
