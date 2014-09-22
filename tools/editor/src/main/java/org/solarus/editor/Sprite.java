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
     * Name of the default animation in the animation set or an empty string.
     */
    private String defaultAnimationName;

    /**
     * The animation set of this sprite.
     */
    private TreeMap<String, SpriteAnimation> animations;

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

                BufferedImage srcImage = null;

                try {
                    if (!srcImageName.equals("tileset")) {
                        srcImage = Project.getProjectImage("sprites/" + srcImageName);
                    }
                    else if (!tilesetId.isEmpty()) {
                        srcImage = Project.getProjectImage(
                                "tilesets/" + Project.getTilesetEntitiesImageFile(tilesetId).getName());
                    }
                } catch (IOException ex) {
                    // image cannot be loaded
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
                          srcImage,
                          firstFrameRectangle,
                          numFrames,
                          numColumns,
                          originX,
                          originY
                      );
                      directions.add(direction);
                    }
                    catch (SpriteException ex) {
                      // The direction has invalid properties:
                      // rethow an exception with a more complete error message.
                      throw new SpriteException("Animation '" + animationName + "', direction "
                          + directions.size() + ": " + ex.getMessage());
                    }
                }

                SpriteAnimation animation = new SpriteAnimation(
                        animationName,
                        srcImageName,
                        directions,
                        frameDelay,
                        frameToLoopOn,
                        tilesetId
                );
                animations.put(animationName, animation);
                if (defaultAnimationName.isEmpty()) {
                    defaultAnimationName = animationName; // set first animation as the default one
                }
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
     * Possible values passed to notifyObservers() to inform observers
     * about what has changed.
     */
    public enum WhatChanged {
        ANIMATION_ADDED,
        ANIMATION_REMOVED,
        ANIMATION_RENAMED,
        DEFAULT_ANIMATION_CHANGED,
        SELECTED_ANIMATION_CHANGED,
        DIRECTION_ADDED,
        DIRECTION_REMOVED,
        SELECTED_DIRECTION_CHANGED
    }

    /**
     * Description of what has changed when notifying observers.
     */
    public class Change {

        private final WhatChanged whatChanged;

        private final Object[] info;

        /**
         * Creates a change description.
         * @param whatChanged Nature of the change.
         * @param info Optional info describing what animations or
         * directions are impacted by the change.
         */
        public Change(WhatChanged whatChanged, Object... info) {
            this.whatChanged = whatChanged;
            this.info = info;
        }

        public WhatChanged getWhatChanged() {
            return whatChanged;
        }

        public Object getInfo() {
            return getInfo(0);
        }

        public Object getInfo(int index) {
            return info[index];
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
            throw new SpriteException("Invalid sprite id: '" + animationSetId + "'");
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
    public Sprite(String animationSetId) throws SpriteException {

        this(animationSetId, "");

        String[] tilesetIds = Project.getResource(ResourceType.TILESET).getIds();
        if (tilesetIds.length > 0) {
            notifyTilesetChanged(tilesetIds[0]);
        }
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
     * description file, or an empty string if the sprite is empty.
     */
    public String getDefaultAnimationName() {
        return defaultAnimationName;
    }

    /**
     * Changes the default animation of this sprite.
     * @param animationName the name of the new default animation,
     * or an empty string to make no explicit default animation.
     */
    public void setDefaultAnimation(String animationName) {

        if (hasAnimation(animationName) || animationName.isEmpty()) {
            defaultAnimationName = animationName;

            isSaved = false;
            setChanged();
            notifyObservers(new Change(WhatChanged.DEFAULT_ANIMATION_CHANGED));
        }
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
                if (direction.contains(x, y)) {
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

        if (animationName.equals(selectedAnimationName)) {
            return;
        }

        if (animationName.isEmpty()) {
            // unselect direction
            selectedDirectionNb = -1;
        } else if (!animations.containsKey(animationName) ) {
            throw new SpriteException("No such animation: '" + animationName + "'");
        } else {
            int nbDirections = animations.get(animationName).getNbDirections();
            if (nbDirections > 0) {
                // select the last direction if wanted direction doesn't exist
                if (selectedDirectionNb >= nbDirections) {
                    selectedDirectionNb = nbDirections - 1;
                }
            } else {
                selectedDirectionNb = -1;
            }
        }

        selectedAnimationName = animationName;
        if (!selectedAnimationName.isEmpty()) {
            getSelectedAnimation().reloadSrcImage();
        }

        setChanged();
        notifyObservers(new Change(WhatChanged.SELECTED_ANIMATION_CHANGED));
    }

    /**
     * Returns the selected animation.
     * @return null is no animation is selected, the animation otherwise.
     */
    public SpriteAnimation getSelectedAnimation() {

        return selectedAnimationName.isEmpty() ? null : animations.get(selectedAnimationName);
    }

    /**
     * Sets the selected animation of this sprite.
     * @param animation The animation to select or null to unselect.
     * @throws SpriteException if the animation doesn't exist
     */
    public void setSelectedAnimation(SpriteAnimation animation) throws SpriteException {
        setSelectedAnimation(animation.getName());
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

        if (selectedDirectionNb == this.selectedDirectionNb) {
            return;
        }

        // select a direction
        if (selectedDirectionNb != -1) {
            SpriteAnimation animation = getSelectedAnimation();
            if (animation == null) {
                throw new SpriteException("Cannot select direction " +
                        selectedDirectionNb + ": no animation was selected");
            } else if (selectedDirectionNb >= animation.getNbDirections()) {
                throw new SpriteException("Cannot select direction " +
                        selectedDirectionNb + ": no such direction");
            }
        }
        this.selectedDirectionNb = selectedDirectionNb;
        setChanged();
        notifyObservers(new Change(WhatChanged.SELECTED_DIRECTION_CHANGED));
    }

    /**
     * Selects a direction and notifies the observers.
     * @param direction The direction to select or null to unselect.
     * @throws SpriteException if the direction doesn't exist.
     */
    public void setSelectedDirection(SpriteAnimationDirection direction) throws SpriteException {

        if (direction == null) {
            unselectDirection();
            return;
        }

        SpriteAnimation animation = getSelectedAnimation();
        if (animation == null) {
            throw new SpriteException("No animation is selected");
        }
        for (int i = 0; i < animation.getNbDirections(); i++) {
            SpriteAnimationDirection currentDirection = animation.getDirection(i);
            if (currentDirection == direction) {
                // Found it.
                setSelectedDirectionNb(i);
                return;
            }
        }

        throw new SpriteException("No such direction");
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

        if (getSelectedAnimation() != null && selectedDirectionNb != -1) {
            return getSelectedAnimation().getDirection(selectedDirectionNb);
        }
        else {
            return null;
        }
    }

    /**
     * Returns a name describing the selected direction.
     *
     * If there are 4 or 8 directions, this is the direction number followed by
     * a hint like "up" or "right-left".
     * Otherwise, this is the direction number alone.
     *
     * @return A name describing the selected direction, or an empty string if
     * no direction is selected.
     */
    public String getSelectedDirectionName() {

        int direction = selectedDirectionNb;
        if (direction == -1) {
            // No direction is selected.
            return "";
        }

        return getSelectedAnimation().getDirectionName(direction);
    }

    /**
     * Add a new animation in this sprite.
     * @param name the name of animation to add
     * @throws SpriteException if animation name already exist.
     */
    public void addAnimation(String name) throws SpriteException {

        if (name.isEmpty()) {
            throw new SpriteException("Animation name is empty");
        }

        if (animations.containsKey(name)) {
            throw new SpriteException("Animation '" + name + "' already exists");
        }

        // If an animation is already selected, set the same source image
        // to the new animation.
        String srcImageName = "";
        SpriteAnimation selectedAnimation = getSelectedAnimation();
        if (selectedAnimation != null) {
            srcImageName = selectedAnimation.getSrcImageName();
        }

        Vector<SpriteAnimationDirection> directions = new Vector<SpriteAnimationDirection>();
        SpriteAnimation animation = new SpriteAnimation(
                name,
                srcImageName,
                directions,
                0,
                -1,
                tilesetId
        );

        animations.put(name, animation);

        // Notify observers that an animation has just been added.
        setChanged();
        notifyObservers(new Change(WhatChanged.ANIMATION_ADDED, name));

        // Select the newly created animation.
        // (This will notify observers again.)
        setSelectedAnimation(name);
    }

    /**
     * Clones the selected animation into a new animation.
     * @throws SpriteException If the animation could not be created.
     */
    public void cloneAnimation(String newAnimationName) throws SpriteException {

        if (newAnimationName.isEmpty()) {
            throw new SpriteException("Animation name is empty");
        }

        if (animations.containsKey(newAnimationName)) {
            throw new SpriteException("Animation '" + newAnimationName + "' already exists");
        }

        // If an animation is already selected, set the same source image
        // to the new animation.
        SpriteAnimation selectedAnimation = getSelectedAnimation();
        if (selectedAnimation == null) {
            throw new SpriteException("No animation is selected");
        }

        SpriteAnimation newAnimation = new SpriteAnimation(newAnimationName, selectedAnimation);

        animations.put(newAnimationName, newAnimation);

        // Notify observers that an animation has just been added.
        isSaved = false;
        setChanged();
        notifyObservers(new Change(WhatChanged.ANIMATION_ADDED, newAnimationName));

        // Select the newly created animation.
        // (This will notify observers again.)
        setSelectedAnimation(newAnimationName);
    }

    /**
     * Rename an animation in this sprite.
     * @param oldName The name of animation to rename.
     * @param newName The new name of the animation.
     * @throws SpriteException If the animation doesn't exist or the new name
     * is already used or is invalid.
     */
    public void renameAnimation(String oldName, String newName) throws SpriteException {

        if (newName.isEmpty()) {
            throw new SpriteException("The new name is empty" );
        }

        if (!animations.containsKey(oldName)) {
            throw new SpriteException("No such animation: '" + oldName + "'");
        }

        if (oldName.equals(newName)) {
            return;
        }

        if (animations.containsKey(newName)) {
            throw new SpriteException("Animation '" + newName + "' already exists" );
        }

        SpriteAnimation animation = animations.get(oldName);
        animation.setName(newName);

        // The key has changed.
        animations.remove(oldName);
        animations.put(newName, animation);

        if (selectedAnimationName.equals(oldName)) {
            // The animation renamed was the selected one.
            selectedAnimationName = newName;
        }

        if (oldName.equals(defaultAnimationName)) {
            // The animation renamed was the default one.
            defaultAnimationName = newName;
        }

        isSaved = false;
        setChanged();
        notifyObservers(new Change(WhatChanged.ANIMATION_RENAMED, oldName, newName));
        setChanged();
        notifyObservers(new Change(WhatChanged.SELECTED_ANIMATION_CHANGED, newName));
    }

    /**
     * Add a new direction to the selected animation.
     * The direction is created with one frame corresponding to the rect.
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
        if (directionNb == 0) {
            // This is the first direction in this animation.
            animation.addDirection(rect);
        }
        else {
            // Some directions already exist: clone the last one.
            animation.cloneDirection(directionNb - 1, null);
        }

        isSaved = false;
        setChanged();
        notifyObservers(new Change(WhatChanged.DIRECTION_ADDED, directionNb));

        setSelectedDirectionNb(directionNb);
    }

    /**
     * Clone the selected animation direction in the selected animation.
     * @param position the position of the clone, null to keep the same position
     * @throws SpriteException if no animation or direction was selected
     */
    public void cloneDirection(Point position) throws SpriteException {

        SpriteAnimation animation = getSelectedAnimation();
        if (animation == null) {
            throw new SpriteException("No selected animation");
        }

        if (selectedDirectionNb == -1) {
            throw new SpriteException("No selected direction");
        }

        animation.cloneDirection(selectedDirectionNb, position);

        isSaved = false;
        setChanged();
        notifyObservers(new Change(WhatChanged.DIRECTION_ADDED, animation.getNbDirections() - 1));

        setSelectedDirectionNb(animation.getNbDirections() - 1);
    }

    /**
     * Clones the selected animation direction in the selected animation.
     * @throws SpriteException if no animation or direction was selected
     */
    public void cloneDirection() throws SpriteException {

        cloneDirection(null);
    }

    /**
     * Remove the selected animation from this sprite.
     * @throws SpriteException if the animation doesn't exist
     */
    public void removeAnimation() throws SpriteException {

        if (selectedAnimationName != null && !selectedAnimationName.isEmpty()) {
            String animationName = selectedAnimationName;

            animations.remove(animationName);
            unselectAnimation();
            if (animationName.equals(defaultAnimationName)) {
                setDefaultAnimation("");
            }

            isSaved = false;
            setChanged();
            notifyObservers(new Change(WhatChanged.ANIMATION_REMOVED, animationName));
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
            unselectDirection();
            animation.removeDirection(directionNb);
            isSaved = false;
            setChanged();
            notifyObservers(new Change(WhatChanged.DIRECTION_REMOVED, animation.getName(), directionNb));
        }
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
     * Changes the tileset used to draw this sprite.
     *
     * This function has an effect only for tileset-dependent sprites.
     *
     * @param tilesetId Id of the new tileset to use.
     * @throws SpriteException if there is an error when analyzing the file or
     * when loading the image.
     */
    public void notifyTilesetChanged(String tilesetId) throws SpriteException {

        if (tilesetId == null) {
            return;
        }

        if (this.tilesetId == null || !tilesetId.equals(this.tilesetId)) {

            this.tilesetId = tilesetId;
            for (SpriteAnimation animation: animations.values()) {
                animation.setTilesetId(tilesetId);
                animation.reloadSrcImage();
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

        // Open the sprite file.
        File spriteFile = Project.getSpriteFile(animationSetId);
        try (PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(spriteFile)))) {

            if (!defaultAnimationName.isEmpty() && animations.containsKey(defaultAnimationName)) {

                lastName = defaultAnimationName;
                SpriteAnimation animation = animations.get(defaultAnimationName);
                out.println(animationToString(defaultAnimationName, animation));
            }

            // Animations.
            for (String name: animations.keySet()) {

                if (defaultAnimationName.isEmpty() || !name.equals(defaultAnimationName)) {
                    lastName = name;
                    SpriteAnimation animation = animations.get(name);
                    out.println(animationToString(name, animation));
                }
            }

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

    public static String animationToString(String name, SpriteAnimation animation) {

        // TODO use a StringBuffer
        String str = "animation{\n";

        String srcImageName = animation.getSrcImageName();
        int frameDelay = animation.getFrameDelay();
        int loopOnFrame = animation.getLoopOnFrame();
        int nbDirections = animation.getNbDirections();

        str += "  name = \"" + name + "\",\n";
        str += "  src_image = \"" + srcImageName + "\",\n";
        if (frameDelay > 0) {
            str += "  frame_delay = " + frameDelay + ",\n";
            if (loopOnFrame >= 0) {
                str += "  frame_to_loop_on = " + loopOnFrame + ",\n";
            }
        }

        // Directions.
        str += "  directions = {\n";
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

            str += "    { x = " + x + ", y = " + y;
            str += ", frame_width = " + frameWidth + ", frame_height = " + frameHeight;
            str += ", origin_x = " + originX + ", origin_y = " + originY;
            if (numFrames > 1) {
                str += ", num_frames = " + numFrames;
                if (numColumns > 0 && numColumns < numFrames) {
                    str += ", num_columns = " + numColumns;
                }
            }
            str += " },\n";
        }
        str += "  },\n}";
        return str;
    }
}

