package org.solarus.editor.gui;

import java.awt.Component;
import java.awt.Dimension;
import java.awt.GridBagConstraints;
import java.awt.GridBagLayout;
import java.awt.Image;
import java.awt.Rectangle;
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.NoSuchElementException;
import java.util.Observable;
import java.util.Observer;

import javax.swing.JButton;
import javax.swing.JLabel;
import javax.swing.JMenuItem;
import javax.swing.JOptionPane;
import javax.swing.JPanel;
import javax.swing.JPopupMenu;
import javax.swing.JScrollPane;
import javax.swing.JTree;
import javax.swing.event.TreeSelectionEvent;
import javax.swing.event.TreeSelectionListener;
import javax.swing.tree.DefaultMutableTreeNode;
import javax.swing.tree.DefaultTreeCellRenderer;
import javax.swing.tree.DefaultTreeModel;
import javax.swing.tree.TreePath;
import javax.swing.tree.TreeSelectionModel;

import org.solarus.editor.Project;
import org.solarus.editor.Sprite;
import org.solarus.editor.SpriteAnimation;
import org.solarus.editor.SpriteAnimationDirection;
import org.solarus.editor.SpriteException;

/**
 * A sprite view composed of:
 * - a tree of animations and directions on the left,
 * - some buttons on the right to add/remove/rename animations and directions.
 */
public class SpriteTree extends JPanel implements Observer, TreeSelectionListener {

    /**
     * The sprite observed.
     */
    private Sprite sprite;

    /**
     * The tree.
     */
    private JTree tree;

    // Buttons on the right.
    private JButton createButton;
    private JMenuItem createAnimationItem;
    private JMenuItem createDirectionItem;
    private JButton cloneButton;
    private JButton renameButton;
    private JButton deleteButton;

    /**
     * Constructor.
     */
    public SpriteTree() {
        super(new GridBagLayout());

        // Left part: the tree.
        tree = new JTree();
        buildTree();
        tree.addTreeSelectionListener(this);

        JScrollPane scroller = new JScrollPane(tree);

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.anchor = GridBagConstraints.FIRST_LINE_START;
        constraints.weightx = 1.0;
        constraints.weighty = 1.0;
        constraints.fill = GridBagConstraints.BOTH;
        add(scroller, constraints);

        // Right part: the buttons.
        JPanel buttonsPanel = new JPanel(new GridBagLayout());
        Dimension buttonSize = new Dimension(24, 24);

        createButton = new JButton(Project.getEditorImageIconOrEmpty("icon_add.png"));
        createButton.setMinimumSize(buttonSize);
        createButton.setMaximumSize(buttonSize);
        createButton.setPreferredSize(buttonSize);
        createButton.setToolTipText("Create animation or direction");
        createButton.addActionListener(new ActionCreate());
        createAnimationItem = new JMenuItem("Create animation");
        createAnimationItem.addActionListener(new ActionCreateAnimation());
        createDirectionItem = new JMenuItem("Create direction");
        createDirectionItem.addActionListener(new ActionCreateDirection());
        constraints.gridx = 0;
        constraints.gridy = 0;
        constraints.anchor = GridBagConstraints.FIRST_LINE_START;
        constraints.weightx = 0.0;
        constraints.weighty = 0.0;
        constraints.fill = GridBagConstraints.NONE;
        buttonsPanel.add(createButton, constraints);

        cloneButton = new JButton(Project.getEditorImageIconOrEmpty("icon_copy.png"));
        cloneButton.setMinimumSize(buttonSize);
        cloneButton.setMaximumSize(buttonSize);
        cloneButton.setPreferredSize(buttonSize);
        cloneButton.setToolTipText("Clone");
        cloneButton.addActionListener(new ActionClone());
        constraints.gridy++;
        buttonsPanel.add(cloneButton, constraints);

        renameButton = new JButton(Project.getEditorImageIconOrEmpty("icon_rename.png"));
        renameButton.setMinimumSize(buttonSize);
        renameButton.setMaximumSize(buttonSize);
        renameButton.setPreferredSize(buttonSize);
        renameButton.setToolTipText("Rename");
        renameButton.addActionListener(new ActionRenameAnimation());
        constraints.gridy++;
        buttonsPanel.add(renameButton, constraints);

        deleteButton = new JButton(Project.getEditorImageIconOrEmpty("icon_cross.png"));
        deleteButton.setMinimumSize(buttonSize);
        deleteButton.setMaximumSize(buttonSize);
        deleteButton.setPreferredSize(buttonSize);
        deleteButton.setToolTipText("Delete");
        deleteButton.addActionListener(new ActionDelete());
        constraints.gridy++;
        buttonsPanel.add(deleteButton, constraints);

        constraints.gridy++;
        constraints.weighty = 1.0;
        constraints.fill = GridBagConstraints.BOTH;
        buttonsPanel.add(new JLabel(), constraints);  // Fill the rest with empty space.

        constraints.gridx = 1;
        constraints.gridy = 0;
        constraints.anchor = GridBagConstraints.FIRST_LINE_START;
        constraints.weightx = 0.0;
        constraints.weighty = 1.0;
        constraints.fill = GridBagConstraints.VERTICAL;
        add(buttonsPanel, constraints);

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

        tree.setModel(null);
        tree.getSelectionModel().setSelectionMode(TreeSelectionModel.SINGLE_TREE_SELECTION);
        tree.setCellRenderer(new SpriteCellRenderer());

        DefaultMutableTreeNode root = createSpriteNode();

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
        tree.setModel(model);

        tree.expandRow(0);  // Expand the root node.
        tree.expandPath(selectionPath);  // Expand the selected animation and direction.
        tree.setSelectionPath(selectionPath);
        tree.scrollPathToVisible(selectionPath);
    }

