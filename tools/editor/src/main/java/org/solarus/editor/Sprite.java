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

import org.luaj.vm2.LoadState;
import org.luaj.vm2.LuaError;
import org.luaj.vm2.LuaFunction;
import org.luaj.vm2.LuaTable;
import org.luaj.vm2.LuaValue;
import org.luaj.vm2.Varargs;
import org.luaj.vm2.compiler.LuaC;
import org.luaj.vm2.lib.OneArgFunction;
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
    private static Image emptySpriteImage = Project.getEditorImageOrEmpty("entity_npc.png");

    /**
     * Loads the description file of the animation set used by this sprite
     * and builds its animation set.
     * @throws MapException if there is an error when analyzing the file.
     */
    public void load() throws MapException {

        try {
            this.animations = new TreeMap<String, SpriteAnimation>();
            File spriteFile = Project.getSpriteFile(animationSetId);
            LuaC.install();
            LuaTable environment = LuaValue.tableOf();

            environment.set("animation", new AnimationFunction());

            LuaFunction code = LoadState.load(new FileInputStream(spriteFile),
                spriteFile.getName(), environment);
            code.call();
        }
        catch (IOException ex) {
            throw new MapException(ex.getMessage());
        }
        catch (LuaError ex) {
            throw new MapException(ex.getMessage());
        }
    }

    /**
     * @brief Lua function animation() called by the sprite data file.
     */
    private class AnimationFunction extends OneArgFunction {

        public LuaValue call(LuaValue arg) {

            try {
                LuaTable table = arg.checktable();

                String animationName = table.get("name").checkjstring();
                String srcImageName = table.get("src_image").checkjstring();
                int frameDelay = table.get("frame_delay").optint(0);
                int frameToLoopOn = table.get("frame_to_loop_on").optint(-1);
                LuaTable directionsTable = table.get("directions").checktable();

                BufferedImage srcImage;

                if (!srcImageName.equals("tileset")) {
                    srcImage = Project.getProjectImage("sprites/" + srcImageName);
                }
                else {
                    String tilesetId = map.getTilesetId();
                    if (tilesetId.isEmpty()) {
                        throw new MapException("No tileset selected");
                    }
                    srcImage = Project.getProjectImage(
                            "tilesets/" + Project.getTilesetEntitiesImageFile(map.getTilesetId()).getName());
                }

                Vector<SpriteAnimationDirection> directions = new Vector<SpriteAnimationDirection>();

                // Traverse the directions table.
                LuaValue key = LuaValue.NIL;
                while (true) {

                    Varargs keyValue = directionsTable.next(key);
                    key = keyValue.arg1();
                    if (key.isnil()) {
                        break;
                    }
                    LuaValue directionTable = keyValue.arg(2);

                    int x = directionTable.get("x").checkint();
                    int y = directionTable.get("y").checkint();
                    int frameWidth = directionTable.get("frame_width").checkint();
                    int frameHeight = directionTable.get("frame_height").checkint();
                    int originX = directionTable.get("origin_x").optint(0);
                    int originY = directionTable.get("origin_y").optint(0);
                    int numFrames = directionTable.get("num_frames").optint(1);
                    int numColumns = directionTable.get("num_colums").optint(numFrames);

                    Rectangle firstFrameRectangle = new Rectangle(x, y, frameWidth, frameHeight);
                    directions.add(new SpriteAnimationDirection(
                            srcImage, firstFrameRectangle, numFrames, numColumns, originX, originY));

                    SpriteAnimation animation = new SpriteAnimation(directions, frameDelay, frameToLoopOn);
                    animations.put(animationName, animation);
                    if (defaultAnimationName == null) {
                        defaultAnimationName = animationName; // set first animation as the default one
                    }
                }
            }
            catch (IOException ex) {
                // File error.
                throw new LuaError(ex);
            }
            catch (MapException ex) {
                // Error in the input file.
                throw new LuaError(ex);
            }
            catch (Exception ex) {
                // Error in the editor.
                ex.printStackTrace();
                throw new LuaError(ex);
            }

            return LuaValue.NIL;
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
        load();
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

