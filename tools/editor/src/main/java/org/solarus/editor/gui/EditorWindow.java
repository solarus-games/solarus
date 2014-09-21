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
import java.awt.event.*;
import java.io.*;
import java.nio.file.NoSuchFileException;
import java.util.*;

import javax.swing.*;
import javax.swing.event.*;

import org.solarus.editor.*;
import org.solarus.editor.Map;
import org.solarus.editor.gui.tree.QuestTree;

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
     * or null to load the default quest if any.
     */
    public EditorWindow(String questPath) {
        super("Solarus Quest Editor " + Project.solarusFormat);

        // set a nice look and feel
        GuiTools.setLookAndFeel();

        tabs = new EditorTabs();
        tabs.addChangeListener(this);

        questTree = new QuestTree(this);
        questTree.setVisible(true);
        final JScrollPane questTreeScroller = new JScrollPane(questTree);
        questTreeScroller.setVerticalScrollBarPolicy(JScrollPane.VERTICAL_SCROLLBAR_ALWAYS);
        questTreeScroller.setHorizontalScrollBarPolicy(JScrollPane.HORIZONTAL_SCROLLBAR_ALWAYS);
        final JSplitPane mainSplitter = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, true, questTreeScroller, tabs);
        mainSplitter.setOneTouchExpandable(true);
        mainSplitter.setDividerLocation(300);

        getContentPane().add(mainSplitter, BorderLayout.CENTER);

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
        if (questPath != null) {
            loadProject(questPath);
        }

        // This object must be added as project observer after the quest tree.
        // This is because we want the quest tree to be up-to-date before we
        // build or destroy subtrees of maps.
        Project.addProjectObserver(this);
    }

    /**
     * Returns the quest tree displayed in the left part of the window.
     * @return The quest tree.
     */
    public QuestTree getQuestTree() {
        return questTree;
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
        item.getAccessibleContext().setAccessibleDescription("Create a new Solarus quest");
        item.addActionListener(new ActionListenerNewProject());
        menu.add(item);

        item = new JMenuItem("Load quest...");
        item.setMnemonic(KeyEvent.VK_O);
        item.getAccessibleContext().setAccessibleDescription("Open an existing Solarus quest");
        item.addActionListener(new ActionListenerLoadProject());
        menu.add(item);

        menu.addSeparator();

        item = new JMenuItem("Quit");
        item.setMnemonic(KeyEvent.VK_Q);
        item.getAccessibleContext().setAccessibleDescription("Exit Solarus Quest Editor");
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
        menuOpenMap.addActionListener(
                new ActionListenerOpenResourceElement(ResourceType.MAP));
        menuOpen.add(menuOpenMap);

        menuOpenTileset = new JMenuItem("Tileset");
        menuOpenTileset.setMnemonic(KeyEvent.VK_T);
        menuOpenTileset.getAccessibleContext().setAccessibleDescription("Open an existing tileset");
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
     * Sets the title of the window.
     */
    private void updateWindowTitle() {
        setTitle(Project.getPathBaseName() + " - Solarus Quest Editor "
                + Project.solarusFormat);
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
        tabs.removeAll();
        updateWindowTitle();
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
                GuiTools.informationDialog("Quest successfully created!\n" +
                    "The next step is to manually edit your quest properties in quest.dat\n" +
                    "(sorry, this is not fully supported by the editor yet).\n");
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
        String questPath = chooser.getProjectPath();

        if (questPath != null) {
            loadProject(questPath);
        }
    }

    /**
     * Loads the project located in the specified directory.
     * @param questPath Path of the quest to load.
     */
    private void loadProject(String questPath) {

        if (!checkCurrentFilesSaved()) {
            return;
        }

        try {
            tabs.removeAll();
            Project.createExisting(questPath);
        }
        catch (ObsoleteQuestException ex) {
            // Quest data files are obsolete: upgrade them and try again.
            boolean upgrade = GuiTools.okCancelDialog(
                "The format of this quest (" + ex.getQuestFormat()
                + ") is outdated.\n"
                + "Your data files will be automatically updated to Solarus "
                + Project.solarusFormat + ".");
            if (upgrade && upgradeProject(questPath, ex.getQuestFormat())) {
                loadProject(questPath);
            }
        }
        catch (ObsoleteEditorException ex) {
            GuiTools.errorDialog(ex.getMessage());
        }
        catch (QuestEditorException ex) {
            GuiTools.errorDialog("Cannot load the project: " + ex.getMessage());
        }
    }

    /**
     * Upgrades the data files of the specified project to the most recent
     * format.
     * A dialog box is shown to display the status of the operation.
     * @param questPath Path of the quest to upgrade.
     * @param questFormat The (obsolete) format of the quest.
     * @return true in case of success.
     */
    private boolean upgradeProject(String questPath, String questFormat) {

        try {
            // First backup the files.
            String backupDirectory = questPath + "/data." + questFormat + ".bak";
            FileTools.deleteRecursive(backupDirectory);  // Remove any previous backup.
            FileTools.copyDirectory(questPath + "/data", backupDirectory);

            // Upgrade data files.
            ExternalLuaScriptDialog dialog = new ExternalLuaScriptDialog("Upgrading quest data files",
                "update_quest", questPath);
            boolean upgradeSuccess = dialog.display();
            if (!upgradeSuccess) {
                // The upgrade failed.
                try {
                    // Restore the backuped version.
                    FileTools.deleteRecursive(questPath + "/data.err");
                    FileTools.renameDirectory(questPath + "/data", questPath + "/data.err");
                    FileTools.deleteRecursive(questPath + "/data");
                    FileTools.renameDirectory(backupDirectory, questPath + "/data");
                    GuiTools.warningDialog("Sorry, an error occured while upgrading the quest.\n"
                            + "Your quest was kept unchanged in format " + questFormat + ".");
                    return false;
                }
                catch (IOException ex) {
                    // The restoration failed for some reason.
                    GuiTools.warningDialog("Sorry, an error occured while upgrading the quest.\n"
                            + "A backup of your quest was saved in format " + questFormat
                            + " was saved in '" + backupDirectory + "'.");
                    return false;
                }
            }
        }
        catch (NoSuchFileException ex) {
            ex.printStackTrace();
            GuiTools.errorDialog("Failed to backup the quest data files:\nNo such file: " + ex.getMessage());
            return false;
        }
        catch (IOException ex) {
            GuiTools.errorDialog("Failed to backup the quest data files:\n" + ex.getMessage());
            return false;
        }
        return true;
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
            createResourceElement(resourceType, "");
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
     * Closes the current editor unless the user is not okay with that.
     */
    private class ActionListenerCloseCurrentEditor implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent ev) {
            tabs.removeCurrentEditor(true);
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
     * @param basepath the default path of the resource.
     */
    public void createResourceElement(ResourceType resourceType, String basepath) {

        String resourceName = resourceType.getName();
        try {
            ResourceBuilderDialog dialog = new ResourceBuilderDialog(resourceType, basepath);
            if (dialog.display()) {
                String id = dialog.getId();
                String friendlyName = dialog.getFriendlyName();

                if (Project.getResource(resourceType).exists(id)) {
                    throw new QuestEditorException(
                            resourceName + " '" + id + "' already exists");
                }
                Project.newResourceElement(resourceType, id, friendlyName);
            }
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Cannot create " + resourceName + ": " + ex.getMessage());
        }
    }

    /**
     * Creates a new lua script, asking its name to the user.
     * @param path The default path of the new script.
     * @return the file name (relative to the project data directory) or null if no exists
     */
    public String createNewLuaScript(String path) {

        NewLuaScriptDialog dialog = new NewLuaScriptDialog(path.isEmpty() ? "" : path + "/");
        if (!dialog.display()) {
            return null;
        }

        String name = dialog.getFileName();
        File file = new File(Project.getDataPath() + "/" + name);

        try {
            if (file.exists()) {
                throw new QuestEditorException("this file already exists");
            }
            if (!Project.ensureParentDirectoryExists(file)) {
                throw new QuestEditorException(
                        "the parent directory no exists and cannot be create");
            }

            file.createNewFile();
            return name;
        }
        catch (Exception ex) {
            GuiTools.errorDialog("Cannot create the script '" +
                    file.getAbsolutePath() + "': " + ex.getMessage());
        }
        return null;
    }

    /**
     * Creates a new directory, asking its name to the user.
     * @param path The default path of the new directory.
     * @return the directory name (relative to the project data directory) or null if no exists
     */
    public String createNewDirectory(String path) {

        NewDirectoryDialog dialog = new NewDirectoryDialog(path.isEmpty() ? "" : path + "/");
        if (!dialog.display()) {
            return null;
        }

        String name = dialog.getDirName();
        File file = new File(Project.getDataPath() + "/" + name);

        try {
            if (file.exists()) {
                throw new QuestEditorException("this directory already exists");
            }

            file.mkdirs();
            return name;
        }
        catch (Exception ex) {
            GuiTools.errorDialog("Cannot create the directory '" +
                    file.getAbsolutePath() + "': " + ex.getMessage());
        }
        return null;
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

        if (!Project.getResource(resourceType).exists(id)) {
            GuiTools.errorDialog(
                    "No such " + resourceNameLower + ": '" + id + "'");
        }
        openResourceElement(resourceType, id);
    }

    /**
     * Ensures that a resource element file exists.
     * If the file doesn't exists, ask and try to create it.
     * @param resourceFile the file of the resource element
     * @param resourceType the type of the resource element
     * @param resourceId the id of the resource element
     * @return true if the file exists or has been successfully created, false otherwise
     * @throws QuestEditorException If the file doesn't exist and cannot be created
     */
    private boolean ensureResourceElementFileExists(File resourceFile,
            ResourceType resourceType, String resourceId) throws QuestEditorException {

        if (!resourceFile.exists()) {
            String resourceTypeName = resourceType.getName().toLowerCase();
            if (!GuiTools.yesNoDialog("The " + resourceTypeName  +
                    " '" + resourceId + "' doesn't exists yet.\n" +
                    "Do you want to create this " + resourceTypeName + "?")) {
                return false;
            }
            try {
                resourceFile.createNewFile();
            } catch (IOException ex) {
                throw new QuestEditorException("the file '" + resourceFile.getAbsolutePath() +
                        "' cannot be created: " + ex.getMessage());
            }
        }
        return true;
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

                    File file = Project.getMapFile(resourceId);
                    if (ensureResourceElementFileExists(file, resourceType, resourceId)) {

                        MapEditorPanel mapEditor = new MapEditorPanel(this, resourceId);
                        tabs.addEditor(mapEditor);
                        // Keep menu items Undo, Cut, Copy, etc. synchronized.
                        mapEditor.getMap().addObserver(this);
                    }
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

                    File file = Project.getTilesetFile(resourceId);
                    if (ensureResourceElementFileExists(file, resourceType, resourceId)) {

                        TilesetEditorPanel tilesetEditor = new TilesetEditorPanel(this, resourceId);
                        tabs.addEditor(tilesetEditor);
                    }
                }
                break;
            }

            case LANGUAGE:
            {
                openTextEditor(Project.getStringsFile(resourceId));
                break;
            }

            case ITEM:
            {
                openTextEditor(Project.getItemScriptFile(resourceId));
                break;
            }

            case SPRITE:
            {
                existingEditor = tabs.getEditor(SpriteEditorPanel.getEditorId(resourceId));
                if (existingEditor != null) {
                    tabs.setSelectedComponent(existingEditor);
                }
                else {

                    File file = Project.getSpriteFile(resourceId);
                    if (ensureResourceElementFileExists(file, resourceType, resourceId)) {

                        SpriteEditorPanel spriteEditor = new SpriteEditorPanel(this, resourceId);
                        tabs.addEditor(spriteEditor);
                    }
                }
                break;
            }

            case ENEMY:
            {
                openTextEditor(Project.getEnemyScriptFile(resourceId));
                break;
            }

            case ENTITY:
            {
                openTextEditor(Project.getEntityScriptFile(resourceId));
                break;
            }

            case SOUND:
            case MUSIC:
            case FONT:
                // No editor for these kinds of resources.
                break;
            }
        }
        catch (QuestEditorException ex) {
            GuiTools.errorDialog(
                    "Cannot open " + resourceType.getName().toLowerCase()
                    + ": " + ex.getMessage());
        }
        catch (NoClassDefFoundError ex) {
            ex.printStackTrace();
            if (ex.getMessage().contains("lua")) {
                // The LuaJ jar is probably missing in the classpath.
                GuiTools.errorDialog(
                        "Cannot open " + resourceType.getName().toLowerCase()
                        + ": The LuaJ jar dependency is missing.\n"
                        + "Please make sure that luaj-jse-2.0.2.jar exists and "
                        + "it is placed in the current directory, and then "
                        + "restart the editor.");
            }
            else {
                // Unknown problem.
                GuiTools.errorDialog(
                        "Cannot open " + resourceType.getName().toLowerCase()
                        + ": " + ex.toString());
            }
        }
        catch (Throwable ex) {
            // Unknown problem.
            ex.printStackTrace();
            GuiTools.errorDialog(
                    "Cannot open " + resourceType.getName().toLowerCase()
                    + ": " + ex.toString());
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
     * Closes the editors of a resource element if some are open.
     * Lets the use save the editors if he wants.
     * @param resourceType Type of resource element to close.
     * @param elementId Id of the resource element to close.
     * @param promptSave true to let the user save the element if necessary,
     * false to close it without confirmation.
     */
    public void closeResourceElement(ResourceType resourceType,
            String elementId, boolean promptSave) {

        // Determine ids of editors to remove.
        ArrayList<String> editorIds = new ArrayList<String>();

        switch (resourceType) {

        case MAP:
        {
            editorIds.add(MapEditorPanel.getEditorId(elementId));
            editorIds.add(TextEditorPanel.getEditorId(Project.getMapScriptFile(elementId)));
            break;
        }

        case TILESET:
        {
            editorIds.add(TilesetEditorPanel.getEditorId(elementId));
            break;
        }

        case LANGUAGE:
        {
            editorIds.add(TextEditorPanel.getEditorId(Project.getDialogsFile(elementId)));
            editorIds.add(TextEditorPanel.getEditorId(Project.getStringsFile(elementId)));
            break;
        }

        case ENEMY:
        {
            editorIds.add(TextEditorPanel.getEditorId(Project.getEnemyScriptFile(elementId)));
            break;
        }

        case ITEM:
        {
            editorIds.add(TextEditorPanel.getEditorId(Project.getItemScriptFile(elementId)));
            break;
        }

        case ENTITY:
        {
            editorIds.add(TextEditorPanel.getEditorId(Project.getEntityScriptFile(elementId)));
            break;
        }

        case SPRITE:
        {
            editorIds.add(SpriteEditorPanel.getEditorId(elementId));
            break;
        }

        case SOUND:
        case MUSIC:
        case FONT:
            // No editor for these kinds of resources.
            break;
        }

        for (String editorId: editorIds) {
            tabs.removeEditor(editorId, promptSave);
        }
    }

    /**
     * Closes the text editor of a file.
     * @param file File to close.
     * @param promptSave true to let the user save the element if necessary,
     * false to close it without confirmation.
     */
    public void closeTextEditor(File file, boolean promptSave) {

        String id = TextEditorPanel.getEditorId(file);
        tabs.removeEditor(id, promptSave);
    }

    /**
     * Changes the id of a resource element, asking the new id to the user.
     * @param resourceType Type of resource element to change.
     * @param oldId The id to change.
     */
    public void moveResourceElement(ResourceType resourceType, String oldId) {

        // First, close the element if it is open.
        closeResourceElement(resourceType, oldId, true);

        // Ask the new id.
        String resourceName = resourceType.getName();
        String resourceNameLower = resourceName.toLowerCase();
        try {
            String newId = (String) JOptionPane.showInputDialog(
                    null,
                    "Please enter a new id for " + resourceNameLower + " '" + oldId + "'",
                    "Change id of " + resourceNameLower + " '" + oldId + "'",
                    JOptionPane.QUESTION_MESSAGE, null, null, oldId);

            if (newId != null) {
                if (Project.getResource(resourceType).exists(newId)) {
                    throw new QuestEditorException(
                            resourceName + " '" + newId + "' already exists");
                }
                Project.moveElement(resourceType, oldId, newId);
            }
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Cannot change id of " + resourceName + " '"
                    + oldId + "': " + ex.getMessage());
        }
    }

    /**
     * Changes the human-readable name of a resource element, asking the new
     * name to the user.
     * @param resourceType Type of resource element to change.
     * @param id Id of the element to change.
     */
    public void renameResourceElement(ResourceType resourceType, String id) {

        // Ask the new name.
        Resource resource = Project.getResource(resourceType);
        String resourceName = resourceType.getName();
        String resourceNameLower = resourceName.toLowerCase();
        try {
            String oldName = resource.getElementName(id);
            String newName = (String) JOptionPane.showInputDialog(
                    null,
                    "Please enter a new name for '" + oldName + "'",
                    "Rename " + resourceNameLower + " '" + oldName + "'",
                    JOptionPane.QUESTION_MESSAGE, null, null, oldName);

            if (newName != null) {
                Project.renameResourceElement(resourceType, id, newName);
            }
        } catch (QuestEditorException ex) {
            GuiTools.errorDialog("Cannot rename " + resourceName + " '"
                    + id + "': " + ex.getMessage());
        }
    }

    /**
     * Deletes a resource element after confirmation from the user.
     * @param resourceType Type of resource.
     * @param resourceId Id of the element to delete.
     */
    public void deleteResourceElement(ResourceType resourceType, String resourceId) {
        try {
            int answer = JOptionPane.showConfirmDialog(this,
                    "Are you sure you want to delete "
                            + resourceType.getName() + " '" + resourceId + "'?",
                    "Are you sure?",
                    JOptionPane.YES_NO_OPTION,
                    JOptionPane.WARNING_MESSAGE);
            if (answer == JOptionPane.YES_OPTION) {
                // Close the element if it is open.
                closeResourceElement(resourceType, resourceId, false);

                // Delete it.
                Project.deleteResourceElement(resourceType, resourceId);
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

        // Close the editor if any (but it is a bug).
        AbstractEditorPanel editor = tabs.getEditor(id);
        if (editor != null) {
            tabs.removeEditor(editor, false);
            new IllegalStateException(resourceType.getName() + " '" + id
                    + "' was destroyed but its editor was still open"
                    ).printStackTrace();
        }
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

        // Close the old editor if any (but it is a bug).
        AbstractEditorPanel editor = tabs.getEditor(oldId);
        if (editor != null) {
            tabs.removeEditor(editor, false);
            new IllegalStateException(resourceType.getName() + " Id '" + oldId
                    + "' was changed from '" + oldId + "' to '" + newId
                    + "' but its editor was still open").printStackTrace();
        }
        closeResourceElement(resourceType, oldId, false);
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

        String editorId = null;

        switch (resourceType) {

        case MAP: editorId = MapEditorPanel.getEditorId(id); break;
        case TILESET: editorId = TilesetEditorPanel.getEditorId(id); break;
        case SPRITE: editorId = SpriteEditorPanel.getEditorId(id); break;

        case LANGUAGE:
        case ENEMY:
        case ITEM:
        case ENTITY:
        case SOUND:
        case MUSIC:
        case FONT:
            // No need to refresh title for these kinds of resources.
            break;
        }

        if (editorId != null) {
            tabs.refreshEditorTitle(editorId);
        }
    }

    /**
     * Returns a map currently open in an editor, given its id.
     * @param mapId Id of the map to get.
     * @return The corresponding map, or null if this map is not open.
     */
    public Map getOpenMap(String mapId) {

        MapEditorPanel mapEditor = getOpenMapEditor(mapId);
        if (mapEditor != null) {
            return mapEditor.getMap();
        }
        return null;
    }

    /**
     * Returns the editor of a map currently open, given the map id.
     * @param mapId Id of the map to get.
     * @return The corresponding map editor, or null if this map is not open.
     */
    public MapEditorPanel getOpenMapEditor(String mapId) {

        String editorId = MapEditorPanel.getEditorId(mapId);
        AbstractEditorPanel editor = tabs.getEditor(editorId);
        return (MapEditorPanel) editor;
    }

    /**
     * Returns all currently open editors of the specified resource type.
     * @param resourceType A type of resource.
     * @return The list of editors of this resource type currently open.
     */
    public Collection<AbstractEditorPanel> getOpenEditors(ResourceType resourceType) {
        return tabs.getEditors(resourceType);
    }

   /**
    * Dialog shown when we want to create a new lua script
    */
   private class NewLuaScriptDialog extends OkCancelDialog {
           private static final long serialVersionUID = 1L;

        // Subcomponents
        private final JTextField nameField;

        /**
         * Constructor.
         */
        public NewLuaScriptDialog(String name) {

            super("New lua script", false);

            JPanel mainPanel = new JPanel(new GridBagLayout());
            GridBagConstraints constraints = new GridBagConstraints();
            constraints.insets = new Insets(5, 5, 5, 5); // margins
            constraints.anchor = GridBagConstraints.LINE_START;
            constraints.gridy = 0;
            constraints.gridx = 0;

            mainPanel.add(new JLabel("name:"), constraints);

            constraints.gridx++;
            nameField = new JTextField(15);
            nameField.setText(name);
            mainPanel.add(nameField, constraints);

            setComponent(mainPanel);
        }

        /**
         * Returns the name of lua script file.
         * @return the file name
         */
        public String getFileName() {

            String name = nameField.getText();

            if (!name.endsWith(".lua")) {
                name += ".lua";
            }

            return name;
        }

        @Override
        protected void applyModifications() {
        }
   }

   /**
    * Dialog shown when we want to create a new directory
    */
   private class NewDirectoryDialog extends OkCancelDialog {
           private static final long serialVersionUID = 1L;

        // Subcomponents
        private final JTextField nameField;

        /**
         * Constructor.
         */
        public NewDirectoryDialog(String name) {

            super("New directory", false);

            JPanel mainPanel = new JPanel(new GridBagLayout());
            GridBagConstraints constraints = new GridBagConstraints();
            constraints.insets = new Insets(5, 5, 5, 5); // margins
            constraints.anchor = GridBagConstraints.LINE_START;
            constraints.gridy = 0;
            constraints.gridx = 0;

            mainPanel.add(new JLabel("name:"), constraints);

            constraints.gridx++;
            nameField = new JTextField(15);
            nameField.setText(name);
            mainPanel.add(nameField, constraints);

            setComponent(mainPanel);
        }

        /**
         * Returns the name of lua script file.
         * @return the file name
         */
        public String getDirName() {

            return nameField.getText();
        }

        @Override
        protected void applyModifications() {
        }
   }
}

