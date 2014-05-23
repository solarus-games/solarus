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
     * Name of the animation currently selected by the user.
     * null: no animation is selected
     */
    private String selectedAnimationName;

    /**
     * Id of the direction currently selected by the user.
     * -1: no direction is selected
     * 0 or more: an existing direction is selected
     */
    private int selectedDirectionNb;

    private BufferedImage doubleImage;

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

        @Override
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
                        throw new SpriteException("No tileset selected");
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
                      throw new SpriteException("Animation '" + animationName + "', direction "
                          + directions.size() + ": " + ex.getMessage());
                    }

                    SpriteAnimation animation = new SpriteAnimation(srcImageName,
                            directions, frameDelay, frameToLoopOn, tilesetId);
                    animations.put(animationName, animation);
                    if (defaultAnimationName == "") {
                        defaultAnimationName = animationName; // set first animation as the default one
                    }
                }
            }
            catch (IOException ex) {
                // File error.
                throw new LuaError(ex);
            }
            catch (SpriteException ex) {
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
        this.selectedAnimationName = "";
        this.defaultAnimationName = "";
        this.selectedDirectionNb = -1;

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
     * @throws QuestEditorException if the resource cannot be renamed
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
     * Retuns animation names of this sprite.
     * @return animation names of this sprite.
     */
    public Collection<String> getAnimationNames() {

        return animations.keySet();
    }

    /**
     * Retuns animations of this sprite.
     * @return animations of this sprite.
     */
    public Collection<SpriteAnimation> getAnimations() {

        return animations.values();
    }

    /**
     * Returns the number of the direction at a location in the sprite,
     * or -1 if there is no direction there.
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     * @return number of the direction at this point, or -1 if there is no direction here
     */
    public int getDirectionNbAt(int x, int y) {

        SpriteAnimation animation = getSelectedAnimation();

        if (animation != null) {
            for (int i = 0; i < animation.getNbDirections(); i++) {
                SpriteAnimationDirection direction = animation.getDirection(i);
                if (direction.getRect().contains(x, y)) {
                    return i; // a direction was found at this point
                }
            }
        }

        return -1; // no direction found
    }

    /**
     * Returns the name of the selected animation.
     * @return the name of animation or an empty string if no animation is selected.
     */
    public String getSelectedAnimationName() {

        return selectedAnimationName;
    }

    /**
     * Set the selected animation of this sprite.
     * @param animationName an empty string to unselect, the name of animation to select
     * @throws SpriteException if the animation doesn't exist
     */
    public void setSelectedAnimation(String animationName) throws SpriteException {

        if (!animationName.equals(selectedAnimationName)) {
            if (animationName.isEmpty()) {
                // unselect direction
                selectedDirectionNb = -1;
            } else if (!animations.containsKey(animationName) ) {
                throw new SpriteException("the animation " + animationName + " doesn't exist");
            } else {
                // select the first direction
                selectedDirectionNb = 0;
            }

            selectedAnimationName = animationName;
            reloadImage();
            setChanged();
            notifyObservers();
        }
    }

    /**
     * Returns the selected animation.
     * @return null is no animation is selected, the animation otherwise.
     */
    public SpriteAnimation getSelectedAnimation() {

        return selectedAnimationName.isEmpty() ? null : animations.get(selectedAnimationName);
    }

    /**
     * Returns the number of the selected direction.
     * @return -1 if no direction is selected, 0 or more if an existing direction is selected.
     */
    public int getSelectedDirectionNb() {

        return selectedDirectionNb;
    }

    /**
     * Selects a direction and notifies the observers.
     * @param selectedDirectionNb -1 to select no direction,
     * 0 or more to select the existing direction with this number.
     * @throws SpriteException if the selected direction doesn't exist.
     */
    public void setSelectedDirectionNb(int selectedDirectionNb) throws SpriteException {

        if (selectedDirectionNb != this.selectedDirectionNb) {

            // select a direction
            if (selectedDirectionNb != -1) {
                SpriteAnimation animation = getSelectedAnimation();
                if (animation == null) {
                    throw new SpriteException("Cannot select direction number " +
                            selectedDirectionNb + ": no animation was selected");
                } else if (selectedDirectionNb >= animation.getNbDirections()) {
                    throw new SpriteException("Cannot select direction number " +
                            selectedDirectionNb + ": this direction doesn't exist");
                }
            }
            this.selectedDirectionNb = selectedDirectionNb;
            setChanged();
            notifyObservers(getSelectedDirection());
        }
    }

    /**
     * Unselects the current animation.
     * This is equivalent to call setSelectedAnimation("").
     */
    public void unselectAnimation() {

        try {
            setSelectedAnimation("");
        } catch (SpriteException ex) {
            // setSelectedAnimation don't throw exception if the direction is -1
        }
    }

    /**
     * Unselects the current direction.
     * This is equivalent to call setSelectedDirectionNb(-1).
     */
    public void unselectDirection() {

        try {
            setSelectedDirectionNb(-1);
        } catch (SpriteException ex) {
            // setSelectedDirectionNb don't throw exception if the direction is -1
        }
    }

    /**
     * Returns the selected direction.
     * @return the selected direction, or null if there is no selected direction
     */
    public SpriteAnimationDirection getSelectedDirection() {
        if (selectedDirectionNb != -1) {
            return getSelectedAnimation().getDirection(selectedDirectionNb);
        }
        else {
            return null;
        }
    }

    /**
     * Add a new animation in this sprite.
     * @param name the name of animation to add
     * @throws SpriteException if animation name already exist.
     */
    public void addAnimation(String name) throws SpriteException {

        if (animations.containsKey(name)) {
            throw new SpriteException("Cannot create animation '" + name + "': this name already exist");
        }

        Vector<SpriteAnimationDirection> directions = new Vector<SpriteAnimationDirection>();
        SpriteAnimation animation = new SpriteAnimation("", directions, 0, 0, tilesetId);

        animations.put(name, animation);
        selectedAnimationName = name;
        selectedDirectionNb = -1;
        reloadImage();

        isSaved = false;
        setChanged();
        notifyObservers(animation);
    }

    /**
     * Rename an animation in this sprite.
     * @param name the name of animation to rename
     * @param newName the new name of the animation
     * @throws SpriteException if animation doesn't exist or name already exist.
     */
    public void renameAnimation(String name, String newName) throws SpriteException {

        if (!animations.containsKey(name)) {
            throw new SpriteException("Cannot rename animation '" + name + "': this animation doesn't exist" );
        }
        if (animations.containsKey(newName)) {
            throw new SpriteException("Cannot rename animation '" + name + "': the name '" +
                    newName + "' already exist" );
        }

        SpriteAnimation animation = animations.remove(name);
        animations.put(newName, animation);

        isSaved = false;
        setChanged();
        notifyObservers(animation);
    }

    /**
     * Add a new direction in the current selected animation.
     * the direction is create with one frame corresponding to the rect and his
     * origin point centered.
     * @param rect the rect corresponding to the first frame of the direction
     * @throws SpriteException if no animation was selected or if the direction
     * cannot be created.
     */
    public void addDirection(Rectangle rect) throws SpriteException {

        SpriteAnimation animation = getSelectedAnimation();

        if (animation == null) {
            throw new SpriteException("No selected animation");
        }

        int directionNb = animation.getNbDirections();
        SpriteAnimationDirection direction = animation.addDirection(rect);
        selectedDirectionNb = directionNb;

        isSaved = false;
        setChanged();
        notifyObservers(direction);
    }

    /**
     * Remove the selected animation from this sprite.
     * @throws SpriteException if the animation doesn't exist
     */
    public void removeAnimation() throws SpriteException {

        if (selectedAnimationName != null && !selectedAnimationName.isEmpty()) {
            String animationName = selectedAnimationName;

            animations.remove(animationName);
            reloadImage();
            selectedAnimationName = "";
            selectedDirectionNb = -1;

            isSaved = false;
            setChanged();
            notifyObservers(animationName);
        } else {
            throw new SpriteException("No animation is selected");
        }
    }

    /**
     * Remove the selected direction from the selected animation of this sprite.
     * @throws SpriteException if the direction doesn't exist
     */
    public void removeDirection() throws SpriteException {

        if (selectedDirectionNb != -1) {

            SpriteAnimation animation = getSelectedAnimation();

            if (animation == null) {
                throw new SpriteException("No animation is selected");
            }

            int directionNb = selectedDirectionNb;
            animation.removeDirection(selectedDirectionNb);
            selectedDirectionNb = -1;
            isSaved = false;
            setChanged();
            notifyObservers(directionNb);
        }
    }

    /**
     * Reloads the selected animation's image.
     * The observers are notified with the new image as parameter.
     */
    public void reloadImage() {

        SpriteAnimation animation = getSelectedAnimation();

        if (animation != null) {
            try {
                animation.reloadImage();
                BufferedImage image = animation.getImage();
                int width = image.getWidth();
                int height = image.getHeight();
                doubleImage = createScaledImage(animation.getImage(), width * 2, height * 2);
            } catch (SpriteException ex) {
                doubleImage = null;
            }
        } else {
            doubleImage = null;
        }
    }

    private BufferedImage createScaledImage(BufferedImage image, int width, int height) {
        BufferedImage scaledImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = scaledImage.createGraphics();
        g.drawImage(image, 0, 0, width, height, null);
        g.dispose();
        return scaledImage;
    }

    /**
     * Returns the 200% scaled version of the selected animation's image, previously
     * loaded by reloadImage().
     * @return the selected animation's image in 200%, or null if the image is not loaded
     */
    public BufferedImage getDoubleImage() {

        return doubleImage;
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
        notifyObservers(animation);
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
            }
        }
    }

    /**
     * Displays a frame of this sprite.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     * @param x horizontal position to draw
     * @param y vertical position to draw
     * @param animationName name of animation to use (null to pick the default one)
     * @param direction direction of animation
     * @param frame index of the frame to get
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

                    int x = (int) position.x;
                    int y = (int) position.y;
                    int frameWidth = (int) dimension.width;
                    int frameHeight = (int) dimension.height;
                    int originX = (int) origin.x;
                    int originY = (int) origin.y;
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
        catch (IOException ex) {
            String message = "";
            if (lastName.length() > 0) {
                message = "Failed to save animation '" + lastName + "': ";
            }
            message += ex.getMessage();
            throw new QuestEditorException(message);
        }
    }
}

