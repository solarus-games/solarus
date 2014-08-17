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
import java.io.File;
import java.io.IOException;
import java.util.Observable;
import java.util.Vector;

import javax.imageio.ImageIO;

/**
 * Represents an animation of a sprite.
 */
public class SpriteAnimation extends Observable {

    /**
     * @brief Name of the animation.
     */
    private String name;

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
     * @brief Name of the source image of this animation, possibly "tileset".
     */
    private String srcImageName;

    /**
     * The scaled images of current sprite animation.
     */
    private BufferedImage[] scaledImages;

    /**
     * @brief Id of the tileset used to draw this animation (if srcImage equals "tileset").
     */
    private String tilesetId;

    /**
     * @brief Image of this animation.
     */
    private BufferedImage srcImage = null;

    /**
     * Creates an animation.
     * @param name Name of the animation.
     * @param srcImageName the source image name of this animation
     * @param directions the list of directions of this animation
     * @param frameDelay interval in milliseconds between two frames
     * @param loopOnFrame index of a frame to loop on when the animation is finished, or -1
     * @param tilesetId the id of tileset to use (only if srcImageName = "tileset")
     */
    public SpriteAnimation(
            String name,
            String srcImageName,
            Vector<SpriteAnimationDirection> directions,
            int frameDelay,
            int loopOnFrame,
            String tilesetId) {
        this.name = name;
        this.directions = directions;
        this.frameDelay = frameDelay;
        this.loopOnFrame = loopOnFrame;
        this.srcImageName = srcImageName;
        this.tilesetId = tilesetId;
        this.scaledImages = new BufferedImage[Zoom.values().length];

        for (SpriteAnimationDirection direction: directions) {
            direction.setAnimation(this);
        }
    }

    /**
     * Copy constructor.
     * @param name Name of the animation to create.
     * @param other Existing animation to copy.
     */
    public SpriteAnimation(
            String name,
            SpriteAnimation other) throws SpriteException {
        this.name = name;
        this.directions = new Vector<SpriteAnimationDirection>();
        this.frameDelay = other.frameDelay;
        this.loopOnFrame = other.loopOnFrame;
        this.srcImageName = other.srcImageName;
        this.tilesetId = other.tilesetId;
        this.scaledImages = new BufferedImage[Zoom.values().length];

        try {
            for (SpriteAnimationDirection otherDirection: other.directions) {
                SpriteAnimationDirection direction = otherDirection.clone();
                direction.setAnimation(this);
                directions.add(direction);
            }
        } catch (CloneNotSupportedException ex) {
            ex.printStackTrace();
            throw new SpriteException(ex.getMessage());
        }
    }

    /**
     * Returns the name of this animation.
     * @return The name.
     */
    public String getName() {
        return name;
    }

