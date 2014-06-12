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
package org.solarus.editor.gui.tree;


import java.awt.event.*;
import java.io.File;
import javax.swing.*;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import org.solarus.editor.*;
import org.solarus.editor.gui.*;

/**
 * A tree that shows the project data directory, resources and scripts.
 * Under the root node, there is a node for each sub directories, resource files
 * or lua scripts.
 *
 * The tree keeps the hierarchy order and the alphabetical order of files.
 *
 * All file or directory nodes have a FileElement user object, and all resource
 * nodes have a ResourceElement user object.
 *
 * - Root (FileElement)
 *   - FileElement
 *     - FileElement
 *       - FileElement (script)
 *       - FileElement (script)
 *       - ...
 *       - ResourceElement
 *       - ResourceElement
 *       - ...
 *     - FileElement (script)
 *     - ...
 *     - ResourceElement
 *     - ...
 *   - FileElement
 *     - FileElement (script)
 *     - ...
 *     - ResourceElement
 *     - ...
 *   - FileElement (script)
 *   - ...
 */
public class QuestTree extends JTree implements ProjectObserver {

    private EditorWindow editorWindow;  // The main window.

    /**
     * Creates a quest tree.
     * @param parent The main quest editor window.
     */
    public QuestTree(EditorWindow parent) {
        setModel(null);  // Because Java makes a stupid example tree by default.
        this.editorWindow = parent;

        setDragEnabled(true);
        addMouseListener(new QuestTreeMouseAdapter());

        Project.addProjectObserver(this);

        setTransferHandler(new QuestTreeTransferHandler());
    }

    /**
     * Called when another project becomes active.
     */
    @Override
    public void currentProjectChanged() {
        rebuildTree();

        // Initially expand maps.
        DefaultMutableTreeNode node = getFileElement(ResourceType.MAP.getDirName());
        if (node != null) {
            DefaultMutableTreeNode root = (DefaultMutableTreeNode) getModel().getRoot();
            expandRow(root.getIndex(node) + 1);
        }
    }

