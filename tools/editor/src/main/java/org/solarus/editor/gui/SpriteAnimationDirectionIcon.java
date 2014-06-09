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
 * This component shows a sprite animation direction's icon.
 */
public class SpriteAnimationDirectionIcon extends JComponent implements Observer {

    private static final Color bgColorSelected = new Color(128, 128, 255);
    private static final Color bgColorUnselected = Color.WHITE;

    /**
     * The sprite animation direction
     */
    private SpriteAnimationDirection direction;

    /**
     * The sprite
     */
    private Sprite sprite;

    /**
     * Coordinates of the tile pattern as displayed by the icon.
     */
    private int dx1, dx2, dy1, dy2;

    /**
     * Coordinates of the direction first frame's icon in the animation double image.
     */
    private int sx1, sx2, sy1, sy2;

    /**
     * Constructor.
     * @param direction
     * @param sprite
     */
    public SpriteAnimationDirectionIcon(SpriteAnimationDirection direction, Sprite sprite) {
        this.direction = direction;
        this.sprite = sprite;
        direction.addObserver(this);
        sprite.addObserver(this);

        update(direction, null);
        update(sprite, null);
    }

    /**
     * Returns the preferred size.
     * @return the preffered size
     */
    @Override
    public Dimension getPreferredSize() {

        Dimension size = direction.getSize();
        return new Dimension(8 + size.width * 2, 8 + size.height * 2);
    }

    /**
     * Draws the direction first frame's image.
     * @param g graphic context
     */
    @Override
    public void paint(Graphics g) {
        Image image = sprite.getScaledImage(Zoom.BIG);

        if (image != null) {
            Color bgColor = (sprite.getSelectedDirection() == direction) ? bgColorSelected : bgColorUnselected;
            g.setColor(bgColor);
            Dimension size = getPreferredSize();
            g.fillRect(0, 0, size.width, size.height);
            g.drawImage(image, dx1, dy1, dx2, dy2, sx1, sy1, sx2, sy2, null);
        }
    }

    /**
     * This function is called when the direction is changed.
     */
    @Override
    public void update(Observable o, Object params) {

        if (o instanceof SpriteAnimationDirection) { // the direction has changed

            Point position = direction.getPosition();
            Dimension size = direction.getSize();

            // coordinates of the direction in the animation image
            sx1 = position.x * 2;
            sx2 = sx1 + (size.width * 2);
            sy1 = position.y * 2;
            sy2 = sy1 + (size.height * 2);

            // coordinates of the direction in the icon

            // check the direction dimensions
            Dimension maxSize = getPreferredSize();
            dx1 = maxSize.width / 2 - size.width;
            dx2 = maxSize.width / 2  + size.width;
            dy1 = maxSize.height / 2 - size.height;
            dy2 = maxSize.height / 2 + size.height;
        }

        repaint();
    }
}
