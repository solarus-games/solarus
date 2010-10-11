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
import javax.swing.event.InternalFrameAdapter;
import javax.swing.event.InternalFrameEvent;
import org.solarus.editor.*;
import org.solarus.editor.Map;
import org.solarus.editor.entities.*;

/**
 * Main window of the map editor.
 */
public class MapEditorWindow extends AbstractEditorWindow implements Observer, ProjectObserver {

    /**
     * The current map.
     */
    private Map map;
    // components
    private MapPropertiesView mapPropertiesView;
    private TilePicker tilePicker;
    private MapView mapView;
    // menu items memorized to enable them later
    private JMenu menuMap;
    private JMenu menuEdit;
    private JMenu menuCreate;
    private JMenuItem menuItemClose;
    private JMenuItem menuItemSave;
    private JMenuItem menuItemUndo;
    private JMenuItem menuItemRedo;
    private JMenuItem menuItemCut;
    private JMenuItem menuItemCopy;
    private JMenuItem menuItemPaste;

    /**
     * Creates a new window.
     * @param quest name of a quest to load, or null to show a dialog to select the quest
     */
    public MapEditorWindow(String quest) {
        setTitle("Solarus - Map Editor");
        Project.addProjectObserver(this);

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
        JScrollPane mapViewScroller = new JScrollPane(mapView);

        MapViewMouseCoordinates mapViewMouseCoordinates =
                new MapViewMouseCoordinates(mapView);

        MapViewHeader mapViewHeader = new MapViewHeader(mapView);

        JPanel rightPanel = new JPanel(new BorderLayout());
        rightPanel.add(mapViewHeader, BorderLayout.NORTH);
        rightPanel.add(mapViewScroller, BorderLayout.CENTER);
        rightPanel.add(mapViewMouseCoordinates, BorderLayout.SOUTH);

//        leftPanel.setMinimumSize(new Dimension(0, 0));
//        rightPanel.setMinimumSize(new Dimension(0, 0));
        JSplitPane rootPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, leftPanel, rightPanel);
        rootPanel.setContinuousLayout(true);
        //rootPanel.setDividerLocation(350);
        rootPanel.resetToPreferredSizes();
        add(rootPanel);
        setClosable(true);
        // add a window listener to confirm when the user closes the window
        setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        addInternalFrameListener(new InternalFrameAdapter() {

            @Override
            public void internalFrameClosing(InternalFrameEvent e) {
                if (checkCurrentFileSaved()) {
                    ((EditorDesktop)MapEditorWindow.this.getParent().getParent()).removeEditor(MapEditorWindow.this);
                    
                }
            }
        });

        // create the menu bar
        createMenuBar();

