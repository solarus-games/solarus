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
import javax.swing.*;
import java.util.*;
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
    private final JLabel positionView;
    private final JLabel sizeView;
    private final JLabel originView;
    private final JLabel nbFramesView;
    private final JLabel nbColumnsView;

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
        add(new JLabel("Position"), constraints);

        // size
        constraints.gridy++;
        add(new JLabel("Size"), constraints);

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
        positionView = new JLabel();
        add(positionView, constraints);

        constraints.gridy++;
        sizeView = new JLabel();
        add(sizeView, constraints);

        constraints.gridy++;
        originView = new JLabel();
        add(originView, constraints);

        constraints.gridy++;
        nbFramesView = new JLabel();
        add(nbFramesView, constraints);

        constraints.gridy++;
        nbColumnsView = new JLabel();
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
                positionView.setText("");
                sizeView.setText("");
                originView.setText("");
                nbFramesView.setText("");
                nbColumnsView.setText("");
            }
            else {
                Point position = selectedDirection.getPosition();
                Dimension size = selectedDirection.getSize();
                Point origin = selectedDirection.getOrigin();

                positionView.setText(position.x + ", " + position.y);
                sizeView.setText(size.width + "x" + size.height);
                originView.setText(origin.x + ", " + origin.y);
                nbFramesView.setText("" + selectedDirection.getNbFrames());
                nbColumnsView.setText("" + selectedDirection.getNbColumns());
            }
       }
    }
}
