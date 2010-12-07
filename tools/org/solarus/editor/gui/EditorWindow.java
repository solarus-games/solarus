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

import java.awt.BorderLayout;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import java.util.Observable;
import java.util.Observer;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import org.solarus.editor.MapEditorHistory;
import org.solarus.editor.Project;
import org.solarus.editor.ProjectObserver;
import org.solarus.editor.ZSDXException;

/**
 * Main window of the editor.
 */
public class EditorWindow extends JFrame implements Observer, ProjectObserver, ChangeListener {

    private EditorDesktop desktop;
    private JMenu menuMap;
    private JMenuItem menuItemCloseMap;
    private JMenuItem menuItemSaveMap;
    private JMenuItem menuItemUndoMap;
    private JMenuItem menuItemRedoMap;
    private JMenuItem menuItemCutMap;
    private JMenuItem menuItemCopyMap;
    private JMenuItem menuItemPasteMap;
    // menus or menu items memorized to enable it later
    private JMenu menuTileset;
    private JMenuItem menuItemCloseTileset;
    private JMenuItem menuItemSaveTileset;
    /**
     * The quest to load in the editor, for use in the sub-editors
     */
    private String quest;

    /**
     * Creates a new window.
     * @param quest name of a quest to load, or null to show a dialog to select the quest
     */
    public EditorWindow(String quest) {
        super("Solarus - Editor");
        this.quest = quest;

        Project.addProjectObserver(this);
        // set a nice look and feel
        GuiTools.setLookAndFeel();

        desktop = new EditorDesktop();
        desktop.addChangeListener(this);
        getContentPane().add(desktop, BorderLayout.CENTER);

        // add a window listener to confirm when the user closes the window
        setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {

            public void windowClosing(WindowEvent e) {
                if (checkCurrentFilesSaved()) {
                    if (GuiTools.yesNoDialog("Do you really want to quit the editor ?")) {
                        System.exit(0);
                    }
                }
            }
        });

        // create the menu bar
        createMenuBar();
        update(null, null);
        if (quest == null) {
            new ActionListenerLoadProject().actionPerformed(null);
        } else {
            try {
                Project.createExisting("../quests/zsdx");
            } catch (ZSDXException ex) {
                new ActionListenerLoadProject().actionPerformed(null);
            }
        }
    }

    /**
     * This function is called when the history changes.
     * @param o the history
     * @param obj additional parameter
     */
    public void update(Observable o, Object obj) {
        if (desktop.getSelectedComponent() == null) {
            menuMap.setEnabled(false);
            menuTileset.setEnabled(false);
        } else {
            menuMap.setEnabled(true);
            menuTileset.setEnabled(true);
            try {

                MapEditorWindow mapEditor = (MapEditorWindow) desktop.getSelectedComponent();
                MapEditorHistory history = mapEditor.getMap().getHistory();
                menuItemCloseMap.setEnabled(true);
                menuItemSaveMap.setEnabled(!history.isSaved());
                menuItemUndoMap.setEnabled(history.canUndo());
                menuItemRedoMap.setEnabled(history.canRedo());

                boolean emptySelection = mapEditor.getMap().getEntitySelection().isEmpty();
                menuItemCutMap.setEnabled(!emptySelection);
                menuItemCopyMap.setEnabled(!emptySelection);
                menuItemPasteMap.setEnabled(mapEditor.getMapView().canPaste());
            } catch (ClassCastException cce) {
                menuItemCloseMap.setEnabled(false);
                menuItemSaveMap.setEnabled(false);
                menuItemUndoMap.setEnabled(false);
                menuItemRedoMap.setEnabled(false);
                menuItemCutMap.setEnabled(false);
                menuItemCopyMap.setEnabled(false);
                menuItemPasteMap.setEnabled(false);
            }
            try {

                TilesetEditorWindow tiilesetEditor = (TilesetEditorWindow) desktop.getSelectedComponent();
                menuItemCloseTileset.setEnabled(true);
                menuItemSaveTileset.setEnabled(true);
            } catch (ClassCastException cce) {
                menuItemCloseTileset.setEnabled(false);
                menuItemSaveTileset.setEnabled(false);
            }

        }
    }

    public void createMenuBar() {
        JMenuBar menuBar = new JMenuBar();

        JMenu menu;
        JMenuItem item;

        // menu Project
        menu = new JMenu("Project");
        menu.setMnemonic(KeyEvent.VK_P);

        item = new JMenuItem("New project...");
        item.setMnemonic(KeyEvent.VK_N);
        item.getAccessibleContext().setAccessibleDescription("Create a new ZSDX project");
        item.addActionListener(new ActionListenerNewProject());
        menu.add(item);

        item = new JMenuItem("Load project...");
        item.setMnemonic(KeyEvent.VK_O);
        item.getAccessibleContext().setAccessibleDescription("Open an existing ZSDX project");
        item.addActionListener(new ActionListenerLoadProject());
        menu.add(item);

        menu.addSeparator();

        item = new JMenuItem("Quit");
        item.setMnemonic(KeyEvent.VK_Q);
        item.getAccessibleContext().setAccessibleDescription("Teletransporter the tileset editor");
        item.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent ev) {
                if (checkCurrentFilesSaved()) {
                    if (GuiTools.yesNoDialog("Do you really want to quit the editor ?")) {
                        System.exit(0);
                    }
                }
            }
        });
        menu.add(item);

        menuBar.add(menu);

        // menu Map
        menuMap = new JMenu("Map");
        menuMap.setMnemonic(KeyEvent.VK_T);
        menuMap.setEnabled(false);

        item = new JMenuItem("New");
        item.setMnemonic(KeyEvent.VK_N);
        item.getAccessibleContext().setAccessibleDescription("Create a new map");
        item.addActionListener(new ActionListenerNewMap());
        menuMap.add(item);

        item = new JMenuItem("Open...");
        item.setMnemonic(KeyEvent.VK_O);
        item.getAccessibleContext().setAccessibleDescription("Open an existing map");
        item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
        item.addActionListener(new ActionListenerOpenMap());
        menuMap.add(item);

        menuItemCloseMap = new JMenuItem("Close");
        menuItemCloseMap.setMnemonic(KeyEvent.VK_C);
        menuItemCloseMap.getAccessibleContext().setAccessibleDescription("Close the current map");
        menuItemCloseMap.addActionListener(new ActionListenerCloseMap());
        menuMap.add(menuItemCloseMap);

        menuItemSaveMap = new JMenuItem("Save");
        menuItemSaveMap.setMnemonic(KeyEvent.VK_S);
        menuItemSaveMap.getAccessibleContext().setAccessibleDescription("Save the current map");
        menuItemSaveMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
        menuItemSaveMap.addActionListener(new ActionListenerSaveMap());
        menuMap.add(menuItemSaveMap);

        menuItemCloseMap.setEnabled(false);
        menuItemSaveMap.setEnabled(false);

        menuMap.addSeparator();

        // menu Edit
