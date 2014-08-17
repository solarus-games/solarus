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
import java.io.File;
import java.io.IOException;
import java.util.*;

import javax.imageio.ImageIO;
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
    private final DefaultAnimationField defaultAnimationView;
    private final SrcImageField srcImageView;
    private final FrameDelayField frameDelayView;
    private final LoopField loopView;
    private final LoopOnFrameField loopOnFrameView;

    /**
     * Constructor.
     */
    public SpriteAnimationView() {

        super(new GridBagLayout());

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.insets = new Insets(5, 5, 5, 5); // margins
        constraints.anchor = GridBagConstraints.FIRST_LINE_START; // alignment of the components
        constraints.fill = GridBagConstraints.HORIZONTAL;

        constraints.weightx = 0;  // Don't give more space to this column when resizing.
        constraints.gridx = 0;
        constraints.gridy = 0;
        add(new JLabel("Default animation"), constraints);

        constraints.gridy++;
        add(new JLabel("Source image"), constraints);

        constraints.gridy++;
        add(new JLabel("Frame delay"), constraints);

        constraints.gridy++;
        loopView = new LoopField();
        add(loopView, constraints);

        constraints.weightx = 1;  // Give any available space to this column when resizing.
        constraints.gridx = 1;
        constraints.gridy = 0;
        defaultAnimationView = new DefaultAnimationField();
        add(defaultAnimationView, constraints);

        constraints.gridy++;
        srcImageView = new SrcImageField();
        add(srcImageView, constraints);

        constraints.fill = GridBagConstraints.NONE;
        constraints.gridy++;
        frameDelayView = new FrameDelayField();
        add(frameDelayView, constraints);

        constraints.gridy++;
        loopOnFrameView = new LoopOnFrameField();
        add(loopOnFrameView, constraints);
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
     * Sets the animation shown in this component.
     * @param animationName The name of animation, or an empty string to
     * show no animation in the component.
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

        setSelectedAnimation(sprite.getSelectedAnimationName());
        defaultAnimationView.update();
        srcImageView.update();
        frameDelayView.update();
        loopView.update();
        loopOnFrameView.update();
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

        private JRadioButton tilesetButton;
        private JRadioButton imageButton;

        /**
         * Constructor.
         */
        public ChooseSpriteImageDialog(String selectedImage) throws SpriteException {

            super("Change source image", false);

            srcImage = selectedImage;

            ButtonGroup buttonGroup = new ButtonGroup();
            tilesetButton = new JRadioButton("tileset");
            tilesetButton.addActionListener(new ActionListener() {

                @Override
                public void actionPerformed(ActionEvent ae) {

                    if (tilesetButton.isSelected()) {
                        srcImage = "tileset";
                    } else {
                        srcImage = imageTree.getSelectedFile();
                    }
                    update();
                }
            });
            buttonGroup.add(tilesetButton);

            imageButton = new JRadioButton("image");
            imageButton.addActionListener(new ActionListener() {

                @Override
                public void actionPerformed(ActionEvent ae) {

                    if (imageButton.isSelected()) {
                        srcImage = imageTree.getSelectedFile();
                    } else {
                        srcImage = "tileset";
                    }
                    update();
                }
            });
            buttonGroup.add(imageButton);

            JPanel buttonPanel = new JPanel();
            buttonPanel.setLayout(new BoxLayout(buttonPanel, BoxLayout.X_AXIS));
            buttonPanel.add(imageButton);
            buttonPanel.add(tilesetButton);

            imageTree = new SpriteImageTree();
            imageTree.setVisible(true);
            final JScrollPane imageTreeScroller = new JScrollPane(imageTree);

            JPanel rightPanel = new JPanel();
            rightPanel.setLayout(new BoxLayout(rightPanel, BoxLayout.Y_AXIS));

            rightPanel.add(buttonPanel);
            rightPanel.add(imageTreeScroller);

            imageView = new ImageView();
            final JScrollPane imageViewScroller = new JScrollPane(imageView);
            imageViewScroller.setAlignmentY(Component.TOP_ALIGNMENT);

            final JSplitPane mainSplitter = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT,
                    true, rightPanel, imageViewScroller);
            mainSplitter.setDividerLocation(200);
            mainSplitter.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));
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

            if (srcImage.equals("tileset")) {
                tilesetButton.setSelected(true);
            } else {
                imageButton.setSelected(true);
            }
            update();
        }

        /**
         * Called when one of a radio button has change.
         */
        private void update() {

            imageTree.setEnabled(!srcImage.equals("tileset"));
            imageView.revalidate();
            imageView.repaint();
        }

        /**
         * Returns the source image
         * @return the source image
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

                File file = null;

                if (srcImage.equals("tileset")) {
                    if (selectedAnimation != null) {
                        String tilesetId = selectedAnimation.getTilesetId();
                        file = Project.getTilesetEntitiesImageFile(tilesetId);
                    }
                } else {
                    file = new File(Project.getDataPath() + "/sprites/" + srcImage);
                }
                try {
                    return ImageIO.read(file);
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
    * Dialog shown when we want to create a new animation in this sprite
    */
   private class NewAnimationDialog extends OkCancelDialog {
           private static final long serialVersionUID = 1L;

        // Subcomponents
        private final JTextField nameField;

        /**
         * Constructor.
         */
        public NewAnimationDialog() {

            super("New animation", false);

            JPanel mainPanel = new JPanel(new GridBagLayout());
            GridBagConstraints constraints = new GridBagConstraints();
            constraints.insets = new Insets(5, 5, 5, 5); // margins
            constraints.anchor = GridBagConstraints.LINE_START;
            constraints.gridy = 0;
            constraints.gridx = 0;

            mainPanel.add(new JLabel("name:"), constraints);

            constraints.gridx++;
            nameField = new JTextField(15);
            mainPanel.add(nameField, constraints);

            setComponent(mainPanel);
        }

        /**
         * Returns the name of animation.
         * @return the name
         */
        public String getAnimationName() {
            return nameField.getText();
        }

        @Override
        protected void applyModifications() {
        }
   }

   /**
     * Component to set the selected animation as the default animation.
     */
    private class DefaultAnimationField extends JCheckBox implements ActionListener {

        /**
         * Constructor.
         */
        public DefaultAnimationField() {
            super("Set as default");
            addActionListener(this);
            update();
        }

        /**
         * This method is called when the user changes the value of this field.
         */
        @Override
        public void actionPerformed(ActionEvent ev) {

            boolean selected = isSelected();
            if (sprite != null) {
                sprite.setDefaultAnimation(selected ? sprite.getSelectedAnimationName() : "");
            }
        }

        /**
         * This function is called when the sprite is changed.
         * The component is updated.
         */
        public void update() {

            if (sprite != null && selectedAnimation != null) {
                setSelected(sprite.getDefaultAnimationName() == sprite.getSelectedAnimationName());
                setEnabled(true);
            }
            else {
                setSelected(false);
                setEnabled(false);
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
        private JButton refreshButton;
        private JButton setButton;

        /**
         * Constructor.
         */
        public SrcImageField() {
            super();
            setLayout(new GridBagLayout());

            GridBagConstraints constraints = new GridBagConstraints();
            constraints.anchor = GridBagConstraints.FIRST_LINE_START; // alignment of the components

            srcImage = new JTextField();
            srcImage.setEditable(false);
            constraints.gridx = 0;
            constraints.gridy = 0;
            constraints.weightx = 1.0;
            constraints.fill = GridBagConstraints.BOTH;
            add(srcImage, constraints);

            tilesetChooser = new ResourceChooser(ResourceType.TILESET, false);
            tilesetChooser.addActionListener(this);

            constraints.gridx = 1;
            constraints.gridy = 0;
            constraints.weightx = 1.0;
            constraints.fill = GridBagConstraints.BOTH;
            add(tilesetChooser, constraints);

            setButton = new JButton(Project.getEditorImageIconOrEmpty("icon_open.png"));
            setButton.setToolTipText("Change source image");
            setButton.setMinimumSize(new Dimension(24, 24));
            setButton.setPreferredSize(new Dimension(24, 24));
            setButton.setMaximumSize(new Dimension(24, 24));
            setButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent ev) {

                    if (selectedAnimation == null) {
                        return;
                    }

                    try {
                        ChooseSpriteImageDialog dialog =
                                new ChooseSpriteImageDialog(selectedAnimation.getSrcImageName());
                        if (dialog.display()) {
                            selectedAnimation.setSrcImageName(dialog.getSrcImage());
                            sprite.setSaved(false);
                            selectedAnimation.reloadSrcImage();
                        }
                    } catch (SpriteException ex) {
                        GuiTools.errorDialog("Cannot change the source image: " + ex.getMessage());
                    }
                }
            });

            constraints.gridx = 2;
            constraints.gridy = 0;
            constraints.weightx = 0.0;
            constraints.fill = GridBagConstraints.NONE;
            add(setButton, constraints);

            refreshButton = new JButton(Project.getEditorImageIconOrEmpty("icon_refresh.png"));
            refreshButton.setToolTipText("Refresh source image");
            refreshButton.setMinimumSize(new Dimension(24, 24));
            refreshButton.setPreferredSize(new Dimension(24, 24));
            refreshButton.setMaximumSize(new Dimension(24, 24));
            refreshButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent ev) {

                    if (selectedAnimation != null) {
                        selectedAnimation.reloadSrcImage();
                    }
                }
            });

            constraints.gridx = 3;
            constraints.gridy = 0;
            constraints.weightx = 0.0;
            constraints.fill = GridBagConstraints.NONE;
            add(refreshButton, constraints);

            update();
        }

        /**
         * This function is called when the animation is changed.
         * The component is updated.
         */
        public void update() {

            if (selectedAnimation != null) {
                refreshButton.setEnabled(true);
                setButton.setEnabled(true);
                String image = selectedAnimation.getSrcImageName();
                image = image.isEmpty() ? "<none>" : image;
                if (!image.equals(srcImage.getText())) {
                    srcImage.setText(image.isEmpty() ? "<none>" : image);
                }
                srcImage.setVisible(!image.equals("tileset"));
                tilesetChooser.setVisible(image.equals("tileset"));
                try {
                    tilesetChooser.setSelectedId(selectedAnimation.getTilesetId());
                } catch (NoSuchElementException ex) {
                    // tileset not found in the list
                }
            }
            else {
                refreshButton.setEnabled(false);
                setButton.setEnabled(false);
                tilesetChooser.setVisible(false);
                srcImage.setText("<none>");
                srcImage.setVisible(true);
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
            update();
        }

        /**
         * This function is called when the animation is changed.
         * The component is updated.
         */
        public void update() {

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
     * Component to choose whether the animation loops.
     */
    private class LoopField extends JPanel implements ActionListener {

        private final JLabel loopLabel;
        private final JCheckBox loopCheckBox;

        public LoopField() {

            super(new BorderLayout());
            loopLabel = new JLabel("Loop ");
            add(loopLabel, BorderLayout.LINE_START);
            loopCheckBox = new JCheckBox();
            add(loopCheckBox, BorderLayout.CENTER);
            loopCheckBox.addActionListener(this);
            update();
        }

        /**
         * This function is called when the animation is changed.
         * The component is updated.
         */
        public void update() {

            if (selectedAnimation == null) {
                loopLabel.setEnabled(false);
                loopCheckBox.setEnabled(false);
                return;
            }

            if (selectedAnimation.getFrameDelay() > 0) {
                loopLabel.setEnabled(true);
                loopCheckBox.setEnabled(true);
                loopCheckBox.setSelected(selectedAnimation.getLoopOnFrame() != -1);
            }
            else {
                loopLabel.setEnabled(false);
                loopCheckBox.setEnabled(false);
            }
        }

        @Override
        public void actionPerformed(ActionEvent event) {
            if (selectedAnimation == null) {
                return;
            }

            boolean currentLoop = selectedAnimation.getLoopOnFrame() != -1;
            boolean newLoop = loopCheckBox.isSelected();
            
            if (newLoop == currentLoop) {
                return;
            }

            if (newLoop) { 
                selectedAnimation.setLoopOnFrame(loopOnFrameView.getLoopOnFrame());
            }
            else {
                selectedAnimation.setLoopOnFrame(-1);
            }
            sprite.setSaved(false);
        }
    }

    /**
     * Component to choose the frame to loop on in this animation.
     */
    private class LoopOnFrameField extends JPanel implements ChangeListener {

        private final JLabel frameLabel;
        private final NumberChooser frameField;

        /**
         * Constructor.
         */
        public LoopOnFrameField() {
            super(new BorderLayout());

            frameField = new NumberChooser(0, 0, Integer.MAX_VALUE);
            frameLabel = new JLabel("on frame ");
            add(frameLabel, BorderLayout.LINE_START);
            add(frameField, BorderLayout.CENTER);
            frameField.addChangeListener(this);
            update();
        }
        
        /**
         * Returns the number shown in the field.
         * @return The frame number, even if the field is disabled.
         */
        public int getLoopOnFrame() {
            return frameField.getNumber();
        }

        /**
         * This function is called when the animation is changed.
         * The component is updated.
         */
        public void update() {

            if (selectedAnimation != null &&
                    selectedAnimation.getFrameDelay() > 0 &&
                    selectedAnimation.getLoopOnFrame() != -1) {
                frameLabel.setEnabled(true);
                frameField.setEnabled(true);
                frameField.setNumber(selectedAnimation.getLoopOnFrame());
            }
            else {
                frameLabel.setEnabled(false);
                frameField.setEnabled(false);
            }
        }

        /**
         * This method is called when the user changes the value of this field.
         */
        @Override
        public void stateChanged(ChangeEvent ev) {

            if (selectedAnimation == null) {
                return;
            }

            int selectedLoopOnFrame = frameField.getNumber();
            int currentloopOnFrame = selectedAnimation.getLoopOnFrame();
            if (currentloopOnFrame != selectedLoopOnFrame) {
                selectedAnimation.setLoopOnFrame(selectedLoopOnFrame);
                sprite.setSaved(false);
            }
        }
    }
}
