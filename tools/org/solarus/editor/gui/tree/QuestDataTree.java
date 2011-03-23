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
package org.solarus.editor.gui.tree;

import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.MouseAdapter;
import java.awt.event.MouseEvent;
import java.io.File;
import java.util.Observable;
import java.util.Observer;
import javax.swing.JComponent;
import javax.swing.JMenuItem;
import javax.swing.JPopupMenu;
import javax.swing.JTree;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import org.solarus.editor.Dialogs;
import org.solarus.editor.Map;
import org.solarus.editor.Project;
import org.solarus.editor.Resource;
import org.solarus.editor.ResourceType;
import org.solarus.editor.ZSDXException;
import org.solarus.editor.entities.Tileset;
import org.solarus.editor.gui.DialogsEditorWindow;
import org.solarus.editor.gui.EditorWindow;
import org.solarus.editor.gui.FileEditorWindow;
import org.solarus.editor.gui.MapEditorWindow;
import org.solarus.editor.gui.TilesetEditorWindow;

/**
 * The Quest data tree
 */
public class QuestDataTree extends JTree implements TreeSelectionListener, Observer {

    private String quest;
    private EditorWindow editorWindow;
    private QuestDataTreePopupMenu popupMenu;

    public QuestDataTree(String quest, EditorWindow parent) {
        this.quest = quest;
        this.editorWindow = parent;
        addTreeSelectionListener(this);
        addMouseListener(new QuestDataTreeMouseAdapter());
        popupMenu = new QuestDataTreePopupMenu();
        //

    }

    public void setRoot(String projectPath) {
        setModel(new EditorTreeModel(projectPath));
        repaint();
    }

    public void valueChanged(TreeSelectionEvent e) {
        //System.out.println(e.getNewLeadSelectionPath());
    }

    public void addMap(Map map) {
        DefaultMutableTreeNode mapNode = (DefaultMutableTreeNode) treeModel.getChild(treeModel.getRoot(), ResourceType.MAP.ordinal());
        ((EditorTreeModel) treeModel).insertNodeInto(new DefaultMutableTreeNode(map), mapNode, mapNode.getChildCount());
        map.addObserver(this);
        repaint();
    }

    public void addTileset(Tileset tileset) {
        DefaultMutableTreeNode tilesetNode = (DefaultMutableTreeNode) treeModel.getChild(treeModel.getRoot(), ResourceType.TILESET.ordinal());
        tilesetNode.add(new DefaultMutableTreeNode(tileset));
        repaint();
    }

    public void update(Observable o, Object arg) {
        repaint();
    }

    class EditorTreeModel extends DefaultTreeModel {

        public EditorTreeModel(String path) {
            super((new DefaultMutableTreeNode("Quest")));
            for (ResourceType resourceType : ResourceType.values()) {
                DefaultMutableTreeNode noeudResource = new DefaultMutableTreeNode(resourceType.getName());
                ((DefaultMutableTreeNode) getRoot()).add(noeudResource);
                if (Project.isLoaded()) {
                    addChildren(noeudResource, resourceType);
                }
            }
        }

        @Override
        public boolean isLeaf(Object e) {
            return ((DefaultMutableTreeNode) e).getChildCount() == 0 || !(((DefaultMutableTreeNode) e).getUserObject() instanceof String);
        }

        protected final void addChildren(DefaultMutableTreeNode parentNode, ResourceType resourceType) {
            try {
                Resource resource = Project.getResource(resourceType);
                String[] ids = resource.getIds();

                for (int i = 0; i < ids.length; i++) {
                    switch (resourceType) {
                        case MAP:
                            parentNode.add(new DefaultMutableTreeNode(new Map(ids[i])));
                            break;
                        case TILESET:
                            parentNode.add(new DefaultMutableTreeNode(new Tileset(ids[i])));
                            break;
                        case DIALOGS:
                            parentNode.add(new DefaultMutableTreeNode(new Dialogs(ids[i])));
                            break;
                        case ENEMY:
                            parentNode.add(new DefaultMutableTreeNode(new CustomFile(Project.getEnemyScriptFile(ids[i]), resource.getElementName(ids[i]))));
                            break;
                        case ITEM:
                            parentNode.add(new DefaultMutableTreeNode(new CustomFile(Project.getItemScriptFile(ids[i]), resource.getElementName(ids[i]))));
                            break;
                        case SPRITE:
                            parentNode.add(new DefaultMutableTreeNode(new CustomFile(Project.getSpriteFile(ids[i]).getAbsolutePath(), resource.getElementName(ids[i]))));
                            break;
                        default:
                            //CustomFile cf = new CustomFile(resourceName)
                            parentNode.add(new DefaultMutableTreeNode(resource.getElementName(ids[i])));
                            break;
                    }
                }
            } catch (ZSDXException zsdxe) {
            }
        }
    }