    /**
     * Changes the name of this animation.
     * @param name The new name.
     */
    public void setName(String name) {

        this.name = name;
        setChanged();
        notifyObservers();
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
     * @return The corresponding direction or null if doesn't exist.
     */
    public SpriteAnimationDirection getDirection(int direction) {

        return direction >= 0 && direction < directions.size() ? directions.get(direction) : null;
    }

    /**
     * Returns a name describing a direction of this animation.
     *
     * If there are 4 or 8 directions, this is the direction number followed by
     * a hint like "up" or "right-left".
     * Otherwise, this is the direction number alone.
     *
     * @param direction Index of a direction in this animation.
     * @return A name describing the direction.
     */
    public String getDirectionName(int direction) {
        if (getNbDirections() == 4 &&
                direction < 4) {
            // 4-direction case.
            String[] directionNames = {
                    "right",
                    "up",
                    "left",
                    "down",
            };
            return direction + " (" + directionNames[direction] + ")";
        }

        if (getNbDirections() == 8 &&
                direction < 8) {
            // 8-direction case.
            String[] directionNames = {
                    "right",
                    "right-up",
                    "up",
                    "left-up",
                    "left",
                    "left-down",
                    "down",
                    "right-down"
            };
            return direction + " (" + directionNames[direction] + ")";
        }

        return Integer.toString(direction);
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
     * Add a new direction in this animation.
     * The direction is created with one frame corresponding to the rect.
     * @param rect the rect corresponding to the first frame of the direction
     * @return the added direction.
     * @throws SpriteException if the direction cannot be created.
     */
    public SpriteAnimationDirection addDirection(Rectangle rect) throws SpriteException {

        BufferedImage srcImage = getSrcImage();
        Point origin = new Point(rect.width / 2, rect.height - 3);

        SpriteAnimationDirection direction = new SpriteAnimationDirection(srcImage, rect, 1, 1, origin.x, origin.y);
        direction.setAnimation(this);
        directions.add(direction);

        setChanged();
        notifyObservers(direction);

        return direction;
    }

    /**
     * Clone a direction in this animation.
     * @param directionNb the number of the direction to clone
     * @param position the position of the clone, null to keep the same position
     * @return the added direction.
     * @throws SpriteException if the direction doesn't exists or cannot be cloned
     */
    public SpriteAnimationDirection cloneDirection(int directionNb, Point position)
            throws SpriteException {

        if (directionNb < 0 || directionNb >= directions.size()) {
            throw new SpriteException("No such direction: " + directionNb);
        }

        SpriteAnimationDirection direction = directions.get(directionNb);

        try {
            direction = direction.clone();
            if (position != null) {
                direction.setPosition(position);
            }
            directions.add(direction);

            setChanged();
            notifyObservers(direction);

            return direction;
        }
        catch (CloneNotSupportedException ex) {
            throw new SpriteException(ex.getMessage());
        }
    }

    /**
     * Remove a direction from this animation.
     * @param directionNb the number of the direction to remove
     * @return the removed direction
     * @throws SpriteException if the direction doesn't exist
     */
    public SpriteAnimationDirection removeDirection(int directionNb) throws SpriteException {

        if (directionNb < 0 || directionNb >= directions.size()) {
            throw new SpriteException("No such direction: " + directionNb);
        }

        SpriteAnimationDirection direction = directions.get(directionNb);
        directions.remove(directionNb);
        setChanged();
        notifyObservers(direction);
        return direction;
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
    public void setTilesetId(String tilesetId) throws SpriteException {

        if (srcImageName.equals("tileset")) {
            this.tilesetId = tilesetId;
            reloadSrcImage();
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

    /**
     * Reloads the animation's image.
     * The observers are notified with the new image as parameter.
     * The image will be null if it cannot be loaded.
     */
    public void reloadSrcImage() {

        try {
            scaledImages = new BufferedImage[Zoom.values().length];
            srcImage = ImageIO.read(getSrcImageFile());
            for (SpriteAnimationDirection direction: directions) {
                direction.setSrcImage(srcImage);
            }

            setChanged();
            notifyObservers(srcImage);
        }
        catch (SpriteException ex) {
            srcImage = null;
        }
        catch (IOException ex) {
            srcImage = null;
        }
    }

    /**
     * Returns the animation's image file.
     * @return the file of animation image
     */
    public File getSrcImageFile() {

        if (srcImageName.equals("tileset")) {

            return Project.getTilesetEntitiesImageFile(tilesetId);
        }

        return new File(Project.getDataPath() + "/sprites/" + srcImageName);
    }

    /**
     * Returns the name of the source image.
     * @return the name
     */
    public String getSrcImageName() {

        return srcImageName;
    }

    /**
     * Changes the source image.
     * @param srcImage the name of the source image
     * @throws SpriteException if this image could not be applied
     */
    public void setSrcImageName(String srcImageName) throws SpriteException {

        if (srcImageName.equals(this.srcImageName)) {
            return;
        }

        this.srcImageName = srcImageName;
        reloadSrcImage();
    }

    /**
     * Returns the animation's image.
     * @return The animation image or null if it cannot be loaded.
     */
    public BufferedImage getSrcImage() {

        // Try to reload image if not loaded yet.
        if (srcImage == null) {
            reloadSrcImage();
        }

        return srcImage;
    }

    /**
     * Creates a scaled version of an image (without any kind of interpolation).
     * @param image The original image.
     * @param width The new width.
     * @param height The new height.
     * @return The scaled image.
     */
    private BufferedImage createScaledImage(BufferedImage image, int width, int height) {

        BufferedImage scaledImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = scaledImage.createGraphics();
        g.drawImage(image, 0, 0, width, height, null);
        g.dispose();
        return scaledImage;
    }

    /**
     * Returns a scaled version of the animation's image, previously loaded by reloadImage().
     * @param zoom the zoom
     * @return the scaled animation's image, or null if the image is not loaded
     */
    public BufferedImage getScaledSrcImage(Zoom zoom) {

        if (srcImage == null) {
            return null;
        }

        int index = zoom.getIndex();
        double zoomValue = zoom.getValue();

        if (scaledImages[index] == null) {
            if (zoomValue == 1.0) {
                scaledImages[index] = srcImage;
            } else {
                int width = (int) Math.round(srcImage.getWidth() * zoomValue);
                int height = (int) Math.round(srcImage.getHeight() * zoomValue);
                scaledImages[index] = createScaledImage(srcImage, width, height);
            }
        }

        return scaledImages[index];
    }

}