//        menuEdit = new JMenu("Edit");
//        menuEdit.setMnemonic(KeyEvent.VK_E);

        menuItemUndoMap = new JMenuItem("Undo");
        menuItemUndoMap.setMnemonic(KeyEvent.VK_U);
        menuItemUndoMap.getAccessibleContext().setAccessibleDescription("Undo the last action");
        menuItemUndoMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Z, ActionEvent.CTRL_MASK));
        menuItemUndoMap.addActionListener(new ActionListenerUndoMap());
        menuItemUndoMap.setEnabled(false);
        menuMap.add(menuItemUndoMap);

        menuItemRedoMap = new JMenuItem("Redo");
        menuItemRedoMap.setMnemonic(KeyEvent.VK_R);
        menuItemRedoMap.getAccessibleContext().setAccessibleDescription("Redo the last action undone");
        menuItemRedoMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, ActionEvent.CTRL_MASK));
        menuItemRedoMap.addActionListener(new ActionListenerRedoMap());
        menuItemRedoMap.setEnabled(false);
        menuMap.add(menuItemRedoMap);

        //menuBar.add(menuEdit);

        menuMap.addSeparator();

        menuItemCutMap = new JMenuItem("Cut");
        menuItemCutMap.setMnemonic(KeyEvent.VK_U);
        menuItemCutMap.getAccessibleContext().setAccessibleDescription("Cut the selected entities");
        menuItemCutMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, ActionEvent.CTRL_MASK));
        menuItemCutMap.addActionListener(new ActionListenerCutMap());
        menuItemCutMap.setEnabled(false);
        menuMap.add(menuItemCutMap);

        menuItemCopyMap = new JMenuItem("Copy");
        menuItemCopyMap.setMnemonic(KeyEvent.VK_C);
        menuItemCopyMap.getAccessibleContext().setAccessibleDescription("Copy the selected entities");
        menuItemCopyMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, ActionEvent.CTRL_MASK));
        menuItemCopyMap.addActionListener(new ActionListenerCopyMap());
        menuItemCopyMap.setEnabled(false);
        menuMap.add(menuItemCopyMap);

        menuItemPasteMap = new JMenuItem("Paste");
        menuItemPasteMap.setMnemonic(KeyEvent.VK_P);
        menuItemPasteMap.getAccessibleContext().setAccessibleDescription("Paste the copied entities");
        menuItemPasteMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V, ActionEvent.CTRL_MASK));
        menuItemPasteMap.addActionListener(new ActionListenerPasteMap());
        menuItemPasteMap.setEnabled(false);
        menuMap.add(menuItemPasteMap);

        menuBar.add(menuMap);

        // tileset menu
        // menu Tileset
        menuTileset = new JMenu("Tileset");
        menuTileset.setEnabled(false);
        menuTileset.setMnemonic(KeyEvent.VK_T);

        item = new JMenuItem("New");
        item.setMnemonic(KeyEvent.VK_N);
        item.getAccessibleContext().setAccessibleDescription("Create a new tileset");
        item.addActionListener(new ActionNewTileset());
        menuTileset.add(item);

        item = new JMenuItem("Open...");
        item.setMnemonic(KeyEvent.VK_O);
        item.getAccessibleContext().setAccessibleDescription("Open an existing tileset");
        item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
        item.addActionListener(new ActionOpenTileset());
        menuTileset.add(item);

        menuItemCloseTileset = new JMenuItem("Close");
        menuItemCloseTileset.setMnemonic(KeyEvent.VK_C);
        menuItemCloseTileset.getAccessibleContext().setAccessibleDescription("Close the current tileset");
        menuItemCloseTileset.addActionListener(new ActionCloseTileset());
        menuItemCloseTileset.setEnabled(false);
        menuTileset.add(menuItemCloseTileset);

        menuItemSaveTileset = new JMenuItem("Save");
        menuItemSaveTileset.setMnemonic(KeyEvent.VK_S);
        menuItemSaveTileset.getAccessibleContext().setAccessibleDescription("Save the current tileset");
        menuItemSaveTileset.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
        menuItemSaveTileset.addActionListener(new ActionSaveTileset());
        menuItemSaveTileset.setEnabled(false);
        menuTileset.add(menuItemSaveTileset);

        menuBar.add(menuTileset);




        setJMenuBar(menuBar);

    }

    /**
     * This function is called when the user wants to quit the editor
     * If any resource opened is not saved, we propose to save it.
     * @return false if the user cancelled
     */
    public boolean checkCurrentFilesSaved() {
        boolean result = true;
        if (desktop.countEditors() > 0) {
            for (AbstractEditorWindow editor : desktop.getEditors()) {
                result = result && editor.checkCurrentFileSaved();
            }
        }
        return result;
    }

    /**
     * This method is called just after a project is loaded.
     */
    public void currentProjectChanged() {
        menuMap.setEnabled(true);
        menuTileset.setEnabled(true);
    }

    /**
     * Prompts the user for a directory and creates a new project
     * in that directory.
     */
    private void newProject() {
        if (!checkCurrentFilesSaved()) {
            return;
        }

        ProjectFileChooser chooser = new ProjectFileChooser();
        String projectPath = chooser.getProjectPath();

        if (projectPath != null) {
            Project project = Project.createNew(projectPath);

            if (project == null) {
                GuiTools.warningDialog("A project already exists in this directory.");
            }
        }
    }

    /**
     * Prompts the user for a directory and loads the project
     * located in that directory.
     */
    private void loadProject() {
        if (desktop.countEditors() > 0) {
            for (AbstractEditorWindow editor : desktop.getEditors()) {
                if (editor.checkCurrentFileSaved()) {
                    return;
                }
            }
        }

        ProjectFileChooser chooser = new ProjectFileChooser();
        String projectPath = chooser.getProjectPath();

        if (projectPath != null) {
            try {
                Project project = Project.createExisting(projectPath);

                if (project == null) {
                    if (GuiTools.yesNoDialog("No project was found in this directory. Do you want to create a new one?")) {
                        Project.createNew(projectPath);

                        if (project == null) {
                            GuiTools.warningDialog("A project already exists in this directory.");
                        }
                    }
                }
            } catch (ZSDXException ex) {
                GuiTools.errorDialog("Cannot load the project: " + ex.getMessage());
            }
        }
    }

    /**
     * Action performed when the user clicks on Project > New project.
     * Creates a new project, asking to the user the project path.
     */
    private class ActionListenerNewProject implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            newProject();
        }
    }

    /**
     * Action performed when the user clicks on Project > Load project.
     * Loads an existing project, asking to the user the project path.
     */
    private class ActionListenerLoadProject implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            loadProject();
        }
    }

    /**
     * Action performed when the user clicks on Map > New.
     * Creates a new map, asking to the user the map name and the map file.
     */
    private class ActionListenerNewMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            MapEditorWindow mapEditor = new MapEditorWindow(EditorWindow.this.quest, EditorWindow.this);
            mapEditor.newMap();
            EditorWindow.this.desktop.addEditor(mapEditor);
            mapEditor.getMap().addObserver(EditorWindow.this);

        }
    }

    /**
     * Action performed when the user clicks on Map > Load.
     * Opens an existing map, asking to the user the map file.
     */
    private class ActionListenerOpenMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            MapEditorWindow mapEditor = new MapEditorWindow(EditorWindow.this.quest, EditorWindow.this);
            mapEditor.openMap();
            EditorWindow.this.desktop.addEditor(mapEditor);
            mapEditor.getMap().addObserver(EditorWindow.this);
        }
    }

    // Specific listeners for Map editor
    /**
     * Action performed when the user clicks on Map > Close.
     * Closes the current map.
     */
    private class ActionListenerCloseMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {

            MapEditorWindow mapEditor = (MapEditorWindow) desktop.getSelectedComponent();

            if (!mapEditor.checkCurrentFileSaved()) {
                return;
            }
            desktop.remove(mapEditor);
        }
    }

    /**
     * Action performed when the user clicks on Map > Save.
     * Saves the map into its file.
     */
    private class ActionListenerSaveMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            MapEditorWindow mapEditor = (MapEditorWindow) desktop.getSelectedComponent();
            mapEditor.saveMap();
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Undo or presses Ctrl + Z.
     * The last action (if any) on the map is cancelled.
     */
    private class ActionListenerUndoMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            MapEditorWindow mapEditor = (MapEditorWindow) desktop.getSelectedComponent();
            try {
                mapEditor.getMap().getHistory().undo();
            } catch (ZSDXException ex) {
                GuiTools.errorDialog("Cannot undo: " + ex.getMessage());
            }
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Redo or presses Ctrl + Y.
     * The last action cancelled (if any) is done again.
     */
    private class ActionListenerRedoMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            MapEditorWindow mapEditor = (MapEditorWindow) desktop.getSelectedComponent();
            try {
                mapEditor.getMap().getHistory().redo();
            } catch (ZSDXException ex) {
                GuiTools.errorDialog("Cannot redo: " + ex.getMessage());
            }
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Cut or presses Ctrl + X.
     * The selected entities are cut.
     */
    private class ActionListenerCutMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            MapEditorWindow mapEditor = (MapEditorWindow) desktop.getSelectedComponent();
            mapEditor.getMapView().cutSelectedEntities();
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Copy or presses Ctrl + C.
     * The selected entities are copied.
     */
    private class ActionListenerCopyMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            MapEditorWindow mapEditor = (MapEditorWindow) desktop.getSelectedComponent();
            mapEditor.getMapView().copySelectedEntities();
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Paste or presses Ctrl + V.
     * The copied entities are added to the map.
     */
    private class ActionListenerPasteMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            MapEditorWindow mapEditor = (MapEditorWindow) desktop.getSelectedComponent();
            mapEditor.getMapView().paste();
        }
    }
