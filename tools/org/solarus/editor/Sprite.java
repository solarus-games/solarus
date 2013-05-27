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
import java.io.*;
import java.util.*;
import org.solarus.editor.entities.*;

/**
 * Represents a sprite.
 */
public class Sprite {

    /**
     * Name of the animation set.
     */
    private String animationSetId;

    /**
     * Name of the default animation in the animation set.
     */
    private String defaultAnimationName;

    /**
     * The animation set of this sprite.
     */
    private TreeMap<String, SpriteAnimation> animations;

    /**
     * The map where this sprite is supposed to be displayed.
     */
    private Map map;

    /**
     * Default image for empty sprites.
     */
    private static Image emptySpriteImage = Project.getEditorImage("npc.png");

    /**
     * Analyzes the description file of the animation set used by this sprite
     * and builds its animation set.
     * @throws MapException if there is an error when analyzing the file
     */
    public void parse() throws MapException {

        int lineNumber = 0;
        String animationName = null;
        try {

            this.animations = new TreeMap<String, SpriteAnimation>();
            BufferedReader in = new BufferedReader(new FileReader(Project.getSpriteFile(animationSetId)));

            // read each animation
            String line;
            while ((line = in.readLine()) != null) {

                lineNumber++;
                if (line.length() == 0) {
                    continue;
                }

                // first line: animation info
                StringTokenizer tokenizer = new StringTokenizer(line);
                animationName = tokenizer.nextToken();
                String imageFileName = tokenizer.nextToken();
                int nbDirections = Integer.parseInt(tokenizer.nextToken());
                int frameDelay = Integer.parseInt(tokenizer.nextToken());
                int loopOnFrame = Integer.parseInt(tokenizer.nextToken());

                BufferedImage srcImage;

                if (!imageFileName.equals("tileset")) {
                  srcImage = Project.getProjectImage("sprites/" + imageFileName);
                }
                else {
                  srcImage = Project.getProjectImage("tilesets/" + Project.getTilesetEntitiesImageFile(map.getTilesetId()).getName());
                }

                SpriteAnimationDirection[] directions = new SpriteAnimationDirection[nbDirections];

                // read each direction
                for (int i = 0; i < nbDirections; i++) {

                    do {
                        line = in.readLine();
                        lineNumber++;
                        if (line == null) {
                            throw new MapException("Line " + lineNumber + ": Unexpected end of file");
                        }
                    }
                    while (line.length() == 0);

                    tokenizer = new StringTokenizer(line);
                    int x = Integer.parseInt(tokenizer.nextToken());
                    int y = Integer.parseInt(tokenizer.nextToken());
                    int width = Integer.parseInt(tokenizer.nextToken());
                    int height = Integer.parseInt(tokenizer.nextToken());
                    int originX = Integer.parseInt(tokenizer.nextToken());
                    int originY = Integer.parseInt(tokenizer.nextToken());
                    int nbFrames = Integer.parseInt(tokenizer.nextToken());
                    int nbColumns = Integer.parseInt(tokenizer.nextToken());

                    Rectangle firstFrameRectangle = new Rectangle(x, y, width, height);
                    directions[i] = new SpriteAnimationDirection(
                            srcImage, firstFrameRectangle, nbFrames, nbColumns, originX, originY);
                }
                SpriteAnimation animation = new SpriteAnimation(directions, frameDelay, loopOnFrame);
                animations.put(animationName, animation);
                if (defaultAnimationName == null) {
                    defaultAnimationName = animationName; // set first animation as the default one
                }
            }
        }
        catch (IOException ex) {
            throw new MapException(ex.getMessage());
        }
        catch (NumberFormatException ex) {
            throw new MapException("Sprite '" + animationSetId + "': Syntax error line " + lineNumber + ": Integer expected");
        }
        catch (NoSuchElementException ex) {
            throw new MapException("Sprite '" + animationSetId + "': Syntax error line " + lineNumber + ": Value expected");
        }
        catch (QuestEditorException ex) {
            throw new MapException("Sprite '" + animationSetId + "': Line " + lineNumber + ", animation '" + animationName + "':\n" + ex.getMessage());
        }
    }

    /**
     * Creates a sprite from the specified animation set id
     * @param animationSetId id of the animation set to use
     * @param map the map where this sprite will be displayed (if any)
     * @throws QuestEditorException If the sprite could not be loaded.
     */
    public Sprite(String animationSetId, Map map) throws MapException {

        this.animationSetId = animationSetId;
        this.map = map;
        parse();
    }

    /**
     * Returns the name of the default animation of this sprite.
     * @return The default animation name, i.e. the first one in the
     * description file, or null if the sprite is empty.
     */
    public String getDefaultAnimationName() {
        return defaultAnimationName;
    }

    /**
     * Returns whether the specified animation exists.
     * @param animationName Name of an animation.
     * @return true if such an animation exists.
     */
    public boolean hasAnimation(String animationName) {
        return animations.containsKey(animationName);
    }

    /**
     * Returns an animation of this sprite.
     * @param animationName Name of the animation to get
     * @return The corresponding animation.
     */
    public SpriteAnimation getAnimation(String animationName) {
        return animations.get(animationName);
    }

    /**
     * Returns a frame of this sprite.
     * @param animationName name of animation to use (null to pick the default one)
     * @param direction direction of animation
     * @param frame index of the frame to get
     * @return the frame, or null if the sprite is empty.
     */
    public Image getFrame(String animationName, int direction, int frame) {

        if (animations.isEmpty()) {
            return null;
        }

        if (animationName == null) {
            animationName = getDefaultAnimationName();
        }
        return animations.get(animationName).getFrame(direction, frame);
    }

    /**
     * Returns the origin point of this sprite.
     * @param animationName name of the animation to consider (null to pick the default one)
     * @param direction direction of animation
     * @return the corresponding origin point
     */
    public Point getOrigin(String animationName, int direction) {

        if (animations.isEmpty()) {
            return new Point(0, 0);
        }

        if (animationName == null) {
            animationName = getDefaultAnimationName();
        }
        return animations.get(animationName).getOrigin(direction);
    }

    /**
     * Returns the size of this sprite.
     * @param animationName name of the animation to consider (null to pick the default one)
     * @param direction direction of animation
     * @return the corresponding size
     */
    public Dimension getSize(String animationName, int direction) {

        if (animations.isEmpty()) {
            // Empty sprite.
            return new Dimension(16, 16);
        }

        if (animationName == null || !animations.containsKey(animationName)) {
            animationName = getDefaultAnimationName();
        }

        return animations.get(animationName).getSize(direction);
    }

    /**
     * Displays a frame of this sprite.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     * @param animationName name of animation to use (null to pick the default one)
     * @param direction direction of animation
     * @param frame index of the frame to get
     * @return the frame
     */
    public void paint(Graphics g, double zoom, boolean showTransparency,
            int x, int y, String animationName, int direction, int frame) {

        if (animations.isEmpty()) {
            // Empty sprite.

            if (showTransparency) {
                g.drawImage(emptySpriteImage, x, y, 16, 16, null);
            }
            else {
                g.drawImage(emptySpriteImage, x, y, 16, 16, MapEntity.bgColor, null);
            }
        }
        else {
            if (animationName == null) {
                animationName = getDefaultAnimationName();
            }
            animations.get(animationName).paint(g, zoom, showTransparency,
                    x, y, direction, frame);
        }
    }
}

