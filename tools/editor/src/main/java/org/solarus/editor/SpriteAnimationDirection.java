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
package org.solarus.editor;

import java.awt.*;
import java.awt.image.*;
import org.solarus.editor.entities.MapEntity;
import java.util.Observable;

/**
 * Represents a direction of animation of a sprite.
 */
public class SpriteAnimationDirection extends Observable {

    /**
     * @brief The animation this direction belongs to.
     */
    private SpriteAnimation animation;

    /**
     * @brief The frames of this animation direction.
     */
    private BufferedImage[] frames;

    /**
     * @brief The source image used to create frames of this direction.
     */
    private BufferedImage srcImage;

    /**
     * @brief Position point of the animation direction (position of the first frame).
     */
    private Point position;

    /**
     * @brief Size of the animation direction (the same for all frames).
     */
    private Dimension size;

    /**
     * @brief Origin point of the sprite (the same for all frames).
     */
    private Point origin;

    /**
     * @brief Number of frames of the animation direction.
     */
    private int nbFrames;

    /**
     * @brief Number of columns of the animation direction.
     */
    private int nbColumns;

    /**
     * Creates a direction of animation.
     * @param srcImage the source image to use
     * @param firstFrameRectangle rectangle representing the first frame
     * (the other ones will have the same size)
     * @param nbFrames number of frames to create
     * @param nbColumns number of columns (because the rectangles may be organized in several rows)
     * @param originX x coordinate of the sprite's origin in each rectangle
     * @param originY y coordinate of the sprite's origin in each rectangle
     * @throws SpriteException if some rectangles are outside the image.
     */
    public SpriteAnimationDirection(
            BufferedImage srcImage,
            Rectangle firstFrameRectangle,
            int nbFrames,
            int nbColumns,
            int originX,
            int originY)
            throws SpriteException {

        this.position = new Point(firstFrameRectangle.x, firstFrameRectangle.y);
        this.size = new Dimension(firstFrameRectangle.width, firstFrameRectangle.height);
        this.origin = new Point(originX, originY);
        this.nbColumns = nbColumns;
        this.nbFrames = nbFrames;

        setSrcImage(srcImage);
    }

    /**
     * Creates the frames of this direction.
     * @throws SpriteException if some rectangles are outside the image.
     */
    private void createFrames() throws RasterFormatException {

        int nbRows = getNbRows();
        frames = new BufferedImage[nbFrames];
        int frame = 0;
        for (int i = 0; i < nbRows && frame < nbFrames; i++) {
            for (int j = 0; j < nbColumns && frame < nbFrames; j++) {

                int x = position.x + j * size.width;
                int y = position.y + i * size.height;
                frames[frame] = srcImage.getSubimage(x, y, size.width, size.height);
                frame++;
            }
        }
    }

    /**
     * Returns the animation this direction belongs to.
     * @return The animation.
     */
    public SpriteAnimation getAnimation() {
        return animation;
    }

    /**
     * Sets the animation this directions belongs to.
     * @param animation The animation this direction belongs to.
     */
    public void setAnimation(SpriteAnimation animation) {
        this.animation = animation;
    }

    /**
     * Returns the number of rows for a number of frames.
     * @return the number of rows
     */
    public int getNbRows() {

        int nbRows = nbFrames / nbColumns;
        if (nbFrames % nbColumns != 0) {
            nbRows++;
        }
        return nbRows;
    }

