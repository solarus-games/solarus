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
package org.solarus.editor.gui;

import java.awt.*;
import java.awt.event.*;
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import org.solarus.editor.*;
import org.solarus.editor.gui.tree.SpriteImageTree;

/**
 * This components shows information about animation of a sprite.
 */
class SpriteAnimationView extends JPanel implements Observer {

    /**
     * The sprite observed.
     */
    private Sprite sprite;

    /**
     * The selected animation.
     */
    private SpriteAnimation selectedAnimation;

    // components
    private final AnimationField animationField;
    private final SrcImageField srcImageView;
    private final FrameDelayField frameDelayView;
    private final LoopOnFrameField loopOnFrameView;

    /**
     * Constructor.
     */
    public SpriteAnimationView() {

        super(new GridBagLayout());

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.insets = new Insets(5, 5, 5, 5); // margins
        constraints.anchor = GridBagConstraints.LINE_START; // alignment of the components

        constraints.gridx = 0;
        constraints.gridy = 0;
        add(new JLabel("Animation"), constraints);

        constraints.gridy++;
        add(new JLabel("Source image"), constraints);

        constraints.gridy++;
        add(new JLabel("Frame delay"), constraints);

        constraints.gridy++;
        add(new JLabel("Loop on frame"), constraints);

        constraints.weightx = 1;
        constraints.gridx = 1;
        constraints.gridy = 0;
        animationField = new AnimationField();
        add(animationField, constraints);

        constraints.gridy++;
        srcImageView = new SrcImageField();
        add(srcImageView, constraints);

        constraints.gridy++;
        frameDelayView = new FrameDelayField();
        add(frameDelayView, constraints);

        constraints.gridy++;
        loopOnFrameView = new LoopOnFrameField();
        add(loopOnFrameView, constraints);

        update(sprite, null);
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
            animationField.setSprite();

            if (sprite != null) {
                sprite.addObserver(this);
            }
            update(sprite, null);
        }
    }

    /**
     * Sets the selected animation.
     * @param animationName the name of animation
     */
    public void setSelectedAnimation(String animationName) {

        SpriteAnimation animation = null;
        if (sprite != null) {
            if (!animationName.isEmpty()) {
                animation = sprite.getAnimation(animationName);
            } else {
                animation = null;
            }
        }

        if (selectedAnimation != null) {
            selectedAnimation.deleteObserver(this);
        }

        selectedAnimation = animation;

        if (selectedAnimation != null) {
            selectedAnimation.addObserver(this);
        }
        update(animation, null);
    }

    /**
     * This function is called when the sprite is changed.
     * @param o the sprite, or null if no sprite is set
     * @param obj not used
     */
    @Override
    public void update(Observable o, Object obj) {

        if (o instanceof Sprite && (obj == null || obj instanceof String)) {
            // the sprite has changed
            setSelectedAnimation(sprite.getSelectedAnimationName());
        }
        else if (obj instanceof SpriteAnimation || o instanceof SpriteAnimation || o == null) {
            // the animation has changed
            animationField.update(o);
            srcImageView.update(o);
            frameDelayView.update(o);
            loopOnFrameView.update(o);
        }
    }

    /**
    * Dialog shown when we want to changes a sprite image
    */
    private class ChooseSpriteImageDialog extends OkCancelDialog {
        private static final long serialVersionUID = 1L;

        /**
         * The source image
         */
        private String srcImage;

        // Subcomponents
        private SpriteImageTree imageTree;
        private ImageView imageView;

        /**
         * Constructor.
         */
        public ChooseSpriteImageDialog(String selectedImage) throws SpriteException {

            super("Change source image", false);

            srcImage = selectedImage;

            imageTree = new SpriteImageTree();
            imageTree.setVisible(true);
            final JScrollPane imageTreeScroller = new JScrollPane(imageTree);

            imageView = new ImageView();
            final JScrollPane imageViewScroller = new JScrollPane(imageView);
            imageViewScroller.setAlignmentY(Component.TOP_ALIGNMENT);

            final JSplitPane mainSplitter = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
                    true, imageTreeScroller, imageViewScroller);
            mainSplitter.setDividerLocation(200);
            setComponent(mainSplitter);

            imageTree.setSelectedFile(srcImage);
            imageTree.addMouseListener(new MouseListener() {

                @Override
                public void mouseClicked(MouseEvent me) {

                    String image = imageTree.getSelectedFile();

                    if (image != null) {
                        srcImage = image;
                        imageView.revalidate();
                        imageView.repaint();
                    }
                }

                @Override
                public void mousePressed(MouseEvent me) {
                }

                @Override
                public void mouseReleased(MouseEvent me) {
                }

                @Override
                public void mouseEntered(MouseEvent me) {
                }

                @Override
                public void mouseExited(MouseEvent me) {
                }
            });

            imageView.revalidate();
        }

        /**
         * Returns the source image
         * @return the sourc image
         */
        public String getSrcImage() {
            return srcImage;
        }

        @Override
        protected void applyModifications() {
        }

        /**
         * Component to show the current selected source image.
         */
        private class ImageView extends JComponent implements Scrollable {

            /**
             * Returns the current image.
             * @return the current image
             */
            public BufferedImage getImage () {

                String path = "";

                if (srcImage.equals("tileset")) {
                    if (selectedAnimation != null) {
                        String tilesetId = selectedAnimation.getTilesetId();
                        path = "tilesets/" + Project.getTilesetEntitiesImageFile(tilesetId).getName();
                    }
                } else {
                    path = "sprites/" + srcImage;
                }
                try {
                    return Project.getProjectImage(path);
                } catch (IOException ex) {
                    return null;
                }
            }

            @Override
            public void paint(Graphics g) {
                BufferedImage image = getImage();
                if (image != null) {
                    g.drawImage(image, 0, 0, null);
                }
            }

            @Override
            public Dimension getPreferredSize() {

                BufferedImage image = getImage();
                if (image != null) {
                    return new Dimension(image.getWidth(), image.getHeight());
                }
                return new Dimension(0, 0);
            }

            @Override
            public Dimension getPreferredScrollableViewportSize() {

                return new Dimension(480, 320);
            }

            @Override
            public int getScrollableUnitIncrement(Rectangle rctngl, int i, int i1) {
                return 16;
            }

            @Override
            public int getScrollableBlockIncrement(Rectangle rctngl, int i, int i1) {
                return 160;
            }

            @Override
            public boolean getScrollableTracksViewportWidth() {
                return false;
            }

            @Override
            public boolean getScrollableTracksViewportHeight() {
                return false;
            }

        }
    }

    /**
     * Component to change the selected animation of the sprite.
     */
    private class AnimationField extends JPanel implements ActionListener {

        /**
         * The animation selector.
         */
        private SpriteAnimationChooser animationChooser;

        private JButton removeButton;

        /**
         * Constructor.
         */
        public AnimationField() {
            super();
            setLayout(new BoxLayout(this, BoxLayout.LINE_AXIS));

            animationChooser = new SpriteAnimationChooser(sprite);
            animationChooser.setPreferredSize(new Dimension(140, 24));
            animationChooser.addActionListener(this);
            animationChooser.setEditable(true);
            add(animationChooser);

            removeButton = new JButton(Project.getEditorImageIconOrEmpty("icon_remove.png"));
            removeButton.setPreferredSize(new Dimension(24, 24));
            removeButton.setToolTipText("Remove animation");
            removeButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent ev) {

                    if (sprite == null) {
                        return;
                    }

                    String selectedAnimationName = animationChooser.getSelected();

                    // Remove the animation
                    try {
                        sprite.removeAnimation();
                    } catch (SpriteException ex) {
                        GuiTools.errorDialog("Cannot remove animation '" +
                                selectedAnimationName + "': " + ex.getMessage());
                    }
                }
            });
            add(removeButton);

            update(sprite);
        }

        /**
        * Changes the sprite observed for the chooser component.
        */
       public void setSprite() {

           animationChooser.setSprite(sprite);
       }

        /**
         * This function is called when the sprite is changed.
         * The selection is updated.
         */
        public void update(Observable o) {

            animationChooser.setEnabled(sprite != null && sprite.getAnimations().size() > 0);
            removeButton.setEnabled(sprite != null && sprite.getSelectedAnimation() != null);
        }

        /**
         * This method is called when the user changes the selected item.
         * The selected animation of the sprite is changed.
         */
        @Override
        public void actionPerformed(ActionEvent ev) {

            if (sprite == null) {
                return;
            }

            if (ev.getActionCommand().equals("comboBoxEdited")) {


                String name = sprite.getSelectedAnimationName();
                String newName = animationChooser.getSelected();

                if (!newName.isEmpty()) {
                    if (name.isEmpty()){
                        // Add a new animation
                        try {
                            sprite.addAnimation(newName);
                        }
                        catch (SpriteException ex) {
                            GuiTools.errorDialog("Cannot add animation: " + ex.getMessage());
                        }
                    } else {
                        try {
                            sprite.renameAnimation(name, newName);
                        } catch (SpriteException ex) {
                            GuiTools.errorDialog("Cannot rename the animation '" +
                                        name + "': " + ex.getMessage());
                        }
                    }
                }
            }
            else {
                String selectedAnimationName = animationChooser.getSelected();
                String currentAnimationName = sprite.getSelectedAnimationName();

                if (!currentAnimationName.equals(selectedAnimationName)) {

                    try {
                        sprite.setSelectedAnimation(selectedAnimationName);
                    }
                    catch (SpriteException ex) {
                        // animation doesn't exists
                    }
                }
            }
        }
    }

   /**
     * Component to choose the source image of this animation.
     */
    private class SrcImageField extends JPanel implements ActionListener {

        // components
        private JTextField srcImage;
        private ResourceChooser tilesetChooser;

        /**
         * Constructor.
         */
        public SrcImageField() {
            super();
            setLayout(new BoxLayout(this, BoxLayout.LINE_AXIS));

            srcImage = new JTextField();
            srcImage.setEditable(false);
            srcImage.setPreferredSize(new Dimension(140, 24));

            tilesetChooser = new ResourceChooser(ResourceType.TILESET, false);
            tilesetChooser.setPreferredSize(new Dimension(140, 24));
            tilesetChooser.addActionListener(this);

            add(srcImage);
            add(tilesetChooser);

            JButton setButton = new JButton(Project.getEditorImageIconOrEmpty("icon_edit.png"));
            setButton.setPreferredSize(new Dimension(24, 24));
            setButton.setToolTipText("Changes source image");
            setButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent ev) {

                    if (selectedAnimation == null) {
                        return;
                    }

                    try {
                        ChooseSpriteImageDialog dialog =
                                new ChooseSpriteImageDialog(selectedAnimation.getSrcImage());
                        if (dialog.display()) {
                            selectedAnimation.setSrcImage(dialog.getSrcImage());
                            sprite.setSaved(false);
                            sprite.reloadImage();
                        }
                    } catch (SpriteException ex) {
                        GuiTools.errorDialog("Cannot changes the source image: " + ex.getMessage());
                    }
                }
            });
            add(setButton);

            update((SpriteAnimation) null);
        }

        /**
         * This function is called when the animation is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            if (selectedAnimation != null) {
                setEnabled(true);
                String image = selectedAnimation.getSrcImage();
                image = image.isEmpty() ? "<none>" : image;
                if (!image.equals(srcImage.getText())) {
                    srcImage.setText(image.isEmpty() ? "<none>" : image);
                }
                srcImage.setVisible(!image.equals("tileset"));
                tilesetChooser.setVisible(image.equals("tileset"));
                tilesetChooser.setSelectedId(selectedAnimation.getTilesetId());
            }
            else {
                tilesetChooser.setVisible(false);
                srcImage.setText("<none>");
                srcImage.setVisible(true);
                setEnabled(false);
            }
        }

        /**
         * This method is called when the user changes the selected item.
         * The tileset of the sprite is changed.
         */
        @Override
        public void actionPerformed(ActionEvent ev) {

            if (sprite != null && selectedAnimation!= null) {
                String selectedTilesetId = tilesetChooser.getSelectedId();
                try {
                    sprite.notifyTilesetChanged(selectedTilesetId);
                } catch (SpriteException ex) {
                    GuiTools.errorDialog("Cannot load the tileset '" + selectedTilesetId + "': " + ex.getMessage());
                }
            }
        }
    }

   /**
     * Component to choose the frame delay of this animation.
     */
    private class FrameDelayField extends NumberChooser implements ChangeListener {

        /**
         * Constructor.
         */
        public FrameDelayField() {
            super(0, 0, Integer.MAX_VALUE);
            setStepSize(10);

            addChangeListener(this);
            update((SpriteAnimation) null);
        }

        /**
         * This function is called when the animation is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            if (selectedAnimation != null) {
                setEnabled(true);
                setValue(selectedAnimation.getFrameDelay());
            }
            else {
                setEnabled(false);
            }
        }

        /**
         * This method is called when the user changes the value of this field.
         */
        @Override
        public void stateChanged(ChangeEvent ev) {

            if (selectedAnimation != null) {
                int selectedFrameDelay = getNumber();
                int currentframeDelay = selectedAnimation.getFrameDelay();

                if (currentframeDelay != selectedFrameDelay) {
                    selectedAnimation.setFrameDelay(selectedFrameDelay);
                    sprite.setSaved(false);
                }
            }
        }
    }

   /**
     * Component to choose the frame delay of this animation.
     */
    private class LoopOnFrameField extends NumberChooser implements ChangeListener {

        /**
         * Constructor.
         */
        public LoopOnFrameField() {
            super(0, -1, Integer.MAX_VALUE);

            addChangeListener(this);
            update((SpriteAnimation) null);
        }

        /**
         * This function is called when the animation is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            if (selectedAnimation != null && selectedAnimation.getFrameDelay() > 0) {
                setEnabled(true);
                setValue(selectedAnimation.getLoopOnFrame());
            }
            else {
                setEnabled(false);
            }
        }

        /**
         * This method is called when the user changes the value of this field.
         */
        @Override
        public void stateChanged(ChangeEvent ev) {

            if (selectedAnimation != null) {
                int selectedLoopOnFrame = getNumber();
                int currentloopOnFrame = selectedAnimation.getLoopOnFrame();
                if (currentloopOnFrame != selectedLoopOnFrame) {
                    selectedAnimation.setLoopOnFrame(selectedLoopOnFrame);
                    sprite.setSaved(false);
                }
            }
        }
    }
}
