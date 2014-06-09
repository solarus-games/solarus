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

import java.io.File;
import java.util.Vector;
import javax.swing.*;
import javax.swing.tree.*;
import org.solarus.editor.Project;
import org.solarus.editor.SpriteException;

/**
 * A tree that shows the imags list in the sprites folder of the game.
 */
public class SpriteImageTree extends JTree {

    /**
     * Creates a sprite image tree.
     * @throws SpriteException if the sprite folder doesn't exists
     */
    public SpriteImageTree() throws SpriteException {

        setModel(null);  // Because Java makes a stupid example tree by default.
        setExpandsSelectedPaths(true);
        rebuildTree();
    }

    /**
     * Rebuilds the whole tree from the resources.
     * @throws SpriteException if the sprite folder doesn't exists
     */
    private void rebuildTree() throws SpriteException {

        DefaultMutableTreeNode root = new DefaultMutableTreeNode("sprites");
        DefaultTreeModel model = new DefaultTreeModel(root);
        model.setAsksAllowsChildren(true);

        File folder = Project.getSpriteDir();
        if (folder.isDirectory()) {
            buildFolderNode(root, folder);
        }
        else {
            throw new SpriteException("The sprite folder doesn't exists");
        }

        setModel(model);
        repaint();
    }

    /**
     * Builds the subtree of a folder node.
     * @param folderNode The node where to built the subtree.
     */
    private void buildFolderNode(DefaultMutableTreeNode folderNode, File folder) {

        Vector<File> files = new Vector<File>();

        for (File file: folder.listFiles()) {

            if (file.isDirectory()) {
                DefaultMutableTreeNode child = new DefaultMutableTreeNode(file.getName());
                buildFolderNode(child, file);
                folderNode.add(child);
            }
            else if (file.getName().endsWith(".png")) {
                files.add(file);
            }
        }

        for (File file: files) {
            folderNode.add(new DefaultMutableTreeNode(file.getName(), false));
        }
    }

    /**
     * Returns a sub node with a specific name.
     * @param node the parent node
     * @param name the name to search
     * @return the found node or null if the node doesn't exists
     */
    private DefaultMutableTreeNode getSubNode(DefaultMutableTreeNode node, String name) {

        DefaultMutableTreeNode child = (DefaultMutableTreeNode) node.getFirstChild();
        while (child != null) {
            String childName = (String) child.getUserObject();
            if (childName.equals(name)) {
                return child;
            }
            child = child.getNextSibling();
        }
        return null;
    }

    /**
     * Select a file.
     * @param filename the file to select
     */
    public void setSelectedFile(String filename) {

        String[] list = filename.split("/");

        DefaultMutableTreeNode node = (DefaultMutableTreeNode) getModel().getRoot();

        for (int i = 0; i < list.length; i++) {

            if (node != null) {
                node = getSubNode(node, list[i]);
            } else {
                break;
            }
        }

        if (node != null) {
            setSelectionPath(new TreePath(node.getPath()));
            repaint();
        }
    }

    /**
     * Returns the selected file name.
     * @return the selected file name or null if no file is selected
     */
    public String getSelectedFile () {

        String image = "";
        DefaultMutableTreeNode node = null;
        TreePath path = getSelectionPath();

        if (path != null) {
            node = (DefaultMutableTreeNode) path.getLastPathComponent();
            // don't select folders
            if (node.getAllowsChildren()) {
                node = null;
            }
        }
        while (node != null) {
            if (!node.isRoot()) {
                image = ((String) node.getUserObject()) + (image.isEmpty() ? "" : "/" + image);
            }
            node = (DefaultMutableTreeNode) node.getParent();
        }
        return image;
    }
}
