/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
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
import java.util.Map;

import org.solarus.editor.entities.MapEntity;

/**
 * Represents a sprite.
 */
public class Sprite {

    /**
     * Name of the animation set.
     */
    private String animationSetId;

    /**
     * The animation set of this sprite.
     */
    private Map<String, SpriteAnimation> animations;

    /**
     * Analyzes the description file of the animation set used by this sprite
     * and builds its animation set.
     * @throws ZSDXException if there is an error when analyzing the file
     */
    public void parse() throws MapException {

        int lineNumber = 0;
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
                String animationName = tokenizer.nextToken();
                String imageFileName = tokenizer.nextToken();
                int nbDirections = Integer.parseInt(tokenizer.nextToken());
                int frameDelay = Integer.parseInt(tokenizer.nextToken());
                int loopOnFrame = Integer.parseInt(tokenizer.nextToken());

                BufferedImage srcImage = Project.getProjectImage("sprites/" + imageFileName);
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
            }
        }
        catch (IOException ex) {
            throw new MapException(ex.getMessage());
        }
        catch (NumberFormatException ex) {
            throw new MapException("Line " + lineNumber + ": Integer expected");
        }
        catch (NoSuchElementException ex) {
            throw new MapException("Line " + lineNumber + ": Value expected");
        }
    }

    /**
     * Creates a sprite from the specified animation set id
     * @param animationSetId id of the animation set to use 
     */
    public Sprite(String animationSetId) throws MapException {

        this.animationSetId = animationSetId;
        parse();
    }

    /**
     * Returns a frame of this sprite.
     * @param animationName name of animation to use (null to pick the default one)
     * @param direction direction of animation
     * @param frame index of the frame to get
     * @return the frame
     */
    public Image getFrame(String animationName, int direction, int frame) {

        if (animationName == null) {
            animationName = animations.keySet().iterator().next();
        }
        return animations.get(animationName).getFrame(direction, frame);
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

        if (animationName == null) {
            animationName = animations.keySet().iterator().next();
        }
        animations.get(animationName).paint(g, zoom, showTransparency,
                x, y, direction, frame);        
    }
}

