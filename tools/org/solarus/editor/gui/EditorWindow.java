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
import java.awt.event.*;
import java.io.*;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import org.jdesktop.swinghelper.transformer.*;
import org.solarus.editor.*;

/**
 * Main window of the editor.
 */
public class EditorWindow extends JFrame
    implements Observer, ProjectObserver, ChangeListener {

    private EditorTabs tabs;
    private QuestTree questTree;

    // Menus and menu items memorized to enable or disable them later.
    private JMenu menuFile;
    private JMenu menuNew;
    private JMenuItem menuNewMap;
    private JMenuItem menuNewTileset;
    private JMenu menuOpen;
    private JMenuItem menuOpenMap;
    private JMenuItem menuOpenTileset;
    private JMenuItem menuItemSave;
    private JMenuItem menuItemClose;
    private JMenuItem menuItemUndo;
    private JMenuItem menuItemRedo;
    private JMenuItem menuItemCut;
    private JMenuItem menuItemCopy;
    private JMenuItem menuItemPaste;

    /**
     * Creates a new window.
     * @param questPath Path of a quest to load,
     * or null to show a dialog to select the quest.
     */
    public EditorWindow(String questPath) {
        super("Solarus Quest Editor");

        Project.addProjectObserver(this);
        // set a nice look and feel
        GuiTools.setLookAndFeel();

        tabs = new EditorTabs();
        tabs.addChangeListener(this);

        questTree = new QuestTree(this);
        questTree.setVisible(true);
        final JScrollPane jsp = new JScrollPane(questTree);
        jsp.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        jsp.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
        final JSplitPane splitPane = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, jsp, tabs);
        splitPane.setDividerLocation(0);
        jsp.setVisible(false);
        JToolBar toolBar = new JToolBar(JToolBar.HORIZONTAL);
        toolBar.setFloatable(false);
        JToggleButton treeButton = new JToggleButton("Quest Data");
        treeButton.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent e) {
                jsp.setVisible(!jsp.isVisible());
                if (jsp.isVisible()) {
                    splitPane.setDividerLocation(300);
                } else {
                    splitPane.setDividerLocation(0);
                }
            }
        });

        JXTransformer t = new JXTransformer(treeButton);
        t.rotate(- Math.PI / 2);
        toolBar.add(t);
        JPanel pToolBar = new JPanel(new BorderLayout());
        pToolBar.add(toolBar, BorderLayout.NORTH);
        getContentPane().add(pToolBar, BorderLayout.WEST);
        getContentPane().add(splitPane, BorderLayout.CENTER);

        // add a window listener to confirm when the user closes the window
        setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {

            @Override
            public void windowClosing(WindowEvent e) {
                if (checkCurrentFilesSaved()) {
                    System.exit(0);
                }
            }
        });

        // create the menu bar
        createMenuBar();
        update(null, null);
        if (questPath == null) {
            new ActionListenerLoadProject().actionPerformed(null);
        } else {
            try {
                Project.createExisting(questPath);
            } catch (QuestEditorException ex) {
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

        if (tabs.getSelectedComponent() != null) {

            menuItemUndo.setEnabled(false);
            menuItemRedo.setEnabled(false);
            menuItemCut.setEnabled(false);
            menuItemCopy.setEnabled(false);
            menuItemPaste.setEnabled(false);

            if (tabs.getSelectedComponent() instanceof MapEditorPanel) {

                MapEditorPanel mapEditor = (MapEditorPanel) tabs.getSelectedComponent();
                MapEditorHistory history = mapEditor.getMap().getHistory();
                menuItemUndo.setEnabled(history.canUndo());
                menuItemRedo.setEnabled(history.canRedo());

                boolean emptySelection = mapEditor.getMap().getEntitySelection().isEmpty();
                menuItemCut.setEnabled(!emptySelection);
                menuItemCopy.setEnabled(!emptySelection);
                menuItemPaste.setEnabled(mapEditor.getMapView().canPaste());
            }
        }
    }

    /**
     * Buils the menu bar of the window.
     */
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
        item.addActionListener(new ActionListenerQuit());
        menu.add(item);

        menuBar.add(menu);

        // Menu File
        menuFile = new JMenu("File");
        menuFile.setEnabled(false);
        menuFile.setMnemonic(KeyEvent.VK_F);
        // Menu File > New
        menuNew = new JMenu("New");
        menuNew.setMnemonic(KeyEvent.VK_N);
        // Item File > New > Map
        menuNewMap = new JMenuItem("Map");
        menuNewMap.setMnemonic(KeyEvent.VK_M);
        menuNewMap.getAccessibleContext().setAccessibleDescription("Create a new map");
        menuNewMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_N, ActionEvent.CTRL_MASK));
        menuNewMap.addActionListener(
                new ActionListenerCreateResourceElement(ResourceType.MAP));
        menuNew.add(menuNewMap);

        // Item File > New > Tileset
        menuNewTileset = new JMenuItem("Tileset");
        menuNewTileset.setMnemonic(KeyEvent.VK_T);
        menuNewTileset.getAccessibleContext().setAccessibleDescription("Create a new tileset");
        menuNewTileset.addActionListener(
                new ActionListenerCreateResourceElement(ResourceType.TILESET));
        menuNew.add(menuNewTileset);
        menuFile.add(menuNew);

        // Menu File > Open
        menuOpen = new JMenu("Open");
        menuOpen.setMnemonic(KeyEvent.VK_O);

        menuOpenMap = new JMenuItem("Map");
        menuOpenMap.setMnemonic(KeyEvent.VK_M);
        menuOpenMap.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
        menuOpenMap.addActionListener(
                new ActionListenerOpenResourceElement(ResourceType.MAP));
        menuOpen.add(menuOpenMap);

        menuOpenTileset = new JMenuItem("Tileset");
        menuOpenTileset.setMnemonic(KeyEvent.VK_T);
        menuOpenTileset.getAccessibleContext().setAccessibleDescription("Open an existing tileset");
        menuOpenTileset.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.ALT_MASK));
        menuOpenTileset.addActionListener(
                new ActionListenerOpenResourceElement(ResourceType.TILESET));
        menuOpen.add(menuOpenTileset);

        menuFile.add(menuOpen);

        menuItemClose = new JMenuItem("Close");
        menuItemClose.setMnemonic(KeyEvent.VK_C);
        menuItemClose.getAccessibleContext().setAccessibleDescription("Close the current editor");
        menuItemClose.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_W, ActionEvent.CTRL_MASK));
        menuItemClose.addActionListener(new ActionListenerCloseCurrentEditor());
        menuFile.add(menuItemClose);

        menuItemSave = new JMenuItem("Save");
        menuItemSave.setMnemonic(KeyEvent.VK_S);
        menuItemSave.getAccessibleContext().setAccessibleDescription("Save the current editor");
        menuItemSave.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
        menuItemSave.addActionListener(new ActionListenerSaveCurrentEditor());
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
     * If any resource open is not saved, we propose to save it.
     * @return false if the user canceled.
     */
    public boolean checkCurrentFilesSaved() {
        boolean result = true;
        if (tabs.countEditors() > 0) {
            for (AbstractEditorPanel editor : tabs.getEditors()) {
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
            try {
                Project.createNew(projectPath);
                setTitle("Solarus Editor - " + projectPath.substring(projectPath.lastIndexOf(File.separator) + 1));
                GuiTools.informationDialog("Quest successfully created!\n" +
                		"The next step is to edit some important files\n" +
                        "(sorry, their edition is not fully supported by this editor yet):\n" +
                		"quest.dat, text/fonts.dat, languages/languages.dat");
            }
            catch (QuestEditorException ex) {
                GuiTools.errorDialog("Cannot create the project: " + ex.getMessage());
            }
        }
    }

    /**
     * Prompts the user for a directory and loads the project
     * located in that directory.
     */
    private void loadProject() {

        if (!checkCurrentFilesSaved()) {
            return;
        }

        ProjectFileChooser chooser = new ProjectFileChooser();
        String projectPath = chooser.getProjectPath();

        if (projectPath != null) {
            try {
                Project.createExisting(projectPath);
                setTitle("Solarus Editor - " + projectPath.substring(projectPath.lastIndexOf(File.separator) + 1));
            }
            catch (QuestEditorException ex) {
                GuiTools.errorDialog("Cannot load the project: " + ex.getMessage());
            }
        }
    }

    /**
     * Action performed when the user clicks on Project > New project.
     * Creates a new project, asking to the user the project path.
     */
    private class ActionListenerNewProject implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            newProject();
        }
    }

    /**
     * Action performed when the user clicks on Project > Load project.
     * Loads an existing project, asking to the user the project path.
     */
    private class ActionListenerLoadProject implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            loadProject();
        }
    }

    /**
     * Action performed when the user wants to exit the program.
     */
    public class ActionListenerQuit implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            if (checkCurrentFilesSaved()) {
                System.exit(0);
            }
        }
    }

    /**
     * Action performed when the user wants to create a new resource element.
     */
    public class ActionListenerCreateResourceElement implements ActionListener {

        private ResourceType resourceType;

        public ActionListenerCreateResourceElement(ResourceType resourceType) {
            this.resourceType = resourceType;
        }

        @Override
        public void actionPerformed(ActionEvent ev) {
            createResourceElement(resourceType);
        }
    }

    /**
     * Action performed when the user wants to open a resource element.
     */
    public class ActionListenerOpenResourceElement implements ActionListener {

        private ResourceType resourceType;

        public ActionListenerOpenResourceElement(ResourceType resourceType) {
            this.resourceType = resourceType;
        }

        @Override
        public void actionPerformed(ActionEvent ev) {
            openResourceElement(resourceType);
        }
    }

    /**
     * Action performed when the user clicks on File > Close.
     * Closes the current editor unless the user is not okay with that..
     */
    private class ActionListenerCloseCurrentEditor implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            tabs.removeCurrentEditor();
        }
    }

    /**
     * Action performed when the user clicks on File > Save.
     * Saves the resource in this editor.
     */
    private class ActionListenerSaveCurrentEditor implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            tabs.saveCurrentEditor();
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Undo or presses Ctrl + Z.
     * The last action (if any) on the map is canceled.
     */
    private class ActionListenerUndoMap implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            MapEditorPanel mapEditor = (MapEditorPanel) tabs.getSelectedComponent();
            try {
                mapEditor.getMap().getHistory().undo();
            } catch (QuestEditorException ex) {
                GuiTools.errorDialog("Cannot undo: " + ex.getMessage());
            }
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Redo or presses Ctrl + Y.
     * The last action canceled (if any) is done again.
     */
    private class ActionListenerRedoMap implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            MapEditorPanel mapEditor = (MapEditorPanel) tabs.getSelectedComponent();
            try {
                mapEditor.getMap().getHistory().redo();
            } catch (QuestEditorException ex) {
                GuiTools.errorDialog("Cannot redo: " + ex.getMessage());
            }
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Cut or presses Ctrl + X.
     * The selected entities are cut.
     */
    private class ActionListenerCutMap implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            MapEditorPanel mapEditor = (MapEditorPanel) tabs.getSelectedComponent();
            mapEditor.getMapView().cutSelectedEntities();
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Copy or presses Ctrl + C.
     * The selected entities are copied.
     */
    private class ActionListenerCopyMap implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            MapEditorPanel mapEditor = (MapEditorPanel) tabs.getSelectedComponent();
            mapEditor.getMapView().copySelectedEntities();
        }
    }

    /**
     * Action performed when user the user clicks on Edit > Paste or presses Ctrl + V.
     * The copied entities are added to the map.
     */
    private class ActionListenerPasteMap implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            MapEditorPanel mapEditor = (MapEditorPanel) tabs.getSelectedComponent();
            mapEditor.getMapView().paste();
        }
    }

    /**
     * Called when the state of the tabs has changed.
     */
    @Override
    public void stateChanged(ChangeEvent e) {
        update(null, null);
    }

    /**
     * Creates a resource element, asking its id to the user.
     * @param resourceType Type of resource element to create.
     */
    public void createResourceElement(ResourceType resourceType) {

        String resourceName = resourceType.getName();
        String resourceNameLower = resourceName.toLowerCase();
        try {
            String id = JOptionPane.showInputDialog(
                    null,
                    "Please enter the id of your new " + resourceNameLower,
                    resourceName + " id",
                    JOptionPane.QUESTION_MESSAGE);

            if (id != null) {
                if (Project.getResource(resourceType).exists(id)) {
                    throw new QuestEditorException(
                            resourceName + " '" + id + "' already exists");
                }
                Project.newElement(resourceType, id);
            }
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Cannot create " + resourceName + ": " + ex.getMessage());
        }
    }

    /**
     * Opens a resource element, asking its id to the user.
     * @param resourceType Type of resource.
     */
    public void openResourceElement(ResourceType resourceType) {

        String resourceName = resourceType.getName();
        String resourceNameLower = resourceName.toLowerCase();

        ResourceChooserDialog dialog = new ResourceChooserDialog(resourceType);
        dialog.setLocationRelativeTo(EditorWindow.this);
        dialog.pack();
        dialog.setVisible(true);
        String id = dialog.getSelectedId();

        if (id.isEmpty()) {
            return;
        }

        try {
            if (!Project.getResource(resourceType).exists(id)) {
                throw new MapException(
                        "No such " + resourceNameLower + ": '" + id + "'");
            }
            openResourceElement(resourceType, id);
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog(
                    "Could not load the " + resourceNameLower + ": " + ex.getMessage());
        }
    }

    /**
     * Opens a resource element with its default editor.
     * @param resourceType Type of resource.
     * @param resourceId Id of the new element.
     */
    public void openResourceElement(ResourceType resourceType, String resourceId) {

        try {
            AbstractEditorPanel existingEditor;
            switch (resourceType) {

            case MAP:
            {
                existingEditor = tabs.getEditor(MapEditorPanel.getEditorId(resourceId));
                if (existingEditor != null) {
                    tabs.setSelectedComponent(existingEditor);
                }
                else {
                    MapEditorPanel mapEditor = new MapEditorPanel(this, resourceId);
                    tabs.addEditor(mapEditor);
                    // Keep menu items Undo, Cut, Copy, etc. synchronized.
                    mapEditor.getMap().addObserver(this);
                }
                break;
            }

            case TILESET:
            {
                existingEditor = tabs.getEditor(TilesetEditorPanel.getEditorId(resourceId));
                if (existingEditor != null) {
                    tabs.setSelectedComponent(existingEditor);
                }
                else {
                    TilesetEditorPanel tilesetEditor = new TilesetEditorPanel(this, resourceId);
                    tabs.addEditor(tilesetEditor);
                }
                break;
            }

            case LANGUAGE:
            {
                openTextEditor(Project.getDialogsFile(resourceId));
                break;
            }

            case ENEMY:
            {
                openTextEditor(Project.getEnemyScriptFile(resourceId));
                break;
            }

            case ITEM:
            {
                openTextEditor(Project.getItemScriptFile(resourceId));
                break;
            }

            case SPRITE:
            {
                openTextEditor(Project.getSpriteFile(resourceId));
                break;
            }
            }
        }
        catch (QuestEditorException ex) {
            GuiTools.errorDialog(
                    "Could not open " + resourceType.getName().toLowerCase()
                    + ": " + ex.getMessage());
        }
    }

    /**
     * Opens a file in a text editor.
     * @param file File to open.
     */
    public void openTextEditor(File file) {

        AbstractEditorPanel existingEditor =
                tabs.getEditor(TextEditorPanel.getEditorId(file));
        if (existingEditor != null) {
            tabs.setSelectedComponent(existingEditor);
        }
        else {
            TextEditorPanel textEditor = new TextEditorPanel(this, file);
            tabs.addEditor(textEditor);
        }
    }

    /**
     * Closes the editor of an element if it is open.
     * Lets the use save the element if he wants.
     * @param resourceType Type of resource element to close.
     * @param resourceId Id of the resource element to close.
     */
    public void closeResourceElement(ResourceType resourceType, String resourceId) {

        // Determine ids of editors to remove.
        ArrayList<String> editorIds = new ArrayList<String>();

        switch (resourceType) {

        case MAP:
        {
            editorIds.add(MapEditorPanel.getEditorId(resourceId);
            editorIds.add(TextEditorPanel.getEditorId(Project.getMapScriptFile(resourceId));
            break;
        }

        case TILESET:
        {
            editorIds.add(TilesetEditorPanel.getEditorId(resourceId));
            break;
        }

        case LANGUAGE:
        {
            editorIds.add(TextEditorPanel.getEditorId(Project.getDialogsFile(resourceId));
            break;
        }

        case ENEMY:
        {
            editorIds.add(TextEditorPanel.getEditorId(Project.getEnemyScript(resourceId));
            break;
        }

        case ITEM:
        {
            break;
        }

        case SPRITE:
        {
            break;
        }
        }
    }

    /**
     * Deletes a resource element after confirmation from the user.
     * @param resourceType Type of resource.
     * @param id Id of the element to delete.
     */
    public void deleteResourceElement(ResourceType resourceType, String id) {
        try {
            int answer = JOptionPane.showConfirmDialog(this,
                    "Are you sure you want to delete "
                            + resourceType.getName() + " '" + id + "'?",
                    "Are you sure?",
                    JOptionPane.YES_NO_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            if (answer == JOptionPane.YES_OPTION) {
                Project.deleteElement(resourceType, id);
            }
        }
        catch (QuestEditorException ex) {
            GuiTools.errorDialog("Could not delete " + resourceType.getName()
                    + ": " + ex.getMessage());
        }
    }

    /**
     * Called when a new resource element has just been created.
     * @param resourceType Type of resource.
     * @param id Id of the new element.
     */
    @Override
    public void resourceElementAdded(ResourceType resourceType, String id) {
        openResourceElement(resourceType, id);
    }

    /**
     * Called when a new resource element has just been deleted.
     * @param resourceType Type of resource.
     * @param id Id of the deleted element.
     */
    @Override
    public void resourceElementRemoved(ResourceType resourceType, String id) {
        // Close the editor if any.
        closeResourceElement(resourceType, id);
    }

    /**
     * Called when a resource element has just been renamed.
     * @param resourceType Type of resource.
     * @param id Id of the element.
     * @param name New human-readable name of the element.
     */
    @Override
    public void resourceElementMoved(ResourceType resourceType, String oldId,
            String newId) {
        // TODO Auto-generated method stub
        closeResourceElement(resourceType, oldId);
    }

    /**
     * Called when a resource element has just been renamed.
     * @param resourceType Type of resource.
     * @param id Id of the element.
     * @param name New human-readable name of the element.
     */
    @Override
    public void resourceElementRenamed(ResourceType resourceType,
            String id, String name) {
        tabs.repaint();  // TODO don't use repaint for this.
    }
}
