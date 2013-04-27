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

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import org.solarus.editor.*;

/**
 * A tree that shows the whole resource list of the game:
 * maps, tilesets, sprites, enemies, etc.
 * Under the root node, there is a node for each resource type.
 * These nodes have a ResourceType user object.
 *
 * Under each resource type, there are the nodes of all corresponding
 * resource elements. These nodes have a ResourceElement user object.
 * 
 * - Root
 *   - ResourceType
 *     - ResourceElement
 *     - ResourceElement
 *     - ResourceElement
 *     - ...
 *   - ResourceType
 *     - ResourceElement
 *     - ResourceElement
 *     - ResourceElement
 *     - ...
 *   - ResourceType
 *     - ResourceElement
 *     - ResourceElement
 *     - ResourceElement
 *     - ...
 *   - ...
 */
public class QuestTree extends JTree
        implements TreeSelectionListener, ProjectObserver {

    private EditorWindow editorWindow;  // The main window.

    /**
     * Creates a quest tree.
     * @param parent The main quest editor window.
     */
    public QuestTree(EditorWindow parent) {
        setModel(null);  // Because Java makes a stupid example tree by default.
        this.editorWindow = parent;

        addTreeSelectionListener(this);
        addMouseListener(new QuestTreeMouseAdapter());

        Project.addProjectObserver(this);
    }

    /**
     * Called when another project becomes active.
     */
    @Override
    public void currentProjectChanged() {
        rebuildTree();
    }

    /**
     * Called when a new resource element has just been created.
     * @param resourceType Type of resource.
     * @param id Id of the new element.
     */
    @Override
    public void resourceElementAdded(ResourceType resourceType, String id) {
        // Insert the new element in the tree.
        try {
            addResourceElementToTree(new ResourceElement(resourceType, id));
        }
        catch (QuestEditorException ex) {
            // Cannot happen: the id is valid here.
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
        rebuildTree();  // TODO Rebuilding the whole tree is slow.
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
        rebuildTree();  // TODO Rebuilding the whole tree is slow.
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
        rebuildTree();  // TODO Rebuilding the whole tree is slow.
    }

    @Override
    public void valueChanged(TreeSelectionEvent e) {
    }

    /**
     * Rebuilds the whole tree from the resources.
     */
    public void rebuildTree() {
        setModel(new QuestTreeModel());
        repaint();
    }

    /**
     * Adds a resource element to the tree.
     * @param element The resource element to add.
     */
    private void addResourceElementToTree(ResourceElement element) {
        DefaultMutableTreeNode resourceNode = (DefaultMutableTreeNode)
                treeModel.getChild(treeModel.getRoot(), element.type.ordinal());
        ((QuestTreeModel) treeModel).insertNodeInto(
                new DefaultMutableTreeNode(element), resourceNode, resourceNode.getChildCount());
        repaint();
    }

    /**
     * Model used for the quest tree.
     */
    private class QuestTreeModel extends DefaultTreeModel {

        /**
         * Creates a quest tree.
         */
        public QuestTreeModel() {
            super((new DefaultMutableTreeNode("Quest")));

            // Create a parent node for each type of resource:
            // map, tileset, sound, etc.
            for (ResourceType resourceType : ResourceType.values()) {
                DefaultMutableTreeNode resourceNode =
                        new DefaultMutableTreeNode(resourceType);
                ((DefaultMutableTreeNode) getRoot()).add(resourceNode);
                if (Project.isLoaded()) {
                    addChildren(resourceNode, resourceType);
                }
            }
        }

        /**
         * Returns whether a node is a leaf.
         * @param node A node of the tree.
         * @return true if this node should be considered as a leaf.
         */
        @Override
        public boolean isLeaf(Object node) {
            return super.isLeaf(node) &&
                    ((DefaultMutableTreeNode) node).getUserObject() instanceof ResourceElement;
        }

        /**
         * Builds the subtree of a resource type.
         * @param parentNode The parent node of the subtree to create.
         * @param resourceType A type of resource.
         */
        protected final void addChildren(DefaultMutableTreeNode parentNode,
                ResourceType resourceType) {

            Resource resource = Project.getResource(resourceType);
            String[] ids = resource.getIds();

            try {
                for (int i = 0; i < ids.length; i++) {
                    parentNode.add(new DefaultMutableTreeNode(
                            new ResourceElement(resourceType, ids[i])));
                }
            }
            catch (QuestEditorException ex) {
                GuiTools.errorDialog("Unexpected error while building the quest tree: " + ex.getMessage());
            }
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
            Object clickedObject = clickedNode.getUserObject();

            if (ev.getButton() == MouseEvent.BUTTON3) {
                // Right click.

                JPopupMenu popupMenu = null;

                if (clickedObject instanceof ResourceElement) {
                    // Right click on a resource element.
                    ResourceElement element = (ResourceElement) clickedObject;
                    popupMenu = new ResourceElementPopupMenu(element);
                }
                else if (clickedObject instanceof ResourceType) {
                    // Right click on a resource type (parent node).
                    ResourceType resourceType = (ResourceType) clickedObject;
                    popupMenu = new ResourceParentPopupMenu(resourceType);
                }
                
                if (popupMenu != null) {
                    popupMenu.show((JComponent) ev.getSource(),
                            ev.getX(), ev.getY());
                }
            }
            else if (ev.getClickCount() == 2) {
                // Double-click: open the clicked element.
                if (clickedObject instanceof ResourceElement) {
                    ResourceElement element = (ResourceElement) clickedObject;
                    editorWindow.openResourceElement(element.type, element.id);
                }
            }
        }
    }

    /**
     * Popup menu of any resource parent node.
     */
    private class ResourceParentPopupMenu extends JPopupMenu {    

        /**
         * Creates the popup menu of a resource type.
         * @param resourceType A resource type.
         */
        public ResourceParentPopupMenu(final ResourceType resourceType) {

            JMenuItem newElementItem = new JMenuItem(
                    "New " + resourceType.getName().toLowerCase());
            newElementItem.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent ev) {
                    editorWindow.createResourceElement(resourceType);
                }
            });
            add(newElementItem);
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

    /**
     * Stores the id of an element from a resource, its name and its
     * resource type.
     * This class is used as the user object for nodes of the quest tree.
     */
    private class ResourceElement {

        public final ResourceType type;
        public final String id;
        public final String name;

        public ResourceElement(ResourceType type, String id)
                throws QuestEditorException {
            this.type = type;
            this.id = id;
            this.name = Project.getResource(type).getElementName(id);
        }

        public String toString() {
            return name;
        }
    }
}
