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

import java.util.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import org.solarus.editor.*;

/**
 * A tree that shows the whole resource list of the game:
 * maps, tilesets, sprites, enemies, etc.
 */
public class QuestTree extends JTree
        implements TreeSelectionListener, ProjectObserver {

    private EditorWindow editorWindow;  // The main window.

    private TilesetParentPopupMenu tilesetParentPopupMenu;  // Popup menu for the tileset parent node.
    private MapParentPopupMenu mapParentPopupMenu;  // Popup menu for the map parent node.
    private MapPopupMenu mapPopupMenu;   // Popup menu for individual maps.

    /**
     * Creates a quest tree.
     * @param parent The main quest editor window.
     */
    public QuestTree(EditorWindow parent) {
        setModel(null);  // Because Java makes a stupid example tree by default.
        this.editorWindow = parent;

        addTreeSelectionListener(this);
        addMouseListener(new QuestTreeMouseAdapter());

        mapParentPopupMenu = new MapParentPopupMenu();
        tilesetParentPopupMenu = new TilesetParentPopupMenu();
        mapPopupMenu = new MapPopupMenu();

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
            String humanName = Project.getResource(resourceType).getElementName(id);
            addElementToTree(new ResourceElement(resourceType, id, humanName));
        }
        catch (QuestEditorException ex) {
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
        // TODO Remove the element from the tree instead of rebuilding the whole tree.
        rebuildTree();
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
        // TODO update the id in the tree instead of rebuilding the whole tree.
        rebuildTree();
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
        // TODO update the name in the tree instead of rebuilding the whole tree.
        rebuildTree();
    }

    @Override
    public void valueChanged(TreeSelectionEvent e) {
    }

    /**
     * Rebuilds the whole tree from the resources.
     */
    public void rebuildTree() {
        setModel(new EditorTreeModel(Project.getDataPath()));
        repaint();        
    }

    /**
     * Adds a resource element to the tree.
     * @param element The resource element to add.
     */
    private void addElementToTree(ResourceElement element) {
        DefaultMutableTreeNode resourceNode = (DefaultMutableTreeNode)
                treeModel.getChild(treeModel.getRoot(), element.type.ordinal());
        ((EditorTreeModel) treeModel).insertNodeInto(
                new DefaultMutableTreeNode(element), resourceNode, resourceNode.getChildCount());
        repaint();
    }

    public void update(Observable o, Object arg) {
        repaint();
    }

    private class EditorTreeModel extends DefaultTreeModel {

        public EditorTreeModel(String path) {
            super((new DefaultMutableTreeNode("Quest")));
            for (ResourceType resourceType : ResourceType.values()) {
                DefaultMutableTreeNode resourceNode = new DefaultMutableTreeNode(resourceType.getName());
                ((DefaultMutableTreeNode) getRoot()).add(resourceNode);
                if (Project.isLoaded()) {
                    addChildren(resourceNode, resourceType);
                }
            }
        }

        @Override
        public boolean isLeaf(Object e) {
            return ((DefaultMutableTreeNode) e).getChildCount() == 0 && !(((DefaultMutableTreeNode) e).getUserObject() instanceof String);
        }

        protected final void addChildren(DefaultMutableTreeNode parentNode, ResourceType resourceType) {
            Resource resource = Project.getResource(resourceType);
            String[] ids = resource.getIds();

            try {
                for (int i = 0; i < ids.length; i++) {
                    parentNode.add(new DefaultMutableTreeNode(new ResourceElement(resourceType, ids[i], resource.getElementName(ids[i]))));
                }
            }
            catch (QuestEditorException ex) {
                GuiTools.errorDialog("Unexpected error while building the quest tree: " + ex.getMessage());
            }
        }
    }

    private class QuestTreeMouseAdapter extends MouseAdapter {

        @Override
        public void mousePressed(MouseEvent e) {
            DefaultMutableTreeNode clickedNode = null;
            TreePath selectionPath = QuestTree.this.getSelectionPath();
            if (e.getButton() == MouseEvent.BUTTON3) {
                if (selectionPath == null) {
                    return;
                }
                clickedNode = (DefaultMutableTreeNode) selectionPath.getLastPathComponent();
                if (clickedNode.getUserObject() instanceof ResourceElement) {
                    // right click: show a popup menu if the element is a map
                    ResourceElement element = (ResourceElement) clickedNode.getUserObject();
                    int row = QuestTree.this.getRowForLocation(e.getX(), e.getY());
                    if (row == -1) {
                        return;
                    }
                    QuestTree.this.setSelectionRow(row);
                    if (element.type == ResourceType.MAP) {
                        mapPopupMenu.setMap(element.id);
                        mapPopupMenu.show((JComponent) e.getSource(),
                                e.getX(), e.getY());
                    }
                }
                else {
                    //Show the popup menu when right-click on map
                    if (clickedNode.getUserObject().equals(ResourceType.MAP.getName())) {
                        mapParentPopupMenu.show((JComponent) e.getSource(),
                                e.getX(), e.getY());
                    }
                    //Popup menu when right-click on tileset
                    else if (clickedNode.getUserObject().equals(ResourceType.TILESET.getName())) {
                        tilesetParentPopupMenu.show((JComponent) e.getSource(),
                                e.getX(), e.getY());                                                      
                    }
                }
            } else if (e.getClickCount() == 2) {
                // double-click: open the clicked file

                if (selectionPath == null) {
                    return;
                }
                clickedNode = (DefaultMutableTreeNode) selectionPath.getLastPathComponent();
                if (clickedNode.getUserObject() instanceof ResourceElement) {
                    ResourceElement element = (ResourceElement) clickedNode.getUserObject();
                    editorWindow.openResourceElement(element.type, element.id);
                }
            }
        }
    }

    /**
     * Popup menu of the map parent node.
     */
    private class MapParentPopupMenu extends JPopupMenu {    

        private JMenuItem newMapMenu;

        public MapParentPopupMenu() {
            newMapMenu = new JMenuItem("New Map");
            newMapMenu.addActionListener(new ActionListener() {
                public void actionPerformed(ActionEvent ev) {
                    editorWindow.createResourceElement(ResourceType.MAP);
                }
            });
            add(newMapMenu);            
        }
    }

    /**
     * Popup menu of the tileset parent node.
     */
    private class TilesetParentPopupMenu extends JPopupMenu {    

        private JMenuItem newTilesetMenu;
        
        public TilesetParentPopupMenu() {
            newTilesetMenu = new JMenuItem("New Tileset");
            newTilesetMenu.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent ev) {
                    editorWindow.createResourceElement(ResourceType.TILESET);            
                }
            });
            add(newTilesetMenu);            
        }
    }

    /**
     * Popup menu of map leaves.
     */
    private class MapPopupMenu extends JPopupMenu {

        private String mapId;
        private JMenuItem mapMenu;
        private JMenuItem scriptMenu;
        private JMenuItem deleteMenu;

        public MapPopupMenu() {

            mapMenu = new JMenuItem("Open Map");
            add(mapMenu);
            mapMenu.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    editorWindow.openResourceElement(ResourceType.MAP, mapId);
                }
            });

            scriptMenu = new JMenuItem("Open Map Script");
            add(scriptMenu);
            scriptMenu.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    editorWindow.openTextEditor(Project.getMapScriptFile(mapId));
                }
            });

            deleteMenu = new JMenuItem("Delete Map");
            add(deleteMenu);
            deleteMenu.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent e) {
                    editorWindow.deleteResourceElement(ResourceType.MAP, mapId);
                }
            });
        }

        public void setMap(String mapId) {
            this.mapId = mapId;
        }
    }

    /**
     * Stores the id of an element from a resource, its name and its
     * resource type.
     */
    private class ResourceElement {

        public final ResourceType type;
        public final String id;
        public final String name;

        public ResourceElement(ResourceType type, String id, String name) {
            this.type = type;
            this.id = id;
            this.name = name;
        }

        public String toString() {
            return name;
        }
    }
}
