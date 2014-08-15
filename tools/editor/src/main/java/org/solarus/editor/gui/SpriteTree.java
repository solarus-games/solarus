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
import org.solarus.editor.SpriteAnimationDirection;

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
                new DefaultMutableTreeNode(new NodeInfo(sprite, "Animations"));
        root.setAllowsChildren(true);  // Show node as a folder even if empty.
  
        getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);

        if (sprite == null) {
            return;
        }

        TreePath selectionPath = null;

        // Add a node for each animation.
        for (String animationName: sprite.getAnimationNames()) {
            SpriteAnimation animation = sprite.getAnimation(animationName);
            DefaultMutableTreeNode animationNode = new DefaultMutableTreeNode(
                    new NodeInfo(animation, animationName));
            animationNode.setAllowsChildren(true);
            root.add(animationNode);

            // Add a node for each direction of this animation.
            for (int i = 0; i < animation.getNbDirections(); i++) {
                SpriteAnimationDirection direction = animation.getDirection(i);
                DefaultMutableTreeNode directionNode = new DefaultMutableTreeNode(
                        new NodeInfo(direction, "Direction " + animation.getDirectionName(i))
                );
                directionNode.setAllowsChildren(false);
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

            if (selectionPath == null &&
                    animationName.equals(sprite.getSelectedAnimationName())) {
                // Initially select the current animation and no direction.
                selectionPath = new TreePath(new Object[] {
                        root,
                        animationNode
                });
            }
        }

        DefaultTreeModel model = new DefaultTreeModel(root);
        model.setAsksAllowsChildren(true);
        setModel(model);

        expandRow(0);  // Expand the root node.
        expandPath(selectionPath);  // Expand the selected animation and direction.
        setSelectionPath(selectionPath);
        scrollPathToVisible(selectionPath);

        // TODO show the icon of each direction
    }

    /**
     * Updates this component.
     * @param o The object that has changed.
     * @param info Info about what has changed, or null to update everything.
     */
    @Override
    public void update(Observable o, Object info) {

        if (o == null) {
            return;
        }

        if (o instanceof Sprite) {

            if (info == null) {
                // Rebuild everything.
                buildTree();
                return;
            }

            Sprite.Change change = (Sprite.Change) info;
            switch (change.getWhatChanged()) {

            case ANIMATION_ADDED:
                // TODO add a new node
                break;

            case ANIMATION_REMOVED:
                // TODO remove the node
                break;

            case ANIMATION_RENAMED:
                // TODO change the node text
                break;

            case DEFAULT_ANIMATION_CHANGED:
                // Nothing to do.
                break;

            case SELECTED_ANIMATION_CHANGED:
                // TODO make the animation and direction selected in the tree.
                break;

            case DIRECTION_ADDED:
                // TODO add a new node
                break;

            case DIRECTION_REMOVED:
                // TODO remove the new node
                break;

            case SELECTED_DIRECTION_CHANGED:
                // TODO make the animation and direction selected in the tree.
                break;

            case SOURCE_IMAGE_REFRESHED:
                // The icons may have changed.
                break;
            }
        }
    }

    /**
     * Event called when the user changes the selection in the tree.
     */
    @Override
    public void valueChanged(TreeSelectionEvent event) {

        try {
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) getLastSelectedPathComponent();
            NodeInfo info = (NodeInfo) node.getUserObject();

            Object data = info.getData();
            if (data instanceof Sprite) {
                // The sprite itself: root node.
                sprite.unselectAnimation();
            }

            else if (data instanceof SpriteAnimation) {
                // A sprite animation: show it and show no direction.
                SpriteAnimation animation = (SpriteAnimation) data;
                sprite.unselectDirection();
                sprite.setSelectedAnimation(animation.getName());
            }

            else if (data instanceof SpriteAnimationDirection) {
                // A direction: show it and show its parent animation.
                SpriteAnimationDirection direction = (SpriteAnimationDirection) data;
                sprite.setSelectedAnimation(direction.getAnimation());
                sprite.setSelectedDirection(direction);
            }
        }
        catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    /**
     * Data to be used as user object in the nodes of the tree.
     */
    private class NodeInfo {

        private Object data;

        private String text;

        /**
         * Constructor.
         * @param data Data associated to the node.
         * @param text Text representation to show for the node.
         */
        public NodeInfo(Object data, String text) {
            this.data = data;
            this.text = text;
        }

        public Object getData() {
            return data;
        }
        
        public String toString() {
            return text;
        }
    }

}
