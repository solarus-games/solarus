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
import java.io.File;
import java.util.Observable;
import java.util.Observer;
import java.util.logging.Level;
import java.util.logging.Logger;
import javax.swing.JFileChooser;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;
import org.solarus.editor.Dialogs;
import org.solarus.editor.MapEditorHistory;
import org.solarus.editor.Project;
import org.solarus.editor.ProjectObserver;
import org.solarus.editor.ResourceType;
import org.solarus.editor.ZSDXException;

/**
 * Main window of the editor.
 */
public class EditorWindow extends JFrame implements Observer, ProjectObserver, ChangeListener {

    private static final String mapEditorClass = "org.solarus.editor.gui.MapEditorWindow";
    private static final String tilesetEditorClass = "org.solarus.editor.gui.TilesetEditorWindow";
    private static final String fileEditorClass = "org.solarus.editor.gui.FileEditorWindow";
    private EditorDesktop desktop;
    private JMenu menuFile;
    private JMenu menuNew;
    private JMenuItem menuNewMap;
    private JMenuItem menuNewTileset;
    private JMenu menuOpen;
    private JMenuItem menuOpenMap;
    private JMenuItem menuOpenTileset;
    private JMenuItem menuOpenIniFile;
    private JMenuItem menuOpenTextFile;
    private JMenuItem menuItemSave;
    private JMenuItem menuItemClose;
    private JMenuItem menuItemUndo;
    private JMenuItem menuItemRedo;
    private JMenuItem menuItemCut;
    private JMenuItem menuItemCopy;
    private JMenuItem menuItemPaste;
    // menus or menu items memorized to enable it later
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
                    //if (GuiTools.yesNoDialog("Do you really want to quit the editor ?")) {
                    System.exit(0);
                    //}
                }
            }
        });