    class QuestDataTreeMouseAdapter extends MouseAdapter {

        @Override
        public void mousePressed(MouseEvent e) {
            DefaultMutableTreeNode clickedNode = null;
            try {
                if (e.getButton() == MouseEvent.BUTTON3) {
                    int row = QuestDataTree.this.getRowForLocation(e.getX(), e.getY());
                    if (row == -1) {
                        return;
                    }
                    QuestDataTree.this.setSelectionRow(row);
                    clickedNode = (DefaultMutableTreeNode) QuestDataTree.this.getSelectionPath().getLastPathComponent();
                    if (clickedNode.getUserObject() instanceof Map) {
                        popupMenu.setMap((Map) clickedNode.getUserObject());
                        popupMenu.show((JComponent) e.getSource(),
                                e.getX(), e.getY());
                    }

                } else if (e.getClickCount() == 2) {


                    clickedNode = (DefaultMutableTreeNode) QuestDataTree.this.getSelectionPath().getLastPathComponent();

                    if (clickedNode.getUserObject() instanceof String) {
                        //
                    } else if (clickedNode.getUserObject() instanceof Map) {
                        Map m = ((Map) clickedNode.getUserObject());
                        MapEditorWindow mapEditor = new MapEditorWindow(quest, editorWindow, m);
                        editorWindow.addEditor(mapEditor);
                        m.addObserver(editorWindow);
                    } else if (clickedNode.getUserObject() instanceof Tileset) {
                        Tileset t = ((Tileset) clickedNode.getUserObject());

                        TilesetEditorWindow tileEditor = new TilesetEditorWindow(quest, editorWindow, t);
                        editorWindow.addEditor(tileEditor);
                    } else if (clickedNode.getUserObject() instanceof Dialogs) {
                        Dialogs d = ((Dialogs) clickedNode.getUserObject());
                        DialogsEditorWindow dialogsEditor = new DialogsEditorWindow(quest, editorWindow, d);
                        editorWindow.addEditor(dialogsEditor);
                    } else {
                        CustomFile cf = ((CustomFile) clickedNode.getUserObject());
                        FileEditorWindow fileEditor = new FileEditorWindow(quest, editorWindow, new File(cf.getAbsolutePath()));
                        editorWindow.addEditor(fileEditor);
                    }

                }
            } catch (ClassCastException cce) {
                System.out.println("Le noeud sur lequel on a cliqué est : " + clickedNode);
                cce.printStackTrace();
            } catch (NullPointerException npe) {
            }
        }
    }

    class QuestDataTreePopupMenu extends JPopupMenu implements ActionListener {

        private Map map;
        private JMenuItem mapMenu, scriptMenu;

        public QuestDataTreePopupMenu() {
            mapMenu = new JMenuItem("Open Map");
            add(mapMenu);
            mapMenu.addActionListener(this);
            scriptMenu = new JMenuItem("Open Map Script");
            add(scriptMenu);
            scriptMenu.addActionListener(this);

        }

        public void actionPerformed(ActionEvent e) {
            if (e.getSource() == mapMenu) {
                MapEditorWindow mapEditor = new MapEditorWindow(quest, editorWindow, map);
                editorWindow.addEditor(mapEditor);
                map.addObserver(editorWindow);
            } else {
                String mapId = map.getId();
                File mapScritFile = Project.getMapScriptFile(mapId);
                FileEditorWindow fileEditor = new FileEditorWindow(quest, editorWindow, mapScritFile);
                editorWindow.addEditor(fileEditor);
            }
        }

        public void setMap(Map map) {
            this.map = map;
        }
    }

    class CustomFile extends File {

        String resourceName;

        public CustomFile(String s, String n) {
            super(s);
            this.resourceName = n;
        }

        @Override
        public String toString() {
            return resourceName;
        }
    }
}
