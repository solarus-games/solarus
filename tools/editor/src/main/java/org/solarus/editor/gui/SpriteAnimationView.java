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
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.util.*;
import javax.swing.*;
import org.solarus.editor.*;

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
    private final JLabel srcImageView;
    private final JLabel frameDelayView;
    private final JLabel loopOnFrameView;

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
        srcImageView = new JLabel();
        add(srcImageView, constraints);

        constraints.gridy++;
        frameDelayView = new JLabel();
        add(frameDelayView, constraints);

        constraints.gridy++;
        loopOnFrameView = new JLabel();
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
            if (animationName != null && animationName.length() > 0) {
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

        if (o instanceof Sprite) {
            setSelectedAnimation(sprite.getSelectedAnimationName());
        } else {
            // update the elementary components here
            if (selectedAnimation == null) {
                srcImageView.setText("");
                frameDelayView.setText("");
                loopOnFrameView.setText("");
            }
            else {
                srcImageView.setText(selectedAnimation.getSrcImage());
                frameDelayView.setText("" + selectedAnimation.getFrameDelay());
                loopOnFrameView.setText("" + selectedAnimation.getLoopOnFrame());
            }
       }

        animationField.update(o);
    }

    /**
     * Component to change the selected animation of the sprite.
     */
    private class AnimationField extends JPanel implements ActionListener {

        /**
         * The animation selector.
         */
        private SpriteAnimationChooser animationChooser;

        /**
         * Constructor.
         */
        public AnimationField() {
            super();
            setLayout(new BoxLayout(this, BoxLayout.LINE_AXIS));

            animationChooser = new SpriteAnimationChooser(sprite);
            animationChooser.setPreferredSize(new Dimension(120, 24));
            add(animationChooser);
            animationChooser.addActionListener(this);

            JButton addButton = new JButton(Project.getEditorImageIconOrEmpty("icon_add.png"));
            addButton.setPreferredSize(new Dimension(24, 24));
            addButton.setToolTipText("Add animation");
            addButton.addActionListener(new ActionListener() {
                @Override
                public void actionPerformed(ActionEvent ev) {

                    if (sprite == null) {
                        return;
                    }

                    // Add an animation.
                    try {
                        throw new SpriteException("Not implemented yet !");
                    }
                    catch (SpriteException ex) {
                        GuiTools.errorDialog("Cannot add animation: " + ex.getMessage());
                    }
                }
            });
            add(addButton);

            JButton removeButton = new JButton(Project.getEditorImageIconOrEmpty("icon_remove.png"));
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

            if (sprite != null) {

                String currentAnimationName = sprite.getSelectedAnimationName();
                String selectedAnimationName = animationChooser.getSelected();

                if (!selectedAnimationName.equals(currentAnimationName)) {
                    animationChooser.setSelected(currentAnimationName);
                }
                animationChooser.setEnabled(true);
            }
            else {
                animationChooser.setEnabled(false);
            }
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

            String selectedAnimationName = animationChooser.getSelected();
            String currentAnimationName = sprite.getSelectedAnimationName();

            if (!currentAnimationName.equals(selectedAnimationName)) {

                try {
                    sprite.setSelectedAnimation(selectedAnimationName);
                }
                catch (SpriteException ex) {
                    GuiTools.errorDialog("Cannot select the animation '" +
                            selectedAnimationName + "': " + ex.getMessage());
                }
            }
        }
    }
}
