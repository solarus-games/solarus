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
 * It can be used as a JComponent or as an Icon.
 */
public class SpriteAnimationDirectionIcon extends JComponent implements Icon, Observer {

    private static final Color bgColorSelected = new Color(128, 128, 255);
    private static final Color bgColorUnselected = Color.WHITE;

    /**
     * The sprite.
     */
    private Sprite sprite;

    /**
     * The sprite animation this direction belongs to.
     */
    private SpriteAnimation animation;

    /**
     * The sprite animation direction represented.
     */
    private SpriteAnimationDirection direction;

    /**
     * Coordinates of the tile pattern as displayed by the icon.
     */
    private int dx1, dx2, dy1, dy2;

    /**
     * Coordinates of the direction first frame's icon in the animation double image.
     */
    private int sx1, sx2, sy1, sy2;

    /**
     * Maximum width and height of an icon.
     */
    public static final int maxLength = 64;

    /**
     * Constructor.
     * @param sprite The sprite.
     * @param animation Animation the icon will belong to.
     * @param direction Direction in this animation.
     */
    public SpriteAnimationDirectionIcon(
            Sprite sprite,
            SpriteAnimation animation,
            SpriteAnimationDirection direction
    ) {
        this.sprite = sprite;
        this.animation = animation;
        this.direction = direction;
        direction.addObserver(this);
        sprite.addObserver(this);

        update(direction, null);
    }

    /**
     * Returns the preferred size.
     * @return the preferred size
     */
    @Override
    public Dimension getPreferredSize() {
        return new Dimension(getIconWidth(), getIconHeight());
    }

    /**
     * Draws the direction first frame's image.
     * @param g Graphic context.
     */
    @Override
    public void paint(Graphics g) {
        paintIcon(this, g, 0, 0);
    }

    /**
     * Draws the direction first frame's image.
     * @param g Graphic context.
     */
    @Override
    public void paintIcon(Component component, Graphics g, int x, int y) {

        Image image = animation.getSrcImage();

        if (image != null) {
            Color bgColor = (sprite.getSelectedDirection() == direction) ? bgColorSelected : bgColorUnselected;
            g.setColor(bgColor);
            g.fillRect(0, 0, getIconWidth(), getIconHeight());
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
            Dimension srcSize = direction.getSize();

            // coordinates of the direction in the animation image
            sx1 = position.x;
            sx2 = sx1 + srcSize.width;
            sy1 = position.y;
            sy2 = sy1 + srcSize.height;

            // coordinates of the direction in the icon

            // check the direction dimensions
            Dimension dstSize = getPreferredSize();
            dx1 = 0;
            dx2 = dstSize.width;
            dy1 = 0;
            dy2 = dstSize.height;
        }

        repaint();
    }

    @Override
    public int getIconWidth() {
        Dimension size = direction.getSize();
        if (size.width > size.height) {
            return Math.min(maxLength, size.width);
        }

        if (size.height > maxLength) {
            return size.width * maxLength / size.height;
        }
        return size.width;
    }

    @Override
    public int getIconHeight() {

        Dimension size = direction.getSize();
        if (size.height > size.width) {
            return Math.min(maxLength, size.height);
        }

        if (size.width > maxLength) {
            return size.height * maxLength / size.width;
        }
        return size.height;
    }
}