// Specific listeners for tileset editor

    /**
     * Action performed when the user clicks on Tileset > New.
     * Creates a new tileset, asking to the user the tileset name and the tileset file.
     */
    private class ActionNewTileset implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            TilesetEditorWindow tilesetEditor = new TilesetEditorWindow(EditorWindow.this.quest, EditorWindow.this);
            tilesetEditor.newTileset();
            EditorWindow.this.desktop.addEditor(tilesetEditor);
        }
    }

    /**
     * Action performed when the user clicks on Tileset > Load.
     * Opens an existing tileset, asking to the user the tileset name.
     */
    private class ActionOpenTileset implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            TilesetEditorWindow tilesetEditor = new TilesetEditorWindow(EditorWindow.this.quest, EditorWindow.this);
            tilesetEditor.openTileset();
            EditorWindow.this.desktop.addEditor(tilesetEditor);
        }
    }

    /**
     * Action performed when the user clicks on Tileset > Close.
     * Closes the current tileset.
     */
    private class ActionCloseTileset implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            TilesetEditorWindow tilesetEditor = (TilesetEditorWindow) desktop.getSelectedComponent();

            if (!tilesetEditor.checkCurrentFileSaved()) {
                return;
            }
            desktop.remove(tilesetEditor);


        }
    }

    /**
     * Action performed when the user clicks on Tileset > Save.
     * Saves the tileset into its file.
     */
    private class ActionSaveTileset implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            TilesetEditorWindow tilesetEditor = (TilesetEditorWindow) desktop.getSelectedComponent();
            tilesetEditor.saveTileset();
        }
    }

    public void stateChanged(ChangeEvent e) {
        update(null, null);
    }
}
