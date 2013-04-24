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

import java.io.*;
import java.util.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import org.solarus.editor.*;
import org.solarus.editor.Map;
import org.solarus.editor.entities.*;

/**
 * A tree that shows the whole resource list of the game:
 * maps, tilesets, sprites, enemies, etc.
 */
public class QuestDataTree extends JTree
        implements TreeSelectionListener, Observer, ProjectObserver {

    private EditorWindow editorWindow;
    private QuestDataTreeTilesetPopupMenu tilesetPopupMenu;
    private QuestDataTreeMapPopupMenu mapPopupMenu;
    private QuestDataTreePopupMenu popupMenu;

    public QuestDataTree(EditorWindow parent) {
        setModel(null);  // Because Java makes a stupid example tree by default.
        this.editorWindow = parent;

        addTreeSelectionListener(this);
        addMouseListener(new QuestDataTreeMouseAdapter());
        
        mapPopupMenu = new QuestDataTreeMapPopupMenu();
        popupMenu = new QuestDataTreePopupMenu();
        tilesetPopupMenu = new QuestDataTreeTilesetPopupMenu();

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
     * Reloads the tree, rebuilding the model from the resources.
     */
    public void rebuildTree() {
        setModel(new EditorTreeModel(Project.getDataPath()));
        repaint();        
    }

    @Override
    public void valueChanged(TreeSelectionEvent e) {
    }

    public void addMap(Map map) {
        DefaultMutableTreeNode mapNode = (DefaultMutableTreeNode) treeModel.getChild(treeModel.getRoot(), ResourceType.MAP.ordinal());
        ResourceElement element = new ResourceElement(ResourceType.MAP, map.getId(), map.getName());
        ((EditorTreeModel) treeModel).insertNodeInto(new DefaultMutableTreeNode(element), mapNode, mapNode.getChildCount());
        map.addObserver(this);
        repaint();
    }

    public void addTileset(Tileset tileset) {
        DefaultMutableTreeNode tilesetNode = (DefaultMutableTreeNode) treeModel.getChild(treeModel.getRoot(), ResourceType.TILESET.ordinal());
        ResourceElement element = new ResourceElement(ResourceType.TILESET, tileset.getId(), tileset.getName());
        tilesetNode.add(new DefaultMutableTreeNode(element));
        repaint();
    }

    public void update(Observable o, Object arg) {
        repaint();
    }

    class EditorTreeModel extends DefaultTreeModel {

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

    class QuestDataTreeMouseAdapter extends MouseAdapter {

        @Override
        public void mousePressed(MouseEvent e) {
            DefaultMutableTreeNode clickedNode = null;
            try {
                TreePath selectionPath = QuestDataTree.this.getSelectionPath();
                if (e.getButton() == MouseEvent.BUTTON3) {
                    if (selectionPath == null) {
                        return;
                    }
                    clickedNode = (DefaultMutableTreeNode) selectionPath.getLastPathComponent();
                    if (clickedNode.getUserObject() instanceof ResourceElement) {
                        // right click: show a popup menu if the element is a map
                        int row = QuestDataTree.this.getRowForLocation(e.getX(), e.getY());
                        if (row == -1) {
                            return;
                        }
                        QuestDataTree.this.setSelectionRow(row);
                        clickedNode = (DefaultMutableTreeNode) QuestDataTree.this.getSelectionPath().getLastPathComponent();
                        ResourceElement element = (ResourceElement) clickedNode.getUserObject();
                        if (element.type == ResourceType.MAP) {
                            popupMenu.setMap(element.id);
                            popupMenu.show((JComponent) e.getSource(),
                                    e.getX(), e.getY());
                        }
                    }
                    else {
                        //Show the popup menu when right-click on map
                        if (clickedNode.getUserObject().equals(ResourceType.MAP.getName())) {
                            mapPopupMenu.show((JComponent) e.getSource(),
                                    e.getX(), e.getY());
                        }
                        //Popup menu when right-click on tileset
                        else if (clickedNode.getUserObject().equals(ResourceType.TILESET.getName())) {
                            tilesetPopupMenu.show((JComponent) e.getSource(),
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

                        switch (element.type) {

                            case MAP:
                            {
                                Map map = new Map(element.id);
                                MapEditorPanel mapEditor = new MapEditorPanel(editorWindow, map);
                                editorWindow.addEditor(mapEditor);
                                map.addObserver(editorWindow);
                                break;
                            }

                            case TILESET:
                            {
                                Tileset tileset = new Tileset(element.id);
                                TilesetEditorPanel tileEditor = new TilesetEditorPanel(editorWindow, tileset);
                                editorWindow.addEditor(tileEditor);
                                break;
                            }

                            case LANGUAGE:
                            {
                                /* TODO uncomment this when the dialog editor works
                                Dialogs dialogs = new Dialogs(element.id);
                                DialogsEditorPanel dialogsEditor = new DialogsEditorPanel(quest, editorWindow, dialogs);
                                editorWindow.addEditor(dialogsEditor);
                                */
                                File file = Project.getDialogsFile(element.id);
                                TextEditorPanel fileEditor = new TextEditorPanel(editorWindow, file);

                                editorWindow.addEditor(fileEditor);
                                break;
                            }

                            case ENEMY:
                            {
                                File f = new File(Project.getEnemyScriptFile(element.id));
                                TextEditorPanel fileEditor = new TextEditorPanel(editorWindow, f);
                                editorWindow.addEditor(fileEditor);
                                break;
                            }

                            case ITEM:
                            {
                                File f = new File(Project.getItemScriptFile(element.id));
                                TextEditorPanel fileEditor = new TextEditorPanel(editorWindow, f);
                                editorWindow.addEditor(fileEditor);
                                break;
                            }

                            case SPRITE:
                            {
                                File f = new File(Project.getSpriteFile(element.id).getAbsolutePath());
                                TextEditorPanel fileEditor = new TextEditorPanel(editorWindow, f);
                                editorWindow.addEditor(fileEditor);
                                break;
                            }
                        }
                    }
                }
            }
            catch (QuestEditorException ex) {
                GuiTools.errorDialog(ex.getMessage());
            }
        }
    }

    /**
     * Popup menu associated to the right click on the map sub-tree.
     */
    class QuestDataTreeMapPopupMenu extends JPopupMenu implements ActionListener {    

        private JMenuItem newMapMenu;
        
        public QuestDataTreeMapPopupMenu() {
            newMapMenu = new JMenuItem("New Map");
            newMapMenu.addActionListener(this);
            add(newMapMenu);            
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            MapEditorPanel mapEditor = new MapEditorPanel(editorWindow);
            mapEditor.newMap();
            editorWindow.addEditor(mapEditor);
            mapEditor.getMap().addObserver(editorWindow);
        }
    }

    /**
     * Popup menu associated to the right click on the tileset sub-tree.
     */
    class QuestDataTreeTilesetPopupMenu extends JPopupMenu implements ActionListener {    

        private JMenuItem newTilesetMenu;
        
        public QuestDataTreeTilesetPopupMenu() {
            newTilesetMenu = new JMenuItem("New Tileset");
            newTilesetMenu.addActionListener(this);
            add(newTilesetMenu);            
        }

        @Override
        public void actionPerformed(ActionEvent e) {
            TilesetEditorPanel tilesetEditor = new TilesetEditorPanel(editorWindow);
            tilesetEditor.newTileset();
            editorWindow.addEditor(tilesetEditor);            
        }
    }

    /**
     * General popup menu of the tree.
     */
    class QuestDataTreePopupMenu extends JPopupMenu implements ActionListener {

        private String mapId;
        private JMenuItem mapMenu, scriptMenu, deleteMenu;

        public QuestDataTreePopupMenu() {
            mapMenu = new JMenuItem("Open Map");
            add(mapMenu);
            mapMenu.addActionListener(this);
            scriptMenu = new JMenuItem("Open Map Script");
            add(scriptMenu);
            scriptMenu.addActionListener(this);
            deleteMenu = new JMenuItem("Delete Map");
            add(deleteMenu);
            deleteMenu.addActionListener(this);
        }

        public void actionPerformed(ActionEvent e) {

            if (e.getSource() == mapMenu) {
                // Open the map.
                try {
                    Map map = new Map(mapId);
                    MapEditorPanel mapEditor = new MapEditorPanel(editorWindow, map);
                    editorWindow.addEditor(mapEditor);
                    map.addObserver(editorWindow);
                }
                catch (QuestEditorException ex) {
                    GuiTools.errorDialog("Could not load the map: " + ex.getMessage());
                }
            }

            else if (e.getSource() == deleteMenu) {
                // Delete the map.
                try {
                    int answer = JOptionPane.showConfirmDialog(this,
                            "Are you sure you want to delete map '" + mapId + "' ?",
                            "Are you sure ?",
                            JOptionPane.YES_NO_OPTION,
                            JOptionPane.WARNING_MESSAGE);
                    if (answer == JOptionPane.YES_OPTION) {
                        Map.delete(mapId);
                        Project.getResource(ResourceType.MAP).removeElement(mapId);
     
                        // TODO: Do it in a cleaner way
                        // Here we reload the whole tree, which is not 
                        // the most optimized way of removing the child
                        // from the tree.
                        rebuildTree();
                        
                        repaint();
                    }
                }
                catch (QuestEditorException ex) {
                    GuiTools.errorDialog("Could not delete the map: " + ex.getMessage());
                }
            } else {
                // open the script
                File mapScritFile = Project.getMapScriptFile(mapId);
                TextEditorPanel fileEditor = new TextEditorPanel(editorWindow, mapScritFile);
                editorWindow.addEditor(fileEditor);
            }
        }

        public void setMap(String mapId) {
            this.mapId = mapId;
        }
    }

    /**
     * Stores the id of an element from a resource, its name and its
     * resource type.
     */
    class ResourceElement {

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
