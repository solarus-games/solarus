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
import java.awt.image.BufferedImage;
import java.io.IOException;
import java.util.Observable;
import java.util.Vector;

/**
 * Represents an animation of a sprite.
 */
public class SpriteAnimation  extends Observable {

    /**
     * @brief The directions of this animation.
     */
    private Vector<SpriteAnimationDirection> directions;

    /**
     * @brief Interval in milliseconds between two frames
     * (this delay is the same for all directions),
     * 0 to let the sprite displayed (only possible when there is one frame)
     */
    private int frameDelay;

    /**
     * @brief Index of a frame to loop on when the animation is finished,
     * or -1 to make no loop.
     */
    private int loopOnFrame;

    /**
     * @brief Name of the source image of this animation.
     */
    private String srcImage;

    /**
     * @brief Id of the tileset used to draw this animation (if srcImage equals "tileset").
     */
    private String tilesetId;

    /**
     * Creates an animation.
     * @param directions the list of directions of this animation
     * @param frameDelay interval in milliseconds between two frames
     * @param loopOnFrame index of a frame to loop on when the animation is finished, or -1
     */
    public SpriteAnimation(Vector<SpriteAnimationDirection> directions, int frameDelay, int loopOnFrame) {
        this.directions = directions;
        this.frameDelay = frameDelay;
        this.loopOnFrame = loopOnFrame;
        this.srcImage = "tileset";
    }

    /**
     * Reloads the animation's image.
     * The observers are notified with the new image as parameter.
     * @throws SpriteException if image cannot be loaded
     */
    public void reloadImage() throws SpriteException {

        String imagePath;
        if (!srcImage.equals("tileset")) {
            imagePath = "sprites/" + srcImage;
        } else {
            imagePath = "tilesets/" + Project.getTilesetEntitiesImageFile(tilesetId).getName();
        }

        try {
            BufferedImage image = Project.getProjectImage(imagePath);
            for (SpriteAnimationDirection direction: directions) {
                direction.setSrcImage(image);
            }
            setChanged();
            notifyObservers(image);
        } catch (IOException ex) {
            throw new SpriteException("Can't load image " + imagePath + ":\n" + ex.getMessage());
        }
    }

    /**
     * Returns the name of the source image.
     * @return the name
     */
    public String getSrcImage () {

        return srcImage;
    }

    /**
     * Changes the source image.
     * @param srcImage the name of the source image
     * @throws SpriteException if this image could not be applied
     */
    public void setSrcImage (String srcImage) throws SpriteException {

        if (srcImage.equals(this.srcImage)) {
            return;
        }

        String previousSrcImage = this.srcImage;

        try {
            this.srcImage = srcImage;
            reloadImage();
        } catch (SpriteException ex) {
            this.srcImage = previousSrcImage;
            reloadImage();
            throw ex;
        }
    }

    /**
     * Returns the origin point of this animation.
     * @param direction direction of animation
     * @return the corresponding origin point
     */
    public Point getOrigin(int direction) {

        return directions.get(direction).getOrigin();
    }

    /**
     * Returns the size of frames in this animation.
     * @param direction direction of animation
     * @return the corresponding size
     */
    public Dimension getSize(int direction) {

        return directions.get(direction).getSize();
    }

    /**
     * Returns a frame of this animation.
     * @param direction direction to use
     * @param frame index of the frame to show
     * @return the frame
     */
    public Image getFrame(int direction, int frame) {

        return directions.get(direction).getFrame(frame);
    }

    /**
     * Returns the number of directions in this animation.
     * @return The number of directions.
     */
    public int getNbDirections() {
        return directions.size();
    }

    /**
     * Returns a direction in this animation.
     * @param direction Index of the direction to get (the first one is 0).
     * @return The corresponding direction.
     */
    public SpriteAnimationDirection getDirection(int direction) {
        return directions.get(direction);
    }

    /**
     * @brief Returns the time interval between two frames.
     *
     * This delay is the same for all directions.
     *
     * @return The interval in milliseconds between two frames.
     * 0 means infinite (only possible when there is one frame).
     */
    public int getFrameDelay() {
        return frameDelay;
    }

    /**
     * @brief Changes the time interval between two frames.
     *
     * This delay is the same for all directions.
     *
     * @param frameDelay The interval in milliseconds between two frames.
     * 0 means infinite (only possible when there is one frame).
     */
    public void setFrameDelay(int frameDelay) {

        this.frameDelay = frameDelay;
        setChanged();
        notifyObservers();
    }

    /**
     * @brief Returns the frame where the animation loops.
     * @returns The index of a frame where to the sprite loops after the last
     * frame, or -1 if there is no loop.
     */
    public int getLoopOnFrame() {
        return loopOnFrame;
    }

    /**
     * @brief Changes the frame where the animation loops.
     * @param index The index of a frame where to the sprite loops after the last
     * frame, or -1 if there is no loop.
     */
    public void setLoopOnFrame(int index) {

        this.loopOnFrame = index;
        setChanged();
        notifyObservers();
    }

    /**
     * Returns id of the tileset used to draw this animation.
     *
     * Used only if srcImage equals "tileset".
     *
     * @return The id of the tileset
     */
    public String getTilesetId () {

        return tilesetId;
    }

    /**
     * Changes id of the tileset used to draw this animation.
     *
     * Used only if srcImage equals "tileset".
     *
     * @param tilesetId The id of the tileset
     */
    public void setTilesetId (String tilesetId) throws SpriteException {

        this.tilesetId = tilesetId;
        if (srcImage.equals("tileset")) {
            try {
                reloadImage();
            } catch (SpriteException ex) {
                throw new SpriteException("Tileset image cannot be loaded:\n" + ex.getMessage());
            }
        } else {
            notifyObservers();
        }
    }

    /**
     * Displays a frame of this sprite.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     * @param direction direction of animation
     * @param frame index of the frame to get
     * @return the frame
     */
    public void paint(Graphics g, double zoom, boolean showTransparency,
            int x, int y, int direction, int frame) {

        directions.get(direction).paint(g, zoom, showTransparency, x, y, frame);
    }
}

