/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
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
package org.solarus.editor;

import java.awt.*;
import java.awt.image.*;

import org.solarus.editor.entities.MapEntity;

/**
 * Represents a direction of animation of a sprite.
 */
public class SpriteAnimationDirection {

    /**
     * @brief The frames of this animation direction.
     */
    private BufferedImage[] frames;

    /**
     * @brief Origin point of the sprite (the same for all frames).
     */
    private Point origin;

    /**
     * Creates a direction of animation.
     * @param srcImage the image to use
     * @param firstFrameRectangle rectangle representing the first frame
     * (the other ones will have the same size)
     * @param originX x coordinate of the sprite's origin in each rectangle
     * @param originY y coordinate of the sprite's origin in each rectangle
     * @param nbFrames number of frames to create
     * @param nbColumns number of columns (because the rectangles may be organized in several rows)
     * @param frames the list of rectangles representing the frames in the image
     * @throws MapException If some rectangles are outside the image.
     */
    public SpriteAnimationDirection(BufferedImage srcImage,
            Rectangle firstFrameRectangle,
            int nbFrames, int nbColumns, int originX, int originY)
            throws MapException {

        int i = 0;
        int j = 0;
        int x = 0;
        int y = 0;
        int width = firstFrameRectangle.width;
        int height = firstFrameRectangle.height;
        int frame = 0;

        try {
            origin = new Point(originX, originY);
            frames = new BufferedImage[nbFrames];

            int nbRows = nbFrames / nbColumns;
            if (nbFrames % nbColumns != 0) {
                nbRows++;
            }

            for (i = 0; i < nbRows && frame < nbFrames; i++) {
                for (j = 0; j < nbColumns && frame < nbFrames; j++) {

                    x = firstFrameRectangle.x + j * width;
                    y = firstFrameRectangle.y + i * height;
                    frames[frame] = srcImage.getSubimage(x, y, width, height);
                    frame++;
                }
            }
        }
        catch (RasterFormatException ex) {
            throw new MapException("Invalid frame " + frame +
                    ": (" + x + "," + y + "),(" + (x + width) + "," + (y + height) +
                    "): size of source image is only " + srcImage.getWidth() + "x" + srcImage.getHeight() +
                    "\nPlease fix your sprite file.");
        }
    }

    /**
     * Returns the origin point of this direction.
     * @return the origin point
     */
    public Point getOrigin() {

        return origin;
    }

    /**
     * Returns the size of frames in this direction.
     * @return the size
     */
    public Dimension getSize() {

        return new Dimension(frames[0].getWidth(), frames[0].getHeight());
    }

    /**
     * Returns a frame of this direction.
     * @param frame index of the frame to show
     */
    public Image getFrame(int frame) {

        return frames[frame];
    }

    /**
     * Displays a frame of this direction.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     * @param frame index of the frame to get
     * @return the frame
     */
    public void paint(Graphics g, double zoom, boolean showTransparency,
            int x, int y, int frame) {

        Image image = frames[frame];

        // calculate the coordinates
        x = (int) ((x - origin.x) * zoom);
        y = (int) ((y - origin.y) * zoom);

        int width = (int) (image.getWidth(null) * zoom);
        int height = (int) (image.getHeight(null) * zoom);

        // display the entity
        if (showTransparency) {
            g.drawImage(image, x, y, width, height, null);
        }
        else {
            g.drawImage(image, x, y, width, height, MapEntity.bgColor, null);
        }
    }
}