    /**
     * Returns the tree model.
     */
    private DefaultTreeModel getTreeModel() {
        return (DefaultTreeModel) tree.getModel();
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
        return (DefaultMutableTreeNode) tree.getModel().getRoot();
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
                direction, getDirectionNodeText(animationName, directionIndex)
        ));
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
     * Returns the text to display for a direction node.
     * @param animationName A sprite animation.
     * @param direction A direction in this animation.
     * @return The appropriate text.
     */
    private String getDirectionNodeText(String animationName, int direction) {

        SpriteAnimation animation = sprite.getAnimation(animationName);
        return "Direction " + animation.getDirectionName(direction);
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
                // Build or rebuild everything.
                buildTree();
                updateButtons();
                return;
            }

            // Local change in the tree.
            Sprite.Change change = (Sprite.Change) info;
            switch (change.getWhatChanged()) {

            case ANIMATION_ADDED:
            {
                // Create a new node.
                String animationName = (String) change.getInfo();
                DefaultMutableTreeNode animationNode = createAnimationNode(animationName);
                DefaultMutableTreeNode root = getSpriteNode();
                getTreeModel().insertNodeInto(
                        animationNode,
                        root,
                        root.getChildCount()
                );

                // Maybe the new animation already has directions.
                SpriteAnimation animation = sprite.getAnimation(animationName);
                for (int i = 0; i < animation.getNbDirections(); i++) {
                    DefaultMutableTreeNode directionNode = createDirectionNode(
                            animationName,
                            i
                            );
                    getTreeModel().insertNodeInto(
                            directionNode,
                            animationNode,
                            animationNode.getChildCount()
                    );
                }

                updateDirectionTexts(animationNode);
                updateSelection();
            }
            break;

            case ANIMATION_REMOVED:
            {
                // Remove the node.
                String animationName = (String) change.getInfo();
                DefaultMutableTreeNode animationNode = getAnimationNode(animationName);
                getTreeModel().removeNodeFromParent(animationNode);
            }
            break;

            case ANIMATION_RENAMED:
            {
                // Update the node's text
                String newAnimationName = (String) change.getInfo(1);
                DefaultMutableTreeNode animationNode = getAnimationNode(newAnimationName);
                NodeInfo nodeInfo = (NodeInfo) animationNode.getUserObject();
                nodeInfo.setText(newAnimationName);
                getTreeModel().nodeChanged(animationNode);
            }
            break;

            case DEFAULT_ANIMATION_CHANGED:
                // Nothing to do.
                break;

            case SELECTED_ANIMATION_CHANGED:
                updateSelection();
                break;

            case DIRECTION_ADDED:
            {
                // Create a new node.
                String animationName = sprite.getSelectedAnimationName();
                int direction = (int) change.getInfo();
                DefaultMutableTreeNode directionNode = createDirectionNode(
                        animationName,
                        direction
                );
                DefaultMutableTreeNode animationNode = getAnimationNode(animationName);
                getTreeModel().insertNodeInto(
                        directionNode,
                        animationNode,
                        animationNode.getChildCount()
                );

                // The number of directions has changed: update direction texts.
                updateDirectionTexts(animationNode);

                updateSelection();
            }
            break;

            case DIRECTION_REMOVED:
            {
                // Remove the node.
                String animationName = (String) change.getInfo(0);
                int direction = (int) change.getInfo(1);
                DefaultMutableTreeNode directionNode = getDirectionNode(animationName, direction);
                getTreeModel().removeNodeFromParent(directionNode);

                // The number of directions has changed: update direction texts.
                updateDirectionTexts(getAnimationNode(animationName));
            }
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
            DefaultMutableTreeNode node = (DefaultMutableTreeNode) tree.getLastSelectedPathComponent();

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
        tree.getSelectionModel().setSelectionPath(path);
        tree.scrollPathToVisible(path);

        updateButtons();
    }

    /**
     * Enables or disables the appropriate buttons depending on what is
     * selected.
     */
    private void updateButtons() {

        if (sprite.getSelectedAnimation() == null) {
            // Nothing is selected.
            createDirectionItem.setEnabled(false);
            cloneButton.setEnabled(false);
            deleteButton.setEnabled(false);
            renameButton.setEnabled(false);
        }
        else {
            createDirectionItem.setEnabled(true);
            cloneButton.setEnabled(true);
            deleteButton.setEnabled(true);
            if (sprite.getSelectedDirection() == null) {
                // An animation is selected but no direction is selected.
                renameButton.setEnabled(true);
            }
            else {
                // An animation and a direction are selected.
                renameButton.setEnabled(false);
            }
        }
    }

    /**
     * Sets the appropriate texts in the direction nodes under the specified
     * animation node.
     * @param animationNode An animation node.
     */
    private void updateDirectionTexts(DefaultMutableTreeNode animationNode) {

        NodeInfo animationNodeInfo = (NodeInfo) animationNode.getUserObject();
        SpriteAnimation animation = (SpriteAnimation) animationNodeInfo.getData();

        for (int i = 0; i < animation.getNbDirections(); i++) {
            DefaultMutableTreeNode directionNode = (DefaultMutableTreeNode) animationNode.getChildAt(i);
            NodeInfo nodeInfo = (NodeInfo) directionNode.getUserObject();
            nodeInfo.setText(getDirectionNodeText(animation.getName(), i));
            getTreeModel().nodeChanged(directionNode);
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

        /**
         * Sets the text displayed for this node.
         * @param text The new text.
         */
        public void setText(String text) {
            this.text = text;
        }

        /**
         * Returns the text displayed for this node.
         * @return The text.
         */
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

    /**
     * Action performed when the user clicks the "Rename animation" button.
     */
    private class ActionRenameAnimation implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent event) {

            if (sprite == null) {
                return;
            }

            String oldAnimationName = sprite.getSelectedAnimationName();
            if (oldAnimationName.isEmpty()) {
                return;
            }

            String newAnimationName = (String) JOptionPane.showInputDialog(
                null,
                "New animation name:",
                "Rename animation '" + oldAnimationName + "'",
                JOptionPane.QUESTION_MESSAGE,
                null,
                null,
                oldAnimationName
            );
            if (newAnimationName != null &&
                    !newAnimationName.equals(oldAnimationName)) {
                try {
                    sprite.renameAnimation(oldAnimationName, newAnimationName);
                }
                catch (SpriteException ex) {
                    GuiTools.errorDialog("Cannot rename animation: " + ex.getMessage());
                }
            }
        }

    }

    /**
     * Action performed when the user clicks the
     * "Create animation or direction" button.
     */
    private class ActionCreate implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent event) {

            // Show a popup menu to choose between creating animation or direction.
            JPopupMenu menu = new JPopupMenu();

            menu.add(createAnimationItem);
            menu.add(createDirectionItem);
            Component source = (Component) event.getSource();
            menu.show(source, source.getWidth(), 0);
        }

    }

    /**
     * Action performed when the user clicks the "Create animation" menu item.
     */
    private class ActionCreateAnimation implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent event) {

            if (sprite == null) {
                return;
            }

            String animationName = (String) JOptionPane.showInputDialog(
                null,
                "Animation name:",
                "Create animation",
                JOptionPane.QUESTION_MESSAGE
            );
            if (animationName != null) {
                try {
                    sprite.addAnimation(animationName);
                }
                catch (SpriteException ex) {
                    GuiTools.errorDialog("Cannot rename animation: " + ex.getMessage());
                }
            }
        }

    }

    /**
     * Action performed when the user clicks the "Create direction" menu item.
     */
    private class ActionCreateDirection implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent event) {

            if (sprite == null) {
                return;
            }

            SpriteAnimation animation = sprite.getSelectedAnimation();
            if (animation == null) {
                // No animation is selected.
                return;
            }

            try {
                sprite.addDirection(new Rectangle(0, 0, 16, 16));
            }
            catch (SpriteException ex) {
                GuiTools.errorDialog("Cannot create direction: " + ex.getMessage());
            }
        }

    }

    /**
     * Action performed when the user clicks the
     * "Clone" button.
     */
    private class ActionClone implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent event) {

            if (sprite == null) {
                return;
            }

            // Clone the selected animation or direction.
            String animationName = sprite.getSelectedAnimationName();
            if (animationName.isEmpty()) {
                // No animation is selected.
                return;
            }

            int direction = sprite.getSelectedDirectionNb();
            if (direction != -1) {
                // Clone the direction.
                try {
                    sprite.cloneDirection();
                }
                catch (SpriteException ex) {
                    GuiTools.errorDialog("Cannot clone direction: " + ex.getMessage());
                }
            }
            else {
                // Clone the animation.
                String newAnimationName = (String) JOptionPane.showInputDialog(
                        null,
                        "New animation name:",
                        "Clone animation '" + animationName,
                        JOptionPane.QUESTION_MESSAGE
                );
                if (newAnimationName != null) {
                    try {
                        sprite.cloneAnimation(newAnimationName);
                    }
                    catch (SpriteException ex) {
                        GuiTools.errorDialog("Cannot clone animation '" + animationName + "': " + ex.getMessage());
                    }
                }
            }

        }

    }

    /**
     * Action performed when the user clicks the
     * "Delete" button.
     */
    private class ActionDelete implements ActionListener {

        @Override
        public void actionPerformed(ActionEvent event) {

            // After confirmation, delete the selected animation or direction.
            if (sprite == null) {
                return;
            }

            String animationName = sprite.getSelectedAnimationName();
            if (animationName.isEmpty()) {
                // No animation is selected.
                return;
            }

            String message;
            String title;
            int direction = sprite.getSelectedDirectionNb();
            if (direction == -1) {
                message = "Do you really want to delete animation '" + animationName + "' and all its directions?";
                title = "Remove animation";
            }
            else {
                message = "Do you really want to delete direction " + sprite.getSelectedDirectionName() + "?";
                title = "Remove direction";
            }
            int answer = JOptionPane.showConfirmDialog(
                    null,
                    message,
                    title,
                    JOptionPane.YES_NO_OPTION,
                    JOptionPane.WARNING_MESSAGE
            );

            if (answer != 0) {
                // Canceled.
                return;
            }

            try {
                if (direction == -1) {
                    // Remove the animation.
                    sprite.removeAnimation();
                }
                else {
                    // Remove the direction.
                    sprite.removeDirection();
                }
            }
            catch (SpriteException ex) {
                String what = (direction == -1) ? "animation" : "direction";
                GuiTools.errorDialog("Cannot delete " + what + ": " + ex.getMessage());
            }
        }

    }

}
