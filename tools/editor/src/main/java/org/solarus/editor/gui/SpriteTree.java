package org.solarus.editor.gui;

import java.util.Observable;
import java.util.Observer;

import javax.swing.JTree;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import org.solarus.editor.Sprite;
import org.solarus.editor.SpriteAnimation;

public class SpriteTree extends JTree implements Observer, TreeSelectionListener {

    /**
     * The sprite observed.
     */
    private Sprite sprite;

    /**
     * Constructor.
     */
    public SpriteTree() {
        setModel(null);
        buildTree();
        addTreeSelectionListener(this);
    }

    /**
     * Sets the sprite observed.
     * @param sprite the sprite
     */
    public void setSprite(Sprite sprite) {

        if (this.sprite != sprite) {
            if (this.sprite != null) {
                this.sprite.deleteObserver(this);
            }

            this.sprite = sprite;

            if (sprite != null) {
                sprite.addObserver(this);
            }
            update(sprite, null);
        }
    }

    /**
     * Builds or rebuilds all nodes of the tree.
     */
    private void buildTree() {

        DefaultMutableTreeNode root =
                new DefaultMutableTreeNode("Animations");
        root.setAllowsChildren(true);  // Show node as a folder even if empty.
        // TODO root.setUserObject(sprite);
  
        DefaultTreeModel model = new DefaultTreeModel(root);
        getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        setModel(model);

        if (sprite == null) {
            return;
        }

        TreePath selectionPath = null;

        // Add a node for each animation.
        for (String animationName: sprite.getAnimationNames()) {
            SpriteAnimation animation = sprite.getAnimation(animationName);
            DefaultMutableTreeNode animationNode = new DefaultMutableTreeNode(animationName);
            animationNode.setAllowsChildren(true);
         // TODO animationNode.setUserObject(animation);
            root.add(animationNode);

            // Add a node for each direction of this animation.
            for (int i = 0; i < animation.getNbDirections(); i++) {
                DefaultMutableTreeNode directionNode = new DefaultMutableTreeNode(
                        "Direction " + animation.getDirectionName(i)
                );
                directionNode.setAllowsChildren(false);
             // TODO directionNode.setUserObject(animation.getDirection(i));
                animationNode.add(directionNode);

                // Initially select the sprite's current animation and direction.
                if (animationName.equals(sprite.getSelectedAnimationName()) &&
                        i == sprite.getSelectedDirectionNb()) {
                    selectionPath = new TreePath(new Object[] {
                            root,
                            animationNode,
                            directionNode
                    });
                }
            }
        }
        expandRow(0);  // Expand the root node.
        setSelectionPath(selectionPath);
    }

    @Override
    public void update(Observable o, Object param) {
        // TODO keep the expanded state
        buildTree();
    }

    /**
     * Event called when the user changes the selection in the tree.
     */
    @Override
    public void valueChanged(TreeSelectionEvent event) {
        // TODO
    }

}
