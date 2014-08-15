package org.solarus.editor.gui;

import java.util.Observable;
import java.util.Observer;

import javax.swing.JTree;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;

import org.solarus.editor.Sprite;
import org.solarus.editor.SpriteAnimation;

public class SpriteTree extends JTree implements Observer {

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
  
        DefaultTreeModel model = new DefaultTreeModel(root);
        setModel(model);

        if (sprite == null) {
            return;
        }

        // Add a node for each animation.
        for (String animationName: sprite.getAnimationNames()) {
            DefaultMutableTreeNode animationNode = new DefaultMutableTreeNode(animationName);
            animationNode.setAllowsChildren(true);
            root.add(animationNode);

            // Add a node for each direction of this animation.
            SpriteAnimation animation = sprite.getAnimation(animationName);
            for (int i = 0; i < animation.getNbDirections(); i++) {
                DefaultMutableTreeNode directionNode = new DefaultMutableTreeNode(
                        "Direction " + animation.getDirectionName(i)
                );
                directionNode.setAllowsChildren(false);
                animationNode.add(directionNode);
            }

            // Expand the node of the default animation.
            if (animationName.equals(sprite.getDefaultAnimationName())) {
                TreePath path = new TreePath(new Object[] { root, animationNode } );
                expandPath(path);
            }

        }
        expandRow(0);  // Expand the root node.
    }

    @Override
    public void update(Observable o, Object param) {
        buildTree();
    }

}
