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
import java.awt.image.BufferedImage;
import java.util.*;
import javax.swing.*;
import javax.swing.event.*;
import org.solarus.editor.*;

/**
 * This components shows information about animation directions of a sprite.
 */
public class SpriteAnimationDirectionView extends JPanel implements Observer {

    /**
     * The sprite observed.
     */
    private Sprite sprite;

    /**
     * The selected direction.
     */
    private SpriteAnimationDirection selectedDirection;

    // components
    private final JLabel sizeView;
    private final PositionField positionView;
    private final OriginField originView;
    private final NbFramesField nbFramesView;
    private final NbColumnsField nbColumnsView;

    /**
     * Constructor.
     */
    public SpriteAnimationDirectionView() {

        super(new GridBagLayout());

        GridBagConstraints constraints = new GridBagConstraints();
        constraints.insets = new Insets(5, 5, 5, 5); // margins
        constraints.anchor = GridBagConstraints.LINE_START; // alignment of the components

        // position
        constraints.gridx = 0;
        constraints.gridy = 0;
        add(new JLabel("Size"), constraints);

        // size
        constraints.gridy++;
        add(new JLabel("Position"), constraints);

        // origin
        constraints.gridy++;
        add(new JLabel("Origin"), constraints);

        // number of frames
        constraints.gridy++;
        add(new JLabel("Number of frames"), constraints);

        // number of columns
        constraints.gridy++;
        add(new JLabel("Numbre of columns"), constraints);

        constraints.weightx = 1;
        constraints.gridx = 1;
        constraints.gridy = 0;
        sizeView = new JLabel();
        add(sizeView, constraints);

        constraints.gridy++;
        positionView = new PositionField();
        add(positionView, constraints);

        constraints.gridy++;
        originView = new OriginField();
        add(originView, constraints);

        constraints.gridy++;
        nbFramesView = new NbFramesField();
        add(nbFramesView, constraints);

        constraints.gridy++;
        nbColumnsView = new NbColumnsField();
        add(nbColumnsView, constraints);
    }

    /**
     * Sets the sprite observed.
     * @param sprite the sprite
     */
    public void setSprite(Sprite sprite) {

        if (this.sprite != null) {
            this.sprite.deleteObserver(this);
        }

        this.sprite = sprite;

        if (sprite != null) {
            sprite.addObserver(this);
        }
        update(sprite, null);
    }

    /**
     * Sets the selected direction.
     * @param directionNb the number of the direction
     */
    public void setSelectedDirection(int directionNb) {

        SpriteAnimationDirection direction = null;
        if (sprite != null) {
            SpriteAnimation animation = sprite.getSelectedAnimation();
            if (animation != null) {
                direction = animation.getDirection(directionNb);
            }
        }

        if (selectedDirection != null) {
            selectedDirection.deleteObserver(this);
        }

        selectedDirection = direction;

        if (selectedDirection != null) {
            selectedDirection.addObserver(this);
        }
        update(direction, null);
    }

    /**
     * This function is called when the sprite is changed.
     * @param o the sprite, or null if no sprite is set
     * @param obj not used
     */
    @Override
    public void update(Observable o, Object obj) {

        if (o instanceof Sprite || o instanceof SpriteAnimation) {
            setSelectedDirection(sprite.getSelectedDirectionNb());
        } else {
            // update the elementary components here
            if (selectedDirection == null) {
                sizeView.setText("");
            }
            else {
                Point position = selectedDirection.getPosition();
                Dimension size = selectedDirection.getSize();
                Point origin = selectedDirection.getOrigin();

                sizeView.setText(size.width + "x" + size.height);
            }

            positionView.update(selectedDirection);
            originView.update(selectedDirection);
            nbFramesView.update(selectedDirection);
            nbColumnsView.update(selectedDirection);
       }
    }

    /**
     * Component to change the direction position.
     */
    private class PositionField extends CoordinatesField {

        private boolean updating = false;