    /**
     * Changes the source image used to create frames of this direction
     * @param srcImage the source image to use
     * @throws SpriteException if some rectangles are outside the image.
     */
    public void setSrcImage (BufferedImage srcImage) throws SpriteException {

        if (srcImage == null) {
            this.srcImage = null;
            frames = new BufferedImage[0];
            return;
        }

        int nbRows = getNbRows();
        int nbColumns = Math.min(this.nbColumns, nbFrames);
        try {
            if (position.x + nbColumns * size.width > srcImage.getWidth() ||
                    position.y + nbRows * size.height > srcImage.getHeight()) {
                throw new Exception();
            }
            this.srcImage = srcImage;
            createFrames();
            setChanged();
            notifyObservers();
        } catch (Exception ex) {
            throw new SpriteException("One of frames is outside the image bounds: (" +
                    position.x + "," + position.y + ")->(" +
                    (position.x + size.width) + "," + (position.y + size.height) +
                    "). Size of source image is only " + srcImage.getWidth() +
                    "x" + srcImage.getHeight() + ".\nPlease fix your sprite file.");
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
     * Changes the origin point of this direction.
     * @param origin the origin
     */
    public void setOrigin(Point origin) {

        this.origin = origin;
        setChanged();
        notifyObservers();
    }

    /**
     * Returns the rectangle of this direction.
     * @return the rectangle
     */
    public Rectangle getRect() {

        int width = size.width * Math.min(nbColumns, nbFrames);
        int height = size.height * getNbRows();
        return new Rectangle(position, new Dimension(width, height));
    }

    /**
     * Returns the rectangles of this direction, a rectangle for each frame.
     * @return the rectangle
     */
    public Rectangle[] getRects() {

        int nbRows = getNbRows();
        Rectangle[] rects = new Rectangle[nbFrames];

        int frame = 0;

        for (int y = 0; frame < nbFrames && y < nbRows; y++) {
            for (int x = 0; frame < nbFrames && x < nbColumns; x++) {

                rects[frame] = new Rectangle(
                        position.x + x * size.width, position.y + y * size.height,
                        size.width, size.height);
                frame++;
            }
        }

        return rects;
    }

    /**
     * Check if direction contains a point.
     * @return true if the point is in the direction, false otherwise
     */
    public boolean contains (int x, int y) {

        for (Rectangle rect: getRects()) {
            if (rect.contains(x, y)) {
                return true;
            }
        }
        return false;
    }

    /**
     * Returns the position of the first frame in this direction.
     * @return the position
     */
    public Point getPosition() {

        return position;
    }

    /**
     * Changes the position of the first frame in this direction.
     * @param position the position
     * @throws SpriteException if some rectangles are outside the image.
     */
    public void setPosition(Point position) throws SpriteException {

        Point prevPosition = this.position;

        try {
            this.position = position;
            setSrcImage(srcImage);
        } catch (SpriteException ex) {
            this.position = prevPosition;
            throw ex;
        }
    }

    /**
     * Returns the size of frames in this direction.
     * @return the size
     */
    public Dimension getSize() {

        return size;
    }

    /**
     * Changes the size of frames in this direction.
     * @param size the size
     * @throws SpriteException if some rectangles are outside the image.
     */
    public void setSize(Dimension size) throws SpriteException {

        Dimension prevSize = this.size;

        try {
            this.size = size;
            setSrcImage(srcImage);
        } catch (SpriteException ex) {
            this.size = prevSize;
            throw ex;
        }
    }

    /**
     * Returns the number of frames in this direction.
     * @return The number of frames.
     */
    public int getNbFrames() {

        return nbFrames;
    }

    /**
     * Changes the number of frames in this direction.
     * @param nbFrames the number of frames
     * @throws SpriteException if some rectangles are outside the image.
     */
    public void setNbFrames(int nbFrames) throws SpriteException {

        int prevNbFrames = this.nbFrames;

        try {
            this.nbFrames = nbFrames;
            setSrcImage(srcImage);
        } catch (SpriteException ex) {
            this.nbFrames = prevNbFrames;
            throw ex;
        }
    }

    /**
     * Returns the number of columns in this direction.
     * @return The number of columns.
     */
    public int getNbColumns() {

        return nbColumns;
    }

    /**
     * Changes the number of columns in this direction.
     * @param nbColumns the number of columns
     * @throws SpriteException if some rectangles are outside the image.
     */
    public void setNbColumns(int nbColumns) throws SpriteException {

        int prevNbColumns = this.nbColumns;

        try {
            this.nbColumns = nbColumns;
            setSrcImage(srcImage);
        } catch (SpriteException ex) {
            this.nbColumns = prevNbColumns;
            throw ex;
        }
    }

    /**
     * Returns a frame of this direction.
     * @param frame Index of the frame to get.
     * @return The corresponding frame.
     */
    public Image getFrame(int frame) {

        return frame >= 0 && frame < frames.length ? frames[frame] : null;
    }

    /**
     * Returns frames list of this direction.
     * @return The frames list.
     */
    public Image[] getFrames() {

        return frames;
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

        Image image = getFrame(frame);

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

    /**
     * Clone this direction.
     * @return the clone of this direction
     * @throws CloneNotSupportedException if cannot be cloned
     */
    @Override
    public SpriteAnimationDirection clone() throws CloneNotSupportedException {

        try {
            SpriteAnimationDirection direction = new SpriteAnimationDirection(
                    srcImage,
                    new Rectangle(position, size),
                    nbFrames,
                    nbColumns,
                    origin.x,
                    origin.y
            );
            direction.setAnimation(animation);
            return direction;
        } catch (SpriteException ex) {
            throw new CloneNotSupportedException(ex.getMessage());
        }
    }
}