        setMap(null);

//	if (quest == null) {
//	    new ActionListenerLoadProject().actionPerformed(null);
//	}
//	else {
//	    try {
//	        Project.createExisting("../quests/zsdx");
//	    }
//	    catch (ZSDXException ex) {
//	        new ActionListenerLoadProject().actionPerformed(null);
//	    }
//	}
    }

    /**
     * Creates the menu bar and adds it to the window.
     */
    private void createMenuBar() {
        JMenuBar menuBar = new JMenuBar();

        JMenu menu;
        JMenuItem item;

        menuMap = new JMenu("Map");
        menuMap.setMnemonic(KeyEvent.VK_T);
        menuMap.setEnabled(true);

        menuBar.add(menuMap);

        menuItemClose = new JMenuItem("Close");
        menuItemClose.setMnemonic(KeyEvent.VK_C);
        menuItemClose.getAccessibleContext().setAccessibleDescription("Close the current map");
        menuItemClose.addActionListener(new ActionListenerClose());
        menuMap.add(menuItemClose);

        menuItemSave = new JMenuItem("Save");
        menuItemSave.setMnemonic(KeyEvent.VK_S);
        menuItemSave.getAccessibleContext().setAccessibleDescription("Save the current map");
        menuItemSave.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
        menuItemSave.addActionListener(new ActionListenerSave());
        menuMap.add(menuItemSave);

        // menu Edit
        menuEdit = new JMenu("Edit");
        menuEdit.setMnemonic(KeyEvent.VK_E);

        menuItemUndo = new JMenuItem("Undo");
        menuItemUndo.setMnemonic(KeyEvent.VK_U);
        menuItemUndo.getAccessibleContext().setAccessibleDescription("Undo the last action");
        menuItemUndo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Z, ActionEvent.CTRL_MASK));
        menuItemUndo.addActionListener(new ActionListenerUndo());
        menuItemUndo.setEnabled(false);
        menuEdit.add(menuItemUndo);

        menuItemRedo = new JMenuItem("Redo");
        menuItemRedo.setMnemonic(KeyEvent.VK_R);
        menuItemRedo.getAccessibleContext().setAccessibleDescription("Redo the last action undone");
        menuItemRedo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, ActionEvent.CTRL_MASK));
        menuItemRedo.addActionListener(new ActionListenerRedo());
        menuItemRedo.setEnabled(false);
        menuEdit.add(menuItemRedo);

        menuBar.add(menuEdit);

        //menu.addSeparator();

        menuItemCut = new JMenuItem("Cut");
        menuItemCut.setMnemonic(KeyEvent.VK_U);
        menuItemCut.getAccessibleContext().setAccessibleDescription("Cut the selected entities");
        menuItemCut.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, ActionEvent.CTRL_MASK));
        menuItemCut.addActionListener(new ActionListenerCut());
        menuItemCut.setEnabled(false);
        menuEdit.add(menuItemCut);

        menuItemCopy = new JMenuItem("Copy");
        menuItemCopy.setMnemonic(KeyEvent.VK_C);
        menuItemCopy.getAccessibleContext().setAccessibleDescription("Copy the selected entities");
        menuItemCopy.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, ActionEvent.CTRL_MASK));
        menuItemCopy.addActionListener(new ActionListenerCopy());
        menuItemCopy.setEnabled(false);
        menuEdit.add(menuItemCopy);

        menuItemPaste = new JMenuItem("Paste");
        menuItemPaste.setMnemonic(KeyEvent.VK_P);
        menuItemPaste.getAccessibleContext().setAccessibleDescription("Paste the copied entities");
        menuItemPaste.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V, ActionEvent.CTRL_MASK));
        menuItemPaste.addActionListener(new ActionListenerPaste());
        menuItemPaste.setEnabled(false);
        menuEdit.add(menuItemPaste);

        // menu Create
        menuCreate = new AddEntitiesMenu(mapView, "Create");
        menuCreate.setMnemonic(KeyEvent.VK_C);

        menuBar.add(menuCreate);

        setJMenuBar(menuBar);
    }

    /**
     * This method is called just after a project is loaded.
     */
    public void currentProjectChanged() {
        menuMap.setEnabled(true);

        if (map != null) {
            closeMap(); // close the map that was open with the previous project
        }
    }

    /**
     * Give the name of the resource opened in the editor
     * @return the name of the map
     */
    public String getResourceName() {
        return map.getName();
    }

    /**
     * Sets the current map. This method is called when the user opens a map,
     * closes the map, or creates a new one.
     * @param map the new map, or null if there is no map loaded
     */
    private void setMap(Map map) {
        // if there was already a map, remove its observers
        if (this.map != null) {
            this.map.deleteObservers();
            this.map.getEntitySelection().deleteObservers();
        }
        this.map = map;

        // enable or disable the menu items
        menuItemClose.setEnabled(map != null);
        menuItemSave.setEnabled(map != null);
        menuEdit.setEnabled(map != null);
        menuCreate.setEnabled(map != null);

        // notify the views
        mapPropertiesView.setMap(map);
        tilePicker.setMap(map);
        mapView.setMap(map);

        if (map != null) {
            // observe the history and the selection to enable or disable the items
            map.getHistory().addObserver(this);
            map.getEntitySelection().addObserver(this);
            update(null, null);
        }
    }

    /**
     * This function is called when the history changes.
     * @param o the history
     * @param obj additional parameter
     */
    public void update(Observable o, Object obj) {

        MapEditorHistory history = map.getHistory();

        menuItemSave.setEnabled(!history.isSaved());
        menuItemUndo.setEnabled(history.canUndo());
        menuItemRedo.setEnabled(history.canRedo());

        boolean emptySelection = map.getEntitySelection().isEmpty();
        menuItemCut.setEnabled(!emptySelection);
        menuItemCopy.setEnabled(!emptySelection);
        menuItemPaste.setEnabled(mapView.canPaste());
    }

    /**
     * This function is called when the user wants to close the current map.
     * If the map is not saved, we propose to save it.
     * @return false if the user cancelled
     */
    public boolean checkCurrentFileSaved() {
        boolean result = true;

        if (map != null && !map.getHistory().isSaved()) {
            int answer = JOptionPane.showConfirmDialog(this,
                    "The map has been modified. Do you want to save it?",
                    "Save the modifications",
                    JOptionPane.YES_NO_CANCEL_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            if (answer == JOptionPane.YES_OPTION) {
                new ActionListenerSave().actionPerformed(null);
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
    private void saveMap() {

        try {
            map.save();
        } catch (InvalidEntityException ex) {
            // the map contains an invalid entity
            GuiTools.warningDialog("Could not save the map: " + ex.getMessage() + " Click OK to edit the invalid entity now.");
            EditEntityDialog dialog = new EditEntityDialog(map, ex.getEntity());
            dialog.display();
        } catch (ZSDXException ex) {
            // another problem occured
            GuiTools.errorDialog("Could not save the map: " + ex.getMessage());
        }
    }

    /**
     * Action performed when the user clicks on Map > Close.
     * Closes the current map.
     */
    private class ActionListenerClose implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            if (!checkCurrentFileSaved()) {
            return;
        }
            ((EditorDesktop)MapEditorWindow.this.getParent().getParent()).removeEditor(MapEditorWindow.this);
        }
    }

    /**
     * Action performed when the user clicks on Map > Save.
     * Saves the map into its file.
     */
    private class ActionListenerSave implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            saveMap();
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Undo or presses Ctrl + Z.
     * The last action (if any) on the map is cancelled.
     */
    private class ActionListenerUndo implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            try {
                map.getHistory().undo();
            } catch (ZSDXException ex) {
                GuiTools.errorDialog("Cannot undo: " + ex.getMessage());
            }
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Redo or presses Ctrl + Y.
     * The last action cancelled (if any) is done again.
     */
    private class ActionListenerRedo implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            try {
                map.getHistory().redo();
            } catch (ZSDXException ex) {
                GuiTools.errorDialog("Cannot redo: " + ex.getMessage());
            }
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Cut or presses Ctrl + X.
     * The selected entities are cut.
     */
    private class ActionListenerCut implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            mapView.cutSelectedEntities();
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Copy or presses Ctrl + C.
     * The selected entities are copied.
     */
    private class ActionListenerCopy implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            mapView.copySelectedEntities();
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Paste or presses Ctrl + V.
     * The copied entities are added to the map.
     */
    private class ActionListenerPaste implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            mapView.paste();
        }
    }
}