        /**
         * Constructor.
         */
        public PositionField() {
            super();
            setMinimum(0, 0);
            setStepSize(8, 8);
            setSeparator(",");

            addChangeListener(new ChangeListener() {
                @Override
                public void stateChanged(ChangeEvent ev) {

                    if (selectedDirection != null && !updating) {
                        Point position = getCoordinates();
                        if (position != selectedDirection.getPosition()) {
                            try {
                                selectedDirection.setPosition(position);
                                sprite.setSaved(false);
                            } catch (SpriteException ex) {
                                 GuiTools.errorDialog("Cannot move the direction: " +
                                         ex.getMessage());
                            }
                        }
                    }
                    update(selectedDirection);
                }
            });
        }

        /**
         * This function is called when the direction is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            updating = true;
            if (sprite != null && selectedDirection != null) {
                setEnabled(true);
                Point position = selectedDirection.getPosition();
                setCoordinates(position.x, position.y);

                try {
                    BufferedImage image = sprite.getSelectedAnimation().getImage();
                    setMaximum(image.getWidth(), image.getHeight());
                } catch (SpriteException ex) {
                    // image cannot be loaded
                }
            }
            else {
                setEnabled(false);
            }
            updating = false;
        }
    }

    /**
     * Component to change the direction origin.
     */
    private class OriginField extends CoordinatesField {

        private boolean updating = false;

        /**
         * Constructor.
         */
        public OriginField() {
            super();
            setStepSize(4, 4);
            setSeparator(",");

            addChangeListener(new ChangeListener() {
                @Override
                public void stateChanged(ChangeEvent ev) {

                    if (selectedDirection != null && !updating) {
                        Point origin = getCoordinates();
                        if (origin != selectedDirection.getOrigin()) {
                            selectedDirection.setOrigin(origin);
                            sprite.setSaved(false);
                        }
                    }
                    update(selectedDirection);
                }
            });
        }

        /**
         * This function is called when the direction is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            updating = true;
            if (selectedDirection != null) {
                setEnabled(true);
                Point origin = selectedDirection.getOrigin();
                setCoordinates(origin.x, origin.y);
            }
            else {
                setEnabled(false);
            }
            updating = false;
        }
    }

    /**
     * Component to change the number of frames of the direction.
     */
    private class NbFramesField extends NumberChooser {

        /**
         * Constructor.
         */
        public NbFramesField() {
            super(0, 1, Integer.MAX_VALUE);

            addChangeListener(new ChangeListener() {
                @Override
                public void stateChanged(ChangeEvent ev) {

                    if (selectedDirection != null) {
                        int nbFrames = getNumber();
                        if (nbFrames != selectedDirection.getNbFrames()) {
                            try {
                                selectedDirection.setNbFrames(nbFrames);
                                sprite.setSaved(false);
                            } catch (SpriteException ex) {
                                GuiTools.errorDialog(
                                        "Cannot changes the number of frames of the direction: " +
                                                ex.getMessage());
                            }
                        }
                    }
                    update(selectedDirection);
                }
            });
        }

        /**
         * This function is called when the direction is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            if (selectedDirection != null) {
                setEnabled(true);
                setNumber(selectedDirection.getNbFrames());
            }
            else {
                setEnabled(false);
            }
        }
    }

    /**
     * Component to change the number of columns of the direction.
     */
    private class NbColumnsField extends NumberChooser {

        /**
         * Constructor.
         */
        public NbColumnsField() {
            super(0, 1, Integer.MAX_VALUE);

            addChangeListener(new ChangeListener() {
                @Override
                public void stateChanged(ChangeEvent ev) {

                    if (selectedDirection != null) {
                        int nbColumns = getNumber();
                        if (nbColumns != selectedDirection.getNbColumns()) {
                            try {
                                selectedDirection.setNbColumns(nbColumns);
                                sprite.setSaved(false);
                            } catch (SpriteException ex) {
                                GuiTools.errorDialog(
                                        "Cannot changes the number of columns of the direction: " +
                                                ex.getMessage());
                            }
                        }
                    }
                    update(selectedDirection);
                }
            });
        }

        /**
         * This function is called when the direction is changed.
         * The component is updated.
         */
        public void update(Observable o) {

            if (selectedDirection != null) {
                setEnabled(true);
                setNumber(selectedDirection.getNbColumns());
            }
            else {
                setEnabled(false);
            }
        }
    }
}
