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
public class Sprite extends Observable {

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
     * Whether this sprite depends on the tileset.
     * If yes, the sprite sheet image file will come from the tileset
     * instead of from a specified fixed file.
     */
    private boolean tilesetDependent;

    /**
     * Id of the tileset used to draw this sprite (for tileset-dependent sprites).
     */
    private String tilesetId;

    /**
     * Default image for empty sprites.
     */
    private static Image emptySpriteImage = Project.getEditorImageOrEmpty("entity_npc.png");

    /**
     * Tells whether the sprite has changed since the last save.
     * True if there has been no modifications, false otherwise.
     */
    private boolean isSaved;

    /**
     * Loads the description file of the animation set used by this sprite
     * and builds its animation set.
     * @throws SpriteException if there is an error when analyzing the file.
     */
    public void load() throws SpriteException {

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
            throw new SpriteException(ex.getMessage());
        }
        catch (LuaError ex) {
            throw new SpriteException(ex.getMessage());
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
                    if (tilesetId.isEmpty()) {
                        throw new MapException("No tileset selected");
                    }
                    tilesetDependent = true;
                    srcImage = Project.getProjectImage(
                            "tilesets/" + Project.getTilesetEntitiesImageFile(tilesetId).getName());
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
                    int numColumns = directionTable.get("num_columns").optint(numFrames);

                    Rectangle firstFrameRectangle = new Rectangle(x, y, frameWidth, frameHeight);


                    try {
                      SpriteAnimationDirection direction =  new SpriteAnimationDirection(
                          srcImage, firstFrameRectangle, numFrames, numColumns, originX, originY
                          );
                      directions.add(direction);
                    }
                    catch (SpriteException ex) {
                      // The direction has invalid properties:
                      // rethow an exception with a more complete error message.
                      throw new MapException("Animation '" + animationName + "', direction "
                          + directions.size() + ": " + ex.getMessage());
                    }

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
     * @param tilesetId id of the tileset to use
     * @throws SpriteException If the sprite could not be loaded.
     */
    public Sprite(String animationSetId, String tilesetId) throws SpriteException {

        if (!isValidId(animationSetId)) {
            throw new SpriteException("Invalid sprite ID: '" + animationSetId + "'");
        }

        this.animationSetId = animationSetId;
        this.tilesetId = tilesetId;
        this.isSaved = false;

        load();
        setSaved(true);
    }

    /**
     * Creates a sprite from the specified animation set id
     * @param animationSetId id of the animation set to use
     * @throws SpriteException If the sprite could not be loaded.
     */
    public Sprite (String animationSetId) throws SpriteException {

        this(animationSetId, Project.getResource(ResourceType.TILESET).getIds()[0]);
    }

    /**
     * Creates a sprite from the specified animation set id
     * @param animationSetId id of the animation set to use
     * @param map the map where this sprite will be displayed (if any)
     * @throws SpriteException If the sprite could not be loaded.
     */
    public Sprite(String animationSetId, Map map) throws SpriteException {

        this(animationSetId, map.getTilesetId());
    }

    /**
     * Returns the id of the sprite.
     * @return the id of the sprite
     */
    public String getId() {
        return animationSetId;
    }

    /**
     * @brief Returns whether a string is a valid sprite id.
     * @param spriteId The id to check.
     * @return true if this is legal.
     */
    public static boolean isValidId(String spriteId) {

        if (spriteId.isEmpty()) {
            return false;
        }

        for (int i = 0; i < spriteId.length(); i++) {
            char c = spriteId.charAt(i);
            if (!Character.isLetterOrDigit(c) && c != '_' && c != '/') {
                return false;
            }
        }

        return true;
    }

    /**
     * Returns the sprite name.
     * @return The human name of the sprite.
     */
    public String getName() {
        Resource mapResource = Project.getResource(ResourceType.SPRITE);
        String name = animationSetId;

        try {
            name = mapResource.getElementName(animationSetId);
        }
        catch (QuestEditorException ex) {
            // Cannot happen: the sprite id must be valid.
            ex.printStackTrace();
        }
        return name;
    }

    /**
     * Changes the name of the sprite.
     * @param name New human-readable name of the sprite.
     */
    public void setName(String name) throws QuestEditorException {

        if (!name.equals(getName())) {
            Project.renameResourceElement(ResourceType.SPRITE, animationSetId, name);
        }
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
     * Set an animation of this sprite.
     * @param animationName name of the animation to set
     * @param animation the animation
     */
    public void setAnimation (String animationName, SpriteAnimation animation) {
        animations.put(animationName, animation);
        notifyObservers();
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
     * Changes the tileset used to draw this sprite.
     *
     * This function has an effect only for tileset-dependent sprites.
     *
     * @param tilesetId Id of the new tileset to use.
     * @throws SpriteException if there is an error when analyzing the file or
     * when loading the image.
     */
    public void notifyTilesetChanged(String tilesetId) throws SpriteException {

        if (tilesetId == null || !tilesetDependent) {
            return;
        }

        if (this.tilesetId == null || !tilesetId.equals(this.tilesetId)) {

            this.tilesetId = tilesetId;
            // Reload the sprite because its source image has changed.
            load();

            for (SpriteAnimation animation: animations.values()) {
                animation.setTilesetId(tilesetId);
                animation.reloadImage();
            }
        }
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

    /**
     * Returns whether or not the sprite has changed since the last save.
     * @return true if there has been no modifications, false otherwise
     */
    public boolean isSaved() {
        return isSaved;
    }

    /**
     * Sets whether the sprite has changed since the last save.
     * @param isSaved true if there has been no modifications.
     */
    public void setSaved(boolean isSaved) {
        this.isSaved = isSaved;
    }

    /**
     * Saves the sprite into its file.
     * @throws QuestEditorException if the file could not be written
     */
    public void save() throws QuestEditorException {

        String lastName = "";
        try {

            // Open the sprite file.
            File spriteFile = Project.getSpriteFile(animationSetId);
            PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(spriteFile)));

            // Animations.
            for (String name: animations.keySet()) {
                lastName = name;
                SpriteAnimation animation = animations.get(name);

                String srcImage = animation.getSrcImage();
                int frameDelay = animation.getFrameDelay();
                int loopOnFrame = animation.getLoopOnFrame();
                int nbDirections = animation.getNbDirections();

                out.println("animation{");
                out.println("  name = \"" + name + "\",");
                out.println("  src_image = \"" + srcImage + "\",");
                if (frameDelay > 0) {
                    out.println("  frame_delay = " + frameDelay + ",");
                    if (loopOnFrame > 0 && loopOnFrame < nbDirections) {
                        out.println("  frame_to_loop_on = " + loopOnFrame + ",");
                    }
                }

                // Directions.
                out.println("  directions = {");
                for (int directionId = 0; directionId < nbDirections; directionId++) {
                    SpriteAnimationDirection direction = animation.getDirection(directionId);

                    Point position = direction.getPosition();
                    Dimension dimension = direction.getSize();
                    Point origin = direction.getOrigin();

                    int x = (new Double(position.x)).intValue();
                    int y = (new Double(position.y)).intValue();
                    int frameWidth = (new Double(dimension.width)).intValue();
                    int frameHeight = (new Double(dimension.height)).intValue();
                    int originX = (new Double(origin.x)).intValue();
                    int originY = (new Double(origin.y)).intValue();
                    int numFrames = direction.getNbFrames();
                    int numColumns = direction.getNbColumns();

                    out.print("    {");
                    out.print(" x = " + x + ", y = " + y);
                    out.print(", frame_width = " + frameWidth + ", frame_height = " + frameHeight);
                    out.print(", origin_x = " + originX + ", origin_y = " + originY);
                    if (numFrames > 1) {
                        out.print(", num_frames = " + numFrames);
                    }
                    if (numColumns > 0) {
                        out.print(", num_columns = " + numColumns);
                    }
                    out.println(" },");
                }
                out.println("  },");

                out.println("}");
            }

            out.close();

            setSaved(true);
        }
        catch (Exception ex) {
            String message = "";
            if (lastName.length() > 0) {
                message = "Failed to save animation '" + lastName + "': ";
            }
            message += ex.getMessage();
            throw new QuestEditorException(message);
        }
    }
}

