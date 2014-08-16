package org.solarus.editor.gui;

import java.awt.Component;
import java.awt.Image;
import java.util.NoSuchElementException;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JTree;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
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

        DefaultMutableTreeNode root = createSpriteNode();

        getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        setCellRenderer(new SpriteCellRenderer());

        if (sprite == null) {
            return;
        }

        TreePath selectionPath = null;

        // Add a node for each animation.
        for (String animationName: sprite.getAnimationNames()) {
            SpriteAnimation animation = sprite.getAnimation(animationName);
            DefaultMutableTreeNode animationNode = createAnimationNode(animationName);
            root.add(animationNode);

            // Add a node for each direction of this animation.
            for (int i = 0; i < animation.getNbDirections(); i++) {
                DefaultMutableTreeNode directionNode = createDirectionNode(animationName, i);
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
     * Creates a node to be used as root element of the tree.
     * @return The sprite node created.
     */
    private DefaultMutableTreeNode createSpriteNode() {

        DefaultMutableTreeNode spriteNode = new DefaultMutableTreeNode(new NodeInfo(sprite, "Animations"));
        spriteNode.setAllowsChildren(true);  // Show node as a folder even if empty.
        return spriteNode;
    }

    /**
     * Returns the node associated to the sprite, that is, the root node.
     */
    private DefaultMutableTreeNode getSpriteNode() {
        return (DefaultMutableTreeNode) getModel().getRoot();
    }

    /**
     * Creates a node associated to the specified animation.
     * @param animationName Name of an animation.
     * @return The node created.
     */
    private DefaultMutableTreeNode createAnimationNode(String animationName) {

        SpriteAnimation animation = sprite.getAnimation(animationName);
        animation.addObserver(this);
        DefaultMutableTreeNode animationNode = new DefaultMutableTreeNode(
                new NodeInfo(animation, animationName));
        animationNode.setAllowsChildren(true);

        return animationNode;
    }

    /**
     * Returns the node associated to the specified animation.
     * @param animationName Name of an animation.
     * @return The corresponding node of the tree.
     */
    private DefaultMutableTreeNode getAnimationNode(String animationName) {

        DefaultMutableTreeNode root = getSpriteNode();
        for (int i = 0; i < root.getChildCount(); i++) {
            DefaultMutableTreeNode animationNode = (DefaultMutableTreeNode) root.getChildAt(i);
            NodeInfo info = (NodeInfo) animationNode.getUserObject();
            SpriteAnimation animation = (SpriteAnimation) info.getData();
            if (animation.getName().equals(animationName)) {
                return animationNode;
            }
        }

        throw new NoSuchElementException("No such animation: '" + animationName + "'");
    }

    /**
     * Creates a node for the specified animation and direction.
     * @param animationName Name of an animation.
     * @param directionIndex A direction.
     * @return The created node.
     */
    private DefaultMutableTreeNode createDirectionNode(String animationName, int directionIndex) {

        SpriteAnimation animation = sprite.getAnimation(animationName);
        SpriteAnimationDirection direction = animation.getDirection(directionIndex);
        direction.addObserver(this);
        DefaultMutableTreeNode directionNode = new DefaultMutableTreeNode(new NodeInfo(
                direction, "Direction " + animation.getDirectionName(directionIndex))
        );
        directionNode.setAllowsChildren(false);

        return directionNode;
    }

    /**
     * Returns the node associated to the specified animation and direction.
     * @param animationName Name of an animation.
     * @param direction A direction.
     * @return The corresponding node of the tree.
     */
    private DefaultMutableTreeNode getDirectionNode(String animationName, int direction) {

        DefaultMutableTreeNode animationNode = getAnimationNode(animationName);
        return (DefaultMutableTreeNode) animationNode.getChildAt(direction);
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

            // Local change in the tree.
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
                updateSelection();
                break;

            case DIRECTION_ADDED:
                // TODO add a new node
                break;

            case DIRECTION_REMOVED:
                // TODO remove the new node
                break;

            case SELECTED_DIRECTION_CHANGED:
                updateSelection();
                break;

            }
        }

        else if (o instanceof SpriteAnimation) {
            if (info instanceof Image) {
                // The source image has changed.
                repaint();
            }
        }
    }

    /**
     * Event called when the user changes the selection in the tree.
     * The corresponding element gets selected in the sprite.
     */
    @Override
    public void valueChanged(TreeSelectionEvent event) {

        try {
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) getLastSelectedPathComponent();

            if (node == null) {
                // Nothing is selected in the tree.
                sprite.unselectAnimation();
                return;
            }

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
     * Selects in the tree what is selected in the sprite.
     */
    private void updateSelection() {

        String animation = sprite.getSelectedAnimationName();
        int direction = sprite.getSelectedDirectionNb();
        TreePath path = null;

        if (animation.isEmpty()) {
            // Nothing is selected selected.
            path = new TreePath(new Object[] {
                    getSpriteNode()
            });
        }
        else if (direction == -1) {
            // An animation is selected and no direction is selected.
            path = new TreePath(new Object[] {
                    getSpriteNode(),
                    getAnimationNode(animation)
            });
        }
        else {
            // An animation and direction are selected.
            path = new TreePath(new Object[] {
                    getSpriteNode(),
                    getAnimationNode(animation),
                    getDirectionNode(animation, direction)
            });
        }
        getSelectionModel().setSelectionPath(path);
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

    /**
     * Cell rendered used to show a custom icon for each direction node.
     */
    private class SpriteCellRenderer extends DefaultTreeCellRenderer {

        @Override
        public Component getTreeCellRendererComponent(
                JTree tree,
                Object node,
                boolean selected,
                boolean expanded,
                boolean leaf,
                int row,
                boolean hasFocus
        ) {

            super.getTreeCellRendererComponent(
                    tree,
                    node,
                    selected,
                    expanded,
                    leaf,
                    row,
                    hasFocus
            );

            NodeInfo info = (NodeInfo) ((DefaultMutableTreeNode) node).getUserObject();
            if (info.getData() instanceof SpriteAnimationDirection) {
                // This is a direction node: use the appropriate icon.
                SpriteAnimationDirection direction = (SpriteAnimationDirection) info.getData();
                setIcon(new SpriteAnimationDirectionIcon(
                        sprite,
                        direction.getAnimation(),
                        direction)
                );
            }

            return this;
        }

    }
}