    /**
     * Called when a new resource element has just been created.
     * @param resourceType Type of resource.
     * @param id Id of the new element.
     */
    @Override
    public void resourceElementAdded(ResourceType resourceType, String id) {

        try {
            addResourceElementToTree(new ResourceElement(resourceType, id));
        } catch (QuestEditorException ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Called when a new resource element has just been deleted.
     * @param resourceType Type of resource.
     * @param id Id of the deleted element.
     */
    @Override
    public void resourceElementRemoved(ResourceType resourceType, String id) {

        DefaultMutableTreeNode node = getResourceElement(resourceType, id);
        if (node != null) {
            DefaultTreeModel model = (DefaultTreeModel) getModel();
            model.removeNodeFromParent(node);
            repaint();
        }
    }

    /**
     * Called when a new resource element has just been moved.
     * @param resourceType Type of resource.
     * @param oldId Old id of the element.
     * @param newId New id of the element.
     */
    @Override
    public void resourceElementMoved(ResourceType resourceType, String oldId,
            String newId) {

        resourceElementRemoved(resourceType, oldId);
        resourceElementAdded(resourceType, newId);
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

        try {
            ResourceElement element = new ResourceElement(resourceType, id);
            DefaultMutableTreeNode node = getResourceElement(resourceType, id);
            if (node != null) {
                node.setUserObject(element);
                repaint();
            }
        } catch (QuestEditorException ex) {
            // if project isn't loaded
            ex.printStackTrace();
        }
    }

    /**
     * Rebuilds the whole tree from the data project directory.
     */
    public void rebuildTree() {

        try {
            DefaultMutableTreeNode root =
                    new DefaultMutableTreeNode(new FileElement(""), true);
            DefaultTreeModel model = new DefaultTreeModel(root);
            model.setAsksAllowsChildren(true);
            setModel(model);

            // add resources
            for (ResourceType type: ResourceType.values()) {
                for (String id: Project.getResource(type).getIds()) {
                    addResourceElementToTree(new ResourceElement(type, id));
                }
            }

            // add lua script files
            buildNode("", new File(Project.getDataPath()));

            repaint();
        }
        catch (QuestEditorException ex) {
            // if the data project directory doesn't exists, can not happen
            ex.printStackTrace();
        }
    }

    private void buildNode(String prefix, File directory) {

        if (!directory.exists() || !directory.isDirectory()) {
            return;
        }

        for (File file: directory.listFiles()) {

            String path = (prefix.isEmpty() ? "" : prefix + "/") + file.getName();
            if (file.isDirectory()) {

                addFileElementToTree(path);
                buildNode(path, file);
            }
            else if (path.endsWith(".lua")) {

                // the file script isn't a resource
                if (!isResourcePath(path)) {
                    addFileElementToTree(path);
                }
            }
        }
    }

    /**
     * Checks if a path corresponds to an existing resource.
     * @param path the path to check
     * @return true if the path corresponds to en existing resource, false otherwise
     */
    private boolean isResourcePath(String path) {

        if (path.contains("/") && (path.endsWith(".dat") || path.endsWith(".lua"))) {

            path = path.substring(path.indexOf("/") + 1);
            path = path.replace(".dat", "").replace(".lua", "");

            for (ResourceType type: ResourceType.values()) {
                if (Project.getResource(type).exists(path)) {
                    return true;
                }
            }
        }
        return false;
    }

    /**
     * Returns a specified file node.
     * @param path the path corresponding to the node
     * @return The found node or null if no exists
     */
    private DefaultMutableTreeNode getFileElement(String path) {

        DefaultMutableTreeNode node = (DefaultMutableTreeNode) getModel().getRoot();

        if (!path.isEmpty()) {
            node = getFileChildNode(node, path);
        }

        return node;
    }

    /**
     * Returns a specified child file node.
     * @param parentNode the parent node that contains the node
     * @param path the path corresponding to the node
     * @return the node or null if no exists
     */
    private DefaultMutableTreeNode getFileChildNode(
            DefaultMutableTreeNode parentNode,String path) {

        DefaultMutableTreeNode child = null;

        if (parentNode.getChildCount() > 0) {
            child = (DefaultMutableTreeNode) parentNode.getFirstChild();
        }

        while (child != null) {

            Object userObject = child.getUserObject();
            if (userObject instanceof FileElement) {

                FileElement type = (FileElement) userObject;
                if (path.equals(type.getPath())) {
                    return child;
                }
                else if (type.isDirectory() && path.startsWith(type.getPath())) {
                    return getFileChildNode(child, path);
                }
            }

            child = child.getNextSibling();
        }

        return child;
    }

    /**
     * Returns a specified resource node.
     * @param resourceType the type of the resource
     * @param id the id of the resource
     * @return The found node or null if no exists
     */
    private DefaultMutableTreeNode getResourceElement(ResourceType resourceType, String id) {

        DefaultMutableTreeNode node = (DefaultMutableTreeNode) getModel().getRoot();
        return getResourceChildNode(node, resourceType, id);
    }

    /**
     * Returns a specified child resource node.
     * @param parentNode the parent node that contains the node
     * @param resourceType the type of the resource
     * @param id the id of the resource
     * @return the node or null if no exists
     */
    private DefaultMutableTreeNode getResourceChildNode(
            DefaultMutableTreeNode parentNode, ResourceType resourceType, String id) {

        DefaultMutableTreeNode child = null;

        if (parentNode.getChildCount() > 0) {
            child = (DefaultMutableTreeNode) parentNode.getFirstChild();
        }

        while (child != null) {

            Object userObject = child.getUserObject();
            if (userObject instanceof ResourceElement) {

                ResourceElement nodeElement = (ResourceElement) userObject;
                if (nodeElement.type == resourceType && nodeElement.id.equals(id)) {
                    return child;
                }
            }
            else if (userObject instanceof FileElement) {

                FileElement type = (FileElement) userObject;
                if (type.isDirectory()) {
                    DefaultMutableTreeNode node = getResourceChildNode(child, resourceType, id);
                    if (node != null) {
                        return node;
                    }
                }
            }

            child = child.getNextSibling();
        }

        return child;
    }

    /**
     * Adds a node element that represent a file to the tree.
     * @param path the path corresponding to the node
     */
    private void addFileElementToTree(String path) {

        // if the node alreay exists
        if (getFileElement(path) != null) {
            return;
        }
        DefaultMutableTreeNode node, parentNode;

        try {
            FileElement element = new FileElement(path);
            node = new DefaultMutableTreeNode(element, element.isDirectory());
            // if is in the root dir
            if (!path.contains("/")) {
                parentNode = (DefaultMutableTreeNode) getModel().getRoot();
            }
            else {
                path = path.substring(0, path.lastIndexOf("/"));
                parentNode = getFileElement(path);
                // try to create directory if is not exists
                if (parentNode == null) {
                    addFileElementToTree(path);
                    parentNode = getFileElement(path);
                }
            }

            if (parentNode != null) {
                DefaultTreeModel model = (DefaultTreeModel) getModel();
                int location = findNewFileLocation(parentNode, element);
                model.insertNodeInto(node, parentNode, location);
                repaint();
            }
        } catch (QuestEditorException ex) {
            // if the project isn't loaded, can not be happen
            ex.printStackTrace();
        }
    }

    /**
     * Find a location for a new file in a node.
     * Lets keep the alphabetical order of files, and keep a separation between
     * files and directories.
     * @param node the node to insert the new file
     * @param element the file element of the node to add
     * @return the new location
     */
    private int findNewFileLocation(DefaultMutableTreeNode node, FileElement element) {

        int location = 0;

        if (node.getChildCount() > 0) {

            DefaultMutableTreeNode child = (DefaultMutableTreeNode) node.getFirstChild();
            while (child != null) {

                Object userObject = child.getUserObject();
                if (userObject instanceof FileElement) {

                    FileElement file = (FileElement) userObject;
                    if (element.isDirectory()) {
                        if (file.isDirectory()) {
                            if (file.getPath().compareTo(element.getPath()) > 0) {
                                return location;
                            }
                        } else {
                            return location;
                        }
                    }
                    else {
                        if (!file.isDirectory() && file.getPath().compareTo(element.getPath()) > 0) {
                            return location;
                        }
                    }
                } else if (userObject instanceof ResourceElement) {
                    return location;
                }

                child = child.getNextSibling();
                location++;
            }
        }

        return location;
    }

    /**
     * Removes a file element node from the tree.
     * If element is a directory, it must be empty to be deleted.
     * @param path the path of the file element
     */
    private void removeFileElement(String path) {

        DefaultMutableTreeNode node = getFileElement(path);
        if (node != null) {
            if (node.getUserObject() instanceof FileElement) {

                File file = new File(Project.getDataPath() + "/" + path);
                if (!file.isDirectory() || file.list().length == 0) {
                    DefaultTreeModel model = (DefaultTreeModel) getModel();
                    model.removeNodeFromParent(node);
                }
            }
        }
    }

    /**
     * Move a file element node in the tree.
     * The lua script file will be moved in the file system.
     * @param oldPath the path of file element ot move
     * @param newPath the new path of the file element
     * @throws QuestEditorException if the script cannot be move
     */
    public void moveFileElement(String oldPath, String newPath)
            throws QuestEditorException {

        if (newPath.equals(oldPath)) {
            return;
        }

        try {
            // check old path
            DefaultMutableTreeNode node = getFileElement(oldPath);
            if (node == null) {
                throw new QuestEditorException("this script doesn't exists");
            }
            // check new path
            DefaultMutableTreeNode newNode = getFileElement(newPath);
            if (newNode != null) {
                throw new QuestEditorException("the new script already exists");
            }
            // trying to move a resource
            if (!(node.getUserObject() instanceof FileElement)) {
                throw new QuestEditorException("the script corresponds to a resource");
            }
            // trying to move a directory
            File file = new File(Project.getDataPath() + "/" + oldPath);
            if (file.isDirectory()) {
                throw new QuestEditorException("the path corresponds to a directory");
            }

            // rename file
            editorWindow.closeTextEditor(file, false);
            File newFile = new File(Project.getDataPath() + "/" + newPath);
            if (!file.renameTo(newFile)) {
                throw new QuestEditorException("error on renaming");
            }
            // remove node
            DefaultTreeModel model = (DefaultTreeModel) getModel();
            model.removeNodeFromParent(node);
            // add new node
            addFileElementToTree(newPath);
        }
        catch (QuestEditorException ex) {
            throw new QuestEditorException("cannot rename script '" + oldPath +
                    "' in '" + newPath + "': " + ex.getMessage());
        }
    }

    /**
     * Adds a node element that represent a resource to the tree.
     * @param element The resource element to add.
     */
    private void addResourceElementToTree(ResourceElement element) {

        String path = element.type.getDirName() + "/" + element.id + ".lua";
        DefaultMutableTreeNode node = getResourceElement(element.type, element.id);
        // if the resource node already exists
        if (node != null) {
            return;
        }

        node = getFileElement(path);
        // if the node exists as a script
        if (node != null) {
            node.setUserObject(element);
        }
        else {
            node = new DefaultMutableTreeNode(element, false);

            path = path.substring(0, path.lastIndexOf("/"));
            DefaultMutableTreeNode parentNode = getFileElement(path);
            // try to create if no exists
            if (parentNode == null) {
                addFileElementToTree(path);
                parentNode = getFileElement(path);
            }

            if (parentNode != null) {
                DefaultTreeModel model = (DefaultTreeModel) getModel();
                int location = findNewResourceLocation(parentNode, element);
                model.insertNodeInto(node, parentNode, location);
                repaint();
            }
        }

        repaint();
    }

    /**
     * Find a location for a new resource in a node.
     * Lets keep the alphabetical order of resources.
     * @param node the node to insert the new file
     * @param element the resource element of the node to add
     * @return the new location
     */
    private int findNewResourceLocation(DefaultMutableTreeNode node, ResourceElement element) {

        int location = 0;

        if (node.getChildCount() > 0) {

            DefaultMutableTreeNode child = (DefaultMutableTreeNode) node.getFirstChild();
            while (child != null) {

                Object userObject = child.getUserObject();
                if (userObject instanceof ResourceElement) {

                    ResourceElement resource = (ResourceElement) userObject;
                    if (resource.id.compareTo(element.id) > 0) {
                        return location;
                    }
                }

                child = child.getNextSibling();
                location++;
            }
        }

        return location;
    }

    /**
     * Quest tree file userObject type.
     */
    protected class FileElement {

        // is a directory
        private final boolean isDirectory;
        // path file corresponding to the file relative to the data project directory
        private final String path;
        // the name to display node (by default filename without extension)
        private String name;

        /**
         * Constructor.
         * @param path path file corresponding to the node file relative to the data project directory
         * @throws QuestEditorException if file no exists
         */
        public FileElement(String path) throws QuestEditorException {

            this.path = path;

            File file = new File(Project.getDataPath() + "/" + path);
            isDirectory = file.isDirectory();
            // if is the root
            if (path.isEmpty()) {
                name = Project.getRootPath();
                name = "<html>Quest <font style=\"color:gray;\">(" + name  + ")</font></html>";
            }
            else {
                name = file.getName();
            }
        }

        /**
         * Checks if the node represents a directory.
         * @return true if the node reprensents a directory, false otherwise
         */
        public boolean isDirectory() {

            return isDirectory;
        }

        /**
         * Returns the path file.
         * @return path file
         */
        public String getPath() {

            return path;
        }

        @Override
        public String toString() {

            return name;
        }
    }

    private void openPopupMenu (DefaultMutableTreeNode node, JComponent source, int x, int y) {

        JPopupMenu popupMenu = null;
        Object userObject = node.getUserObject();

        if (userObject instanceof ResourceElement) {

            ResourceElement element = (ResourceElement) userObject;
            popupMenu = new ResourceElementPopupMenu(element);
        }
        else if (userObject instanceof FileElement) {

            FileElement element = (FileElement) userObject;
            String path = element.getPath();

            if (element.isDirectory()) {
                for (ResourceType type: ResourceType.values()) {
                    String dirName = type.getDirName();
                    if (path.equals(dirName)) {
                        popupMenu = new ResourceParentPopupMenu(path, type, "");
                        break;
                    }
                    else if (path.startsWith(dirName)) {
                        String basepath = path.substring(path.indexOf("/") + 1);
                        popupMenu = new ResourceParentPopupMenu(path, type, basepath);
                        break;
                    }
                }

                if (popupMenu == null) {
                    popupMenu = new DirectoryPopupMenu(path);
                }

            } else {
                popupMenu = new FileElementPopupMenu(path);
            }
        }

        if (popupMenu != null) {
            popupMenu.show(source, x, y);
        }
    }

    /**
     * Handles the mouse events received by the quest tree.
     */
    private class QuestTreeMouseAdapter extends MouseAdapter {

        /**
         * Called when a mouse button is pressed on the tree component.
         */
        @Override
        public void mousePressed(MouseEvent ev) {

            int row = QuestTree.this.getRowForLocation(ev.getX(), ev.getY());
            if (row != -1) {
                QuestTree.this.setSelectionRow(row);
            }

            TreePath selectionPath = QuestTree.this.getSelectionPath();
            if (selectionPath == null) {
                return;
            }

            // Retrieve the node clicked.
            DefaultMutableTreeNode clickedNode = (DefaultMutableTreeNode)
                    selectionPath.getLastPathComponent();

            if (ev.getButton() == MouseEvent.BUTTON3) {
                // Right click.
                openPopupMenu(clickedNode, (JComponent) ev.getSource(), ev.getX(), ev.getY());
            }
            else if (ev.getClickCount() == 2) {
                // Double-click: open the clicked element.
                Object clickedObject = clickedNode.getUserObject();
                if (clickedObject instanceof ResourceElement) {
                    ResourceElement element = (ResourceElement) clickedObject;
                    editorWindow.openResourceElement(element.type, element.id);
                }
                else if  (clickedObject instanceof FileElement) {
                    FileElement element = (FileElement) clickedObject;
                    if (!element.isDirectory()) {
                        editorWindow.openTextEditor(new File(Project.getDataPath()
                                + "/" + element.getPath()));
                    }
                }
            }
        }
    }

    /**
     * Popup menu of any script file node.
     */
    private class FileElementPopupMenu extends JPopupMenu {

        /**
         * Creates the popup menu of a script file.
         * @param path The path of the file
         */
        public FileElementPopupMenu(final String path) {

            // Open.
            JMenuItem menuItem = new JMenuItem("Open script");
            add(menuItem);
            menuItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    editorWindow.openTextEditor(new File(Project.getDataPath() + "/" + path));
                }
            });

            // Delete.
            menuItem = new JMenuItem("Delete script");
            add(menuItem);
            menuItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    File file = new File(Project.getDataPath() + "/" + path);

                    int answer = JOptionPane.showConfirmDialog(editorWindow,
                    "Are you sure you want to delete '"
                            + path + "' script ?",
                    "Are you sure?",
                    JOptionPane.YES_NO_OPTION,
                    JOptionPane.WARNING_MESSAGE);

                    // if yes, delete the file
                    if (answer == JOptionPane.YES_OPTION && file.delete()) {
                        removeFileElement(path);
                        editorWindow.closeTextEditor(file, false);
                    }
                }
            });
        }
    }

    /**
     * Popup menu of any directory node.
     */
    private class DirectoryPopupMenu extends JPopupMenu {

        /**
         * Creates the popup menu of a resource type.
         * @param path the path of the directory.
         */
        public DirectoryPopupMenu(final String path) {

            // new script
            JMenuItem elementItem = new JMenuItem("New script");
            elementItem.addActionListener(new ActionListener() {

                @Override
                public void actionPerformed(ActionEvent ev) {

                    String name = editorWindow.createNewLuaScript(path);
                    if (name != null && !name.isEmpty()) {
                        addFileElementToTree(name);
                    }
                }
            });
            add(elementItem);

            // delete directory (if empty)
            final File file = new File(Project.getDataPath() + "/" + path);
            elementItem = new JMenuItem("delete directory");
            elementItem.setEnabled(file.isDirectory() && file.list().length == 0);
            elementItem.addActionListener(new ActionListener() {

                @Override
                public void actionPerformed(ActionEvent ev) {
                    if (file.delete()) {
                        removeFileElement(path);
                    }
                }
            });
            add(elementItem);
        }
    }

    /**
     * Popup menu of any resource parent node.
     */
    private class ResourceParentPopupMenu extends DirectoryPopupMenu {

        /**
         * Creates the popup menu of a resource type.
         * @param path the path of the director
         * @param resourceType A resource type.y.
         * @param basepath the default path of the resource.
         */
        public ResourceParentPopupMenu(final String path,
                final ResourceType resourceType, final String basepath) {
            super(path);

            JMenuItem elementItem = new JMenuItem(
                    "New " + resourceType.getName().toLowerCase());
            elementItem.addActionListener(new ActionListener() {

                @Override
                public void actionPerformed(ActionEvent ev) {

                    editorWindow.createResourceElement(resourceType, basepath);
                }
            });
            addSeparator();
            add(elementItem);
        }
    }

    /**
     * Popup menu of a resource element.
     */
    private class ResourceElementPopupMenu extends JPopupMenu {

        /**
         * Creates a popup menu for the given resource element.
         * @param element A resource element.
         */
        public ResourceElementPopupMenu(final ResourceElement element) {

            // Open.
            String resourceTypeName = element.type.getName().toLowerCase();
            JMenuItem menuItem = new JMenuItem("Open " + resourceTypeName);
            add(menuItem);
            menuItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    editorWindow.openResourceElement(element.type, element.id);
                }
            });

            // Open map script (specific to maps).
            if (element.type == ResourceType.MAP) {
                menuItem = new JMenuItem("Open map script");
                add(menuItem);
                menuItem.addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        editorWindow.openTextEditor(Project.getMapScriptFile(element.id));
                    }
                });
            }

            // Move.
            menuItem = new JMenuItem("Change id");
            add(menuItem);
            menuItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    editorWindow.moveResourceElement(element.type, element.id);
                }
            });

            // Rename.
            menuItem = new JMenuItem("Rename");
            add(menuItem);
            menuItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    editorWindow.renameResourceElement(element.type, element.id);
                }
            });

            // Delete.
            menuItem = new JMenuItem("Delete " + resourceTypeName);
            add(menuItem);
            menuItem.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    editorWindow.deleteResourceElement(element.type, element.id);
                }
            });
        }
    }
}