//        getToolkit().addAWTEventListener(new ActionOnCurrentEditor(), AWTEvent.KEY_EVENT_MASK);

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

        menuFile.setEnabled(Project.isLoaded());

        if (desktop.getSelectedComponent() != null) {
            String editorClass = desktop.getSelectedComponent().getClass().getName();
            //System.out.println(c.getClass().getName());
            boolean isMapEditor = mapEditorClass.equals(editorClass);
            boolean isTilesetEditor = tilesetEditorClass.equals(editorClass);
            boolean isFileEditor = fileEditorClass.equals(editorClass);

            if (isMapEditor) {

                MapEditorWindow mapEditor = (MapEditorWindow) desktop.getSelectedComponent();
                MapEditorHistory history = mapEditor.getMap().getHistory();
                menuItemUndo.setEnabled(history.canUndo());
                menuItemRedo.setEnabled(history.canRedo());

                boolean emptySelection = mapEditor.getMap().getEntitySelection().isEmpty();
                menuItemCut.setEnabled(!emptySelection);
                menuItemCopy.setEnabled(!emptySelection);
                menuItemPaste.setEnabled(mapEditor.getMapView().canPaste());
            } else {
                // No resource opened interface the editor : only new/open actions are enabled

                menuItemUndo.setEnabled(false);
                menuItemRedo.setEnabled(false);
                menuItemCut.setEnabled(false);
                menuItemCopy.setEnabled(false);
                menuItemPaste.setEnabled(false);
            }
        }
    }

    public void createMenuBar() {
        JMenuBar menuBar = new JMenuBar();

        JMenu menu;
        JMenuItem item;

        // menu Project
        menu = new JMenu("Quest");
        menu.setMnemonic(KeyEvent.VK_Q);

        item = new JMenuItem("New quest...");
        item.setMnemonic(KeyEvent.VK_N);
        item.getAccessibleContext().setAccessibleDescription("Create a new ZSDX quest");
        item.addActionListener(new ActionListenerNewProject());
        menu.add(item);

        item = new JMenuItem("Load quest...");
        item.setMnemonic(KeyEvent.VK_O);
        item.getAccessibleContext().setAccessibleDescription("Open an existing ZSDX quest");
        item.addActionListener(new ActionListenerLoadProject());
        menu.add(item);

        menu.addSeparator();

        item = new JMenuItem("Quit");
        item.setMnemonic(KeyEvent.VK_Q);
        item.getAccessibleContext().setAccessibleDescription("Teletransporter the tileset editor");
        item.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent ev) {
                if (checkCurrentFilesSaved()) {
                    //if (GuiTools.yesNoDialog("Do you really want to quit the editor ?")) {
                    System.exit(0);
                    //}
                }
            }
        });
        menu.add(item);

        menuBar.add(menu);

        //MenuFile
        menuFile = new JMenu("File");
        menuFile.setEnabled(false);
        menuFile.setMnemonic(KeyEvent.VK_F);
        //New
        menuNew = new JMenu("New");
        menuNew.setMnemonic(KeyEvent.VK_N);
        // menu Map
        menuNewMap = new JMenuItem("Map");
        menuNewMap.setMnemonic(KeyEvent.VK_M);
        menuNewMap.getAccessibleContext().setAccessibleDescription("Create a new map");
        menuNewMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, ActionEvent.CTRL_MASK));
        menuNewMap.addActionListener(new ActionListenerNewMap());
        menuNew.add(menuNewMap);
        // menu Tileset
        menuNewTileset = new JMenuItem("Tileset");
        menuNewTileset.setMnemonic(KeyEvent.VK_T);
        menuNewTileset.getAccessibleContext().setAccessibleDescription("Create a new tileset");
        menuNewTileset.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, ActionEvent.ALT_MASK));
        menuNewTileset.addActionListener(new ActionNewTileset());
        menuNew.add(menuNewTileset);
        // menu TextFile
        // incoming
        menuFile.add(menuNew);

        // menu open
        menuOpen = new JMenu("Open");
        menuOpen.setMnemonic(KeyEvent.VK_O);
        menuOpen.addActionListener(new ActionListenerOpenMap());

        menuOpenMap = new JMenuItem("Map");
        menuOpenMap.setMnemonic(KeyEvent.VK_M);
        menuOpenMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
        menuOpenMap.addActionListener(new ActionListenerOpenMap());
        menuOpen.add(menuOpenMap);

        menuOpenTileset = new JMenuItem("Tileset");
        menuOpenTileset.setMnemonic(KeyEvent.VK_T);
        menuOpenTileset.getAccessibleContext().setAccessibleDescription("Open an existing tileset");
        menuOpenTileset.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.ALT_MASK));
        menuOpenTileset.addActionListener(new ActionOpenTileset());
        menuOpen.add(menuOpenTileset);

        menuOpenIniFile = new JMenuItem("Dialogs");
        menuOpenIniFile.setMnemonic(KeyEvent.VK_D);
        menuOpenIniFile.getAccessibleContext().setAccessibleDescription("Open an existing dialogs file");
        menuOpenIniFile.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.SHIFT_MASK));
        menuOpenIniFile.addActionListener(new ActionOpenIniFile());
        menuOpen.add(menuOpenIniFile);

        menuOpenTextFile = new JMenuItem("Text File");
        menuOpenTextFile.setMnemonic(KeyEvent.VK_F);
        menuOpenTextFile.getAccessibleContext().setAccessibleDescription("Open an existing file");
        //menuOpenTextFile.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.SHIFT_MASK));
        menuOpenTextFile.addActionListener(new ActionOpenFile());
        menuOpen.add(menuOpenTextFile);

        menuFile.add(menuOpen);

        menuItemClose = new JMenuItem("Close");
        menuItemClose.setMnemonic(KeyEvent.VK_C);
        menuItemClose.getAccessibleContext().setAccessibleDescription("Close the current editor");
        menuItemClose.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W, ActionEvent.CTRL_MASK));
        menuItemClose.addActionListener(new ActionCloseCurrentEditor());
        menuFile.add(menuItemClose);

        menuItemSave = new JMenuItem("Save");
        menuItemSave.setMnemonic(KeyEvent.VK_S);
        menuItemSave.getAccessibleContext().setAccessibleDescription("Save the current editor");
        menuItemSave.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
        menuItemSave.addActionListener(new ActionSaveCurrentEditor());
        menuFile.add(menuItemSave);

        menuFile.addSeparator();

        menuItemUndo = new JMenuItem("Undo");
        menuItemUndo.setMnemonic(KeyEvent.VK_U);
        menuItemUndo.getAccessibleContext().setAccessibleDescription("Undo the last action");
        menuItemUndo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Z, ActionEvent.CTRL_MASK));
        menuItemUndo.addActionListener(new ActionListenerUndoMap());
        menuItemUndo.setEnabled(false);
        menuFile.add(menuItemUndo);

        menuItemRedo = new JMenuItem("Redo");
        menuItemRedo.setMnemonic(KeyEvent.VK_R);
        menuItemRedo.getAccessibleContext().setAccessibleDescription("Redo the last action undone");
        menuItemRedo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, ActionEvent.CTRL_MASK));
        menuItemRedo.addActionListener(new ActionListenerRedoMap());
        menuItemRedo.setEnabled(false);
        menuFile.add(menuItemRedo);

        //menuBar.add(menuEdit);

        menuFile.addSeparator();

        menuItemCut = new JMenuItem("Cut");
        menuItemCut.setMnemonic(KeyEvent.VK_U);
        menuItemCut.getAccessibleContext().setAccessibleDescription("Cut the selected entities");
        menuItemCut.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_X, ActionEvent.CTRL_MASK));
        menuItemCut.addActionListener(new ActionListenerCutMap());
        menuItemCut.setEnabled(false);
        menuFile.add(menuItemCut);

        menuItemCopy = new JMenuItem("Copy");
        menuItemCopy.setMnemonic(KeyEvent.VK_C);
        menuItemCopy.getAccessibleContext().setAccessibleDescription("Copy the selected entities");
        menuItemCopy.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_C, ActionEvent.CTRL_MASK));
        menuItemCopy.addActionListener(new ActionListenerCopyMap());
        menuItemCopy.setEnabled(false);
        menuFile.add(menuItemCopy);

        menuItemPaste = new JMenuItem("Paste");
        menuItemPaste.setMnemonic(KeyEvent.VK_P);
        menuItemPaste.getAccessibleContext().setAccessibleDescription("Paste the copied entities");
        menuItemPaste.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_V, ActionEvent.CTRL_MASK));
        menuItemPaste.addActionListener(new ActionListenerPasteMap());
        menuItemPaste.setEnabled(false);
        menuFile.add(menuItemPaste);

        menuBar.add(menuFile);

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
        menuFile.setEnabled(true);
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
            if (mapEditor.getMap() != null) {
                EditorWindow.this.desktop.addEditor(mapEditor);
                mapEditor.getMap().addObserver(EditorWindow.this);
            }
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
            if (tilesetEditor.getTileset() == null) {
                return;//user cancellation
            }
            EditorWindow.this.desktop.addEditor(tilesetEditor);
        }
    }

    /**
     * Action performed when the user clicks on File > Load.
     * Opens an existing file
     */
    private class ActionOpenIniFile implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            DialogsEditorWindow dialogsEditor = new DialogsEditorWindow(EditorWindow.this.quest, EditorWindow.this);
            dialogsEditor.openDialogs();
            if (dialogsEditor.getDialogs() == null) {
                return;//user cancellation
            }
            EditorWindow.this.desktop.addEditor(dialogsEditor);

        }
    }

    /**
     * Action performed when the user clicks on File > Load.
     * Opens an existing file
     */
    private class ActionOpenFile implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            FileEditorWindow fileEditor = new FileEditorWindow(EditorWindow.this.quest, EditorWindow.this);
            JFileChooser jfc = new JFileChooser(Project.getRootPath() + File.separator + "data");
            jfc.showOpenDialog(EditorWindow.this);
            File selectedFile = jfc.getSelectedFile();
            if (selectedFile != null) {
                fileEditor.setFile(selectedFile);
                EditorWindow.this.desktop.addEditor(fileEditor);
            }

        }
    }

    /**
     * Action performed when the user clicks on File > Close.
     * Closes the current file.
     */
    private class ActionCloseCurrentEditor implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            desktop.removeCurrentEditor();
        }
    }

    /**
     * Action performed when the user clicks on Tileset > Save.
     * Saves the tileset into its file.
     */
    private class ActionSaveCurrentEditor implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            desktop.saveCurrentEditor();
        }
    }

//    /**
//     * Action performed when the user uses the Ctrl-W or Ctrl-S keys
//     * Close or save the current editor
//     */
//    private class ActionOnCurrentEditor implements AWTEventListener {
//
//        public void eventDispatched(AWTEvent ev) {
//            if (ev instanceof KeyEvent) {
//                KeyEvent kev = (KeyEvent) ev;
//                System.out.println("Touche appuyée depuis " + this.getClass().getName());
//                int code = kev.getKeyCode();
//                if (kev.isControlDown()) {
//                    if (code == KeyEvent.VK_W) {
//                        desktop.removeCurrentEditor();
//                    } else if (code == KeyEvent.VK_S) {
//                        desktop.saveCurrentEditor();
//                    }
//                }
//            }
//        }
//    }
    public void stateChanged(ChangeEvent e) {
        update(null, null);
    }
}
