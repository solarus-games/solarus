/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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

import imagej.util.swing.tree.*;
import java.awt.event.*;
import java.util.NoSuchElementException;
import java.util.Observable;
import java.util.Observer;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.tree.*;
import java.lang.reflect.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.EntityType;
import org.solarus.editor.entities.EntitySubtype;

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
public class QuestTree extends JTree implements ProjectObserver, Observer {

    private EditorWindow editorWindow;  // The main window.
    
    /**
     * Creates a quest tree.
     * @param parent The main quest editor window.
     */
    public QuestTree(EditorWindow parent) {
        setModel(null);  // Because Java makes a stupid example tree by default.
        this.editorWindow = parent;

        final CheckBoxNodeRenderer renderer = new CheckBoxNodeRenderer();
        setCellRenderer(renderer);

        final CheckBoxNodeEditor editor = new CheckBoxNodeEditor(this);
        setCellEditor(editor);
        setEditable(true);

        addMouseListener(new QuestTreeMouseAdapter());

        Project.addProjectObserver(this);
    }

    /**
     * Called when another project becomes active.
     */
    @Override
    public void currentProjectChanged() {
        rebuildTree();

        // Initially expand maps.
        expandRow(ResourceType.MAP.ordinal() + 1);
    }

    /**
     * Called when an observed model changes.
     * @param model The model.
     * @param param Parameter.
     */
    @Override
    public void update(Observable model, Object param) {

        if (model instanceof MapViewSettings) {
            MapViewSettings mapViewSettings = (MapViewSettings) model;
            Map map = mapViewSettings.getMap();
            DefaultMutableTreeNode mapNode = getResourceElementNode(
                    ResourceType.MAP, map.getId());

            DefaultMutableTreeNode child =
                    (DefaultMutableTreeNode) mapNode.getFirstChild();
            while (child != null) {
                CheckBoxNodeData data =
                        (CheckBoxNodeData) child.getUserObject();
                EntityType entityType = EntityType.values()[mapNode.getIndex(child)];
                boolean shown = mapViewSettings.getShowEntityType(entityType);
                data.setChecked(shown);
                child = child.getNextSibling();
            }
        }
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
        DefaultMutableTreeNode resourceNode = getResourceElementNode(resourceType, id);
        resourceNode.removeFromParent();
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

        try {
            DefaultMutableTreeNode resourceNode = getResourceElementNode(resourceType, oldId);
            resourceNode.setUserObject(new ResourceElement(resourceType, newId));
            repaint();
        }
        catch (QuestEditorException ex) {
            // Should not happen.
            ex.printStackTrace();
        }
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
            DefaultMutableTreeNode resourceNode = getResourceElementNode(resourceType, id);
            resourceNode.setUserObject(new ResourceElement(resourceType, id));
            repaint();
        }
        catch (QuestEditorException ex) {
            // Should not happen.
            ex.printStackTrace();
        }
    }

    /**
     * Rebuilds the whole tree from the resources.
     */
    public void rebuildTree() {

        DefaultMutableTreeNode root = new DefaultMutableTreeNode("Quest");
        DefaultTreeModel model = new DefaultTreeModel(root);
        model.setAsksAllowsChildren(true);
        
        // Create a parent node for each type of resource:
        // map, tileset, sound, etc.
        for (ResourceType resourceType: ResourceType.values()) {
            DefaultMutableTreeNode resourceTypeNode =
                    new DefaultMutableTreeNode(resourceType);
            root.add(resourceTypeNode);
            if (Project.isLoaded()) {
                buildResourceTypeChildren(resourceTypeNode);
            }
        }

        setModel(model);

        treeModel.addTreeModelListener(new TreeModelListener() {

            @Override
            public void treeNodesChanged(final TreeModelEvent event) {

                Object[] children = event.getChildren();
                for (final Object child: children) {
                    DefaultMutableTreeNode node = (DefaultMutableTreeNode) child;
                    final Object userObject = node.getUserObject();

                    if (node.getLevel() == 3 && userObject instanceof CheckBoxNodeData) {
                        final CheckBoxNodeData data =
                                (CheckBoxNodeData) userObject;
                        DefaultMutableTreeNode mapNode = (DefaultMutableTreeNode) node.getParent();
                        int index = mapNode.getIndex(node);
                        EntityType entityType = EntityType.values()[index];

                        String mapId = ((ResourceElement) mapNode.getUserObject()).id;
                        Map map = editorWindow.getOpenMap(mapId);
                        map.getViewSettings().setShowEntityType(
                                entityType, data.isChecked());
                    }
                }
            }

            @Override
            public void treeNodesInserted(final TreeModelEvent event) {
            }

            @Override
            public void treeNodesRemoved(final TreeModelEvent event) {
            }

            @Override
            public void treeStructureChanged(final TreeModelEvent event) {
            }
        });

        repaint();
    }

    /**
     * Builds the subtree of a resource type node.
     * @param resourceTypeNode The node where to built the subtree.
     */
    private void buildResourceTypeChildren(DefaultMutableTreeNode resourceTypeNode) {

        ResourceType resourceType = (ResourceType) resourceTypeNode.getUserObject();
        Resource resource = Project.getResource(resourceType);
        String[] ids = resource.getIds();

        try {
            for (int i = 0; i < ids.length; i++) {
                DefaultMutableTreeNode elementNode = new DefaultMutableTreeNode(
                        new ResourceElement(resourceType, ids[i]));
                elementNode.setAllowsChildren(false);
                resourceTypeNode.add(elementNode);
            }
        }
        catch (QuestEditorException ex) {
            GuiTools.errorDialog("Unexpected error while building the quest tree: " + ex.getMessage());
        }
    }

    /**
     * Returns the model used by the quest tree.
     * @return The model.
     */
    private DefaultTreeModel getQuestTreeModel() {
        return (DefaultTreeModel) getModel();
    }

    /**
     * Returns the node that represents a specified resource type.
     * @param resourceType Type of resource.
     * @return The node of this resource.
     */
    private DefaultMutableTreeNode getResourceTypeNode(
            ResourceType resourceType) {

        return (DefaultMutableTreeNode) getModel().getChild(
                getModel().getRoot(), resourceType.ordinal());
    }

    /**
     * Returns the node that represents a specified resource element.
     * @param resourceType Type of resource.
     * @param id Id of the element to get.
     * @return The node of this element.
     * @throws NoSuchElementException If such a resource node was not found in
     * the tree.
     */
    private DefaultMutableTreeNode getResourceElementNode(ResourceType resourceType,
            String id) throws NoSuchElementException {

        DefaultMutableTreeNode typeNode = getResourceTypeNode(resourceType);
        DefaultMutableTreeNode candidateNode = (DefaultMutableTreeNode) typeNode.getFirstChild();
        while (candidateNode != null) {
            ResourceElement element = (ResourceElement) candidateNode.getUserObject(); 
            if (element.id.equals(id)) {
                return candidateNode;
            }
            candidateNode = candidateNode.getNextSibling();
        }

        throw new NoSuchElementException("Cannot find a " +
                resourceType.getName() + " node with id '" + id + "'");
    }

    /**
     * Adds a resource element to the tree.
     * @param element The resource element to add.
     */
    private void addResourceElementToTree(ResourceElement element) {
        DefaultMutableTreeNode resourceNode = getResourceTypeNode(element.type);
        getQuestTreeModel().insertNodeInto(
                new DefaultMutableTreeNode(element),
                resourceNode,
                resourceNode.getChildCount());
        repaint();
    }

    /**
     * This function is called when a map is being opened in the editor.
     * The quest tree then creates some nodes under that map.
     * @param map The map just opened.
     */
    public void openMap(Map map) {

        DefaultMutableTreeNode mapNode = getResourceElementNode(
                ResourceType.MAP, map.getId());
        mapNode.setAllowsChildren(true);
        mapNode.removeAllChildren();

        for (EntityType entityType: EntityType.values()) {
            final CheckBoxNodeData data = new CheckBoxNodeData(
                    entityType.getHumanName(),
                    map.getViewSettings().getShowEntityType(entityType));
            DefaultMutableTreeNode entityTypeNode =
                    new DefaultMutableTreeNode(data);
            entityTypeNode.setAllowsChildren(false);
            getQuestTreeModel().insertNodeInto(
                    entityTypeNode,
                    mapNode,
                    mapNode.getChildCount());
        }

        TreePath path = new TreePath(mapNode.getPath());
        expandPath(path);
        scrollPathToVisible(path);

        map.getViewSettings().addObserver(this);
        update(map.getViewSettings(), null);
    }

    /**
     * This function is called when a map is being closed in the editor.
     * The quest tree then removes all nodes under that map.
     * @param map The map being closed.
     */
    public void closeMap(Map map) {

        map.getViewSettings().deleteObserver(this);
        DefaultMutableTreeNode mapNode = getResourceElementNode(
                ResourceType.MAP, map.getId());
        mapNode.removeAllChildren();
        mapNode.setAllowsChildren(false);
        getQuestTreeModel().reload(mapNode);
        repaint();
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
                else if (clickedObject instanceof CheckBoxNodeData) {
                    // Right click on a map entity type.
                    final CheckBoxNodeData data =
                        (CheckBoxNodeData) clickedObject;
                    DefaultMutableTreeNode mapNode = (DefaultMutableTreeNode) clickedNode.getParent();
                    int index = mapNode.getIndex(clickedNode);
                    EntityType entityType = EntityType.values()[index];

                    String mapId = ((ResourceElement) mapNode.getUserObject()).id;
                    MapEditorPanel mapEditor = editorWindow.getOpenMapEditor(mapId);

                    if (mapEditor == null) {
                        throw new IllegalStateException("This map is not open");
                    }
                    popupMenu = new MapEntityTypePopupMenu(mapEditor, entityType);
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
     * Popup menu of a map entity type.
     */
    private class MapEntityTypePopupMenu extends JPopupMenu {

        /**
         * Creates a popup menu for the given type of map entity.
         * @param mapEditor A map editor currently open.
         * @param entityType A type of map entity.
         */
        public MapEntityTypePopupMenu(final MapEditorPanel mapEditor, final EntityType entityType) {

            // Create.
            String entityTypeHumanName = entityType.getHumanName();
            if (!entityType.hasSubtype()) {
                // Only one subtype.
                JMenuItem menuItem = new JMenuItem("Create " + entityTypeHumanName);
                add(menuItem);
                menuItem.addActionListener(new ActionListener() {
                    @Override
                    public void actionPerformed(ActionEvent e) {
                        mapEditor.getMapView().startAddingEntity(entityType, null);
                    }
                });
            }
            else {
                // Several subtypes: make a submenu to give the choice.
                Class<? extends EntitySubtype> subtypeEnum = entityType.getSubtypeEnum();
                try {
                    String[] humanNames = (String[]) subtypeEnum.getField("humanNames").get(null);
                    Enum<?>[] values = (Enum<?>[]) subtypeEnum.getMethod("values").invoke(null);

                    JMenu menu = new JMenu("Create " + entityTypeHumanName);
                    for (int i = 0; i < values.length; i++) {
                        final EntitySubtype entitySubtype = (EntitySubtype) values[i];
                        JMenuItem menuItem = new JMenuItem(humanNames[i]);
                        menu.add(menuItem);
                        menuItem.addActionListener(new ActionListener() {
                            @Override
                            public void actionPerformed(ActionEvent e) {
                                mapEditor.getMapView().startAddingEntity(entityType, entitySubtype);
                            }
                        });
                    }
                    add(menu);
                }
                catch (NoSuchFieldException ex) {
                    System.err.println("Field 'humanNames' is missing in enumeration " + subtypeEnum.getName());
                    ex.printStackTrace();
                    System.exit(1);
                }
                catch (NoSuchMethodException ex) {
                    System.err.println("Method 'values' is missing in enumeration " + subtypeEnum.getName());
                    ex.printStackTrace();
                    System.exit(1);
                }
                catch (IllegalAccessException ex) {
                    System.err.println("Cannot access a member in enumeration " + subtypeEnum.getName() + ": ex.getMessage()");
                    ex.printStackTrace();
                    System.exit(1);
                }
                catch (InvocationTargetException ex) {
                    ex.getCause().printStackTrace();
                    System.exit(1);
                }
            }
        }
    }
}

