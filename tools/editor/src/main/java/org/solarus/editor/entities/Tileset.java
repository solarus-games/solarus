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
package org.solarus.editor.entities;

import java.util.*;
import java.io.*;
import java.awt.*;
import java.awt.image.*;
import javax.imageio.*;
import org.luaj.vm2.*;
import org.luaj.vm2.lib.*;
import org.luaj.vm2.compiler.*;
import org.solarus.editor.*;

/**
 * This class describes a tileset.
 * A tileset is observable. When it changes, the observers are notified with
 * a parameter indicating what has just changed:
 *   - a TilePattern: indicates that this tile pattern was created
 *   - an Integer: indicates that the tile pattern with this id was removed
 *   - null: other cases
 */
public class Tileset extends Observable {

    // tileset data

    /**
     * Id of the tileset.
     */
    private String tilesetId;

    /**
     * Background color (default is black).
     */
    private Color backgroundColor;

    /**
     * The tile patterns.
     * The key of a tile pattern is its id in the tileset.
     */
    private TreeMap<String, TilePattern> tilePatterns;

    /**
     * Maximum integer id of a tile in the map.
     */
    private int maxId;

    /**
     * The tileset image.
     */
    private BufferedImage image;

    /**
     * The scaled tileset images.
     */
    private BufferedImage[] scaledImages;

    // information about the user actions on the tileset

    /**
     * Tells whether the tileset has changed since the last save.
     * True if there has been no modifications, false otherwise.
     */
    private boolean isSaved;

    /**
     * Id of the tile pattern currently selected by the user,
     * or null if no tile pattern is selected.
     */
    private String selectedTilePatternId;

    /**
     * Creates or loads a tileset.
     * @param tilesetId Id of the tileset to create (may be a new one or an existing one).
     * @throws QuestEditorException if the tileset could not be loaded
     */
    public Tileset(String tilesetId) throws QuestEditorException {

        if (!isValidId(tilesetId)) {
            throw new MapException("Invalid tileset id: '" + tilesetId + "'");
        }

        this.backgroundColor = Color.BLACK;
        this.isSaved = false;
        this.maxId = 0;
        this.selectedTilePatternId = null; // none
        this.tilePatterns = new TreeMap<String, TilePattern>(new NaturalOrderComparator<String>());
        this.tilesetId = tilesetId;

        load();
        setSaved(true);
    }

    /**
     * Returns the id of the tileset.
     * @return the id of the tileset
     */
    public String getId() {
        return tilesetId;
    }

    /**
     * @brief Returns whether a string is a valid tileset id.
     * @param tilesetId The id to check.
     * @return true if this is legal.
     */
    public static boolean isValidId(String tilesetId) {

        if (tilesetId.isEmpty()) {
            return false;
        }

        for (int i = 0; i < tilesetId.length(); i++) {
            char c = tilesetId.charAt(i);
            if (!Character.isLetterOrDigit(c) && c != '_') {
                return false;
            }
        }

        return true;
    }

    /**
     * @brief Returns whether a string is a valid tile pattern id.
     * @param patternId The id to check.
     * @return true if this is legal.
     */
    public static boolean isValidPatternId(String patternId) {

        if (patternId.isEmpty()) {
            return false;
        }

        for (int i = 0; i < patternId.length(); i++) {
            char c = patternId.charAt(i);
            if (
                    c == '\"' ||
                    c == '\'' ||
                    c == '\\' ||
                    Character.getType(c) == Character.LINE_SEPARATOR
               ) {
                return false;
            }
        }

        return true;
    }

    /**
     * Returns a string representation of this tileset.
     * @return The name of the tileset.
     */
    @Override
    public String toString() {
        return getName();
    }

    /**
     * Returns the tileset name.
     * @return The human name of the tileset.
     */
    public String getName() {
        Resource mapResource = Project.getResource(ResourceType.TILESET);
        String name = tilesetId;

        try {
            name = mapResource.getElementName(tilesetId);
        }
        catch (QuestEditorException ex) {
            // Cannot happen: the tileset id must be valid.
            ex.printStackTrace();
        }
        return name;
    }

    /**
     * Changes the name of the tileset.
     * @param name New human-readable name of the tileset.
     */
    public void setName(String name) throws QuestEditorException {

        if (!name.equals(getName())) {
            Project.renameResourceElement(ResourceType.TILESET, tilesetId, name);
        }
    }

    /**
     * Returns the background color of the tileset.
     * @return the background color
     */
    public Color getBackgroundColor() {
        return backgroundColor;
    }

    /**
     * Changes the background color of the tileset.
     * @param backgroundColor the new background color
     */
    public void setBackgroundColor(Color backgroundColor) {
        this.backgroundColor = backgroundColor;
        setSaved(false);
        setChanged();
        notifyObservers();
    }

    /**
     * Reloads the tileset's image.
     * The observers are notified with the new image as parameter.
     */
    public void reloadImage() {
        try {
            Zoom zooms[] = Zoom.values();
            scaledImages = new BufferedImage[zooms.length];

            image = ImageIO.read(getImageFile());
            int width = image.getWidth(null);
            int height = image.getHeight(null);

            for (int i = 0; i < zooms.length; i++) {

                double zoom = zooms[i].getValue();
                if (zoom != 1.0) {
                    scaledImages[i] = createScaledImage(image, (int) Math.round(width * zoom),
                            (int) Math.round(height * zoom));
                } else {
                    scaledImages[i] = image;
                }
            }
        }
        catch (IOException e) {
            image = null;
            scaledImages = null;
        }

        setChanged();
        notifyObservers(image);
    }

    private BufferedImage createScaledImage(BufferedImage image, int width, int height) {
        BufferedImage scaledImage = new BufferedImage(width, height, BufferedImage.TYPE_INT_ARGB);
        Graphics2D g = scaledImage.createGraphics();
        g.drawImage(image, 0, 0, width, height, null);
        g.dispose();
        return scaledImage;
    }

    /**
     * Returns the tileset's image file.
     * @return the image file of the tileset
     */
    public File getImageFile() {
        return Project.getTilesetImageFile(tilesetId);
    }

    /**
     * Returns the tileset's image, previously loaded by reloadImage().
     * @return the tileset's image, or null if the image is not loaded
     */
    public BufferedImage getImage() {
        return image;
    }

    /**
     * Returns a scaled version of the tileset image, previously loaded by reloadImage().
     * @param zoom the zoom
     * @return the scaled tileset image, or null if the image is not loaded
     */
    public BufferedImage getScaledImage(Zoom zoom) {
        return scaledImages == null ? null : scaledImages[zoom.getIndex()];
    }

    /**
     * Returns a scaled version of the tileset image, previously loaded by reloadImage().
     * @param zoomValue the value of zoom
     * @return the scaled tileset image, or null if the image is not loaded
     */
    public BufferedImage getScaledImage(double zoomValue) {

        Zoom zoom = Zoom.get(zoomValue);
        if (zoom == null) {
            return null;
        }
        return getScaledImage(zoom);
    }

    /**
     * Returns the 200% scaled version of the tileset's image, previously loaded by reloadImage().
     * @return the tileset's image in 200%, or null if the image is not loaded
     */
    public BufferedImage getDoubleImage() {
        return getScaledImage(Zoom.BIG);
    }

    /**
     * Returns the number of tile patterns in the tileset.
     * @return the number of tile patterns
     */
    public int getNbTilePatterns() {
        return tilePatterns.size();
    }

    /**
     * Returns the ids of the tile patterns.
     * @return the ids of the tile patterns
     */
    public Set<String> getTilePatternIds() {
        return tilePatterns.keySet();
    }

    /**
     * Returns the tile patterns of this tileset.
     * @return the tile patterns
     */
    public Collection<TilePattern> getTilePatterns() {
        return tilePatterns.values();
    }

    /**
     * Returns a tile pattern.
     * @param id id of the tile pattern to get
     * @return the tile pattern with this id
     * @throws NoSuchElementException if there is no tile pattern with this id
     */
    public TilePattern getTilePattern(String id) throws NoSuchElementException {

        TilePattern tilePattern = tilePatterns.get(id);

        if (tilePattern == null) {
            throw new NoSuchElementException("There is no tile pattern with id '" + id + "' in the tileset.");
        }

        return tilePattern;
    }

    /**
     * Returns the id of the tile pattern at a location in the tileset,
     * or 0 if there is no tile pattern there.
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     * @return id of the tile pattern at this point, or null if there is no tile pattern here
     */
    public String getTilePatternIdAt(int x, int y) {

        for (String id: getTilePatternIds()) {
            Rectangle patternRectangle = getTilePattern(id).getPositionInTileset();
            if (patternRectangle.contains(x, y)) {
                return id; // a tile pattern was found at this point
            }
        }

        return null; // no tile pattern found
    }

    /**
     * Returns the id of the selected tile pattern.
     * @return The id of the selected tile pattern, or null if no tile pattern
     * is selected.
     */
    public String getSelectedTilePatternId() {
        return selectedTilePatternId;
    }

    /**
     * Selects a tile patterns and notifies the observers.
     * @param selectedTilePatternId id of the tile pattern to select, or null
     * to unselect any previously selected tile pattern.
     */
    public void setSelectedTilePatternId(String selectedTilePatternId) {

        if (this.selectedTilePatternId == null && selectedTilePatternId == null) {
            return;
        }

        if (this.selectedTilePatternId == null ||
                selectedTilePatternId == null ||
                !selectedTilePatternId.equals(this.selectedTilePatternId)
            ) {
            this.selectedTilePatternId = selectedTilePatternId;
            setChanged();
            notifyObservers();
        }
    }

    /**
     * Unselects the current tile pattern.
     * This is equivalent to setSelectedTileId(null).
     */
    public void unselectTilePattern() {
        setSelectedTilePatternId(null);
    }

    /**
     * Returns the selected tile pattern.
     * @return The selected tile pattern, or null if there is no selected
     * tile pattern or if doesn't exist yet.
     */
    public TilePattern getSelectedTilePattern() {
        if (selectedTilePatternId != null) {
            return getTilePattern(selectedTilePatternId);
        }
        else {
            return null;
        }
    }

    /**
     * Creates the tile pattern specified by the current selection area
     * adds it to the tileset and make it selected.
     * The observers are notified with the created TilePattern as parameter.
     * @param newTilePatternArea Position of the new tile pattern.
     * @param ground Ground property of the created tile pattern.
     * @throws TilesetException if the tile size is incorrect
     */
    public void addTilePattern(Rectangle newTilePatternArea, Ground ground)
        throws TilesetException {

        TilePattern tilePattern = new TilePattern(newTilePatternArea, Layer.LOW, ground);

        maxId++;
        String id = Integer.toString(maxId);
        tilePatterns.put(id, tilePattern);

        isSaved = false;

        setChanged();
        notifyObservers(tilePattern); // indicates that a tile pattern has been created

        setSelectedTilePatternId(id);
    }

    /**
     * Adds a tile pattern to the tileset.
     * @param tilePatternId id of the tile pattern
     * @param tilePattern the tile pattern to add
     */
    private void addTilePattern(String tilePatternId, TilePattern tilePattern) {
        tilePatterns.put(tilePatternId, tilePattern);
    }

    /**
     * Removes the selected tile pattern.
     * The observers are notified with the removed tile pattern as parameter.
     */
    public void removeTilePattern() {
        String id = getSelectedTilePatternId();

        if (id != null) {
            tilePatterns.remove(id);
            setSelectedTilePatternId(null);

            isSaved = false;

            setChanged();
            notifyObservers(id); // indicate that the tile has been removed
        }
    }

    /**
     * Changes the id of a tile pattern.
     * Observers are notified twice: with the old tile pattern id that gets
     * removes, and then with the new tile pattern.
     * @param oldPatternId Id to change.
     * @param newPatternId New id.
     * @throws TilesetException If the old pattern id does not exist or if the
     * new one is invalid or already used.
     */
    public void changeTilePatternId(String oldPatternId, String newPatternId) throws TilesetException {

        if (newPatternId.equals(oldPatternId)) {
            return;
        }

        TilePattern pattern = getTilePattern(oldPatternId);
        if (pattern == null) {
            throw new TilesetException("No such tile pattern: '" + oldPatternId + "'");
        }

        if (!isValidPatternId(newPatternId)) {
            throw new TilesetException("Invalid tile pattern id: '" + newPatternId + "'");
        }

        if (tilePatterns.containsKey(newPatternId)) {
            throw new TilesetException("Tile pattern '" + newPatternId + "' already exists");
        }

        String selectedPatternId = getSelectedTilePatternId();
        boolean wasSelected = selectedPatternId != null && selectedPatternId.equals(oldPatternId);

        // Remove the old one.
        if (wasSelected) {
            setSelectedTilePatternId(null);
        }

        tilePatterns.remove(oldPatternId);
        isSaved = false;
        setChanged();
        notifyObservers(oldPatternId);

        // Add the new one.

        int intId;
        try {
            intId = Integer.parseInt(newPatternId);
            // If the new id is an integer, keep maxId up to date.
            if (intId > maxId) {
                maxId = intId;
            }
        }
        catch (NumberFormatException ex) {
        }

        tilePatterns.put(newPatternId, pattern);
        setChanged();
        notifyObservers(pattern);

        if (wasSelected) {
            setSelectedTilePatternId(newPatternId);
        }
    }

    /**
     * Returns whether or not the tileset has changed since the last save.
     * @return true if there has been no modifications, false otherwise
     */
    public boolean isSaved() {
        return isSaved;
    }

    /**
     * Sets whether the tileset has changed since the last save.
     * @param isSaved true if there has been no modifications.
     */
    public void setSaved(boolean isSaved) {
        this.isSaved = isSaved;
    }

    /**
     * Loads the tileset from its file.
     * @throws QuestEditorException if the file could not be read.
     */
    public void load() throws QuestEditorException {

        try {
            File tilesetFile = Project.getTilesetFile(tilesetId);
            LuaC.install();
            LuaTable environment = LuaValue.tableOf();

            environment.set("background_color", new BackgroundColorFunction());
            environment.set("tile_pattern", new TilePatternFunction());

            LuaFunction code = LoadState.load(new FileInputStream(tilesetFile),
                    tilesetFile.getName(), environment);
            code.call();
        }
        catch (IOException ex) {
            throw new QuestEditorException(ex.getMessage());
        }
        catch (LuaError ex) {
            throw new QuestEditorException("Error when loading the tileset file: " + ex.getMessage());
        }

        setChanged();
        notifyObservers();
    }

    /**
     * Saves the tileset into its file.
     * @throws QuestEditorException if the file could not be written
     */
    public void save() throws QuestEditorException {

        String lastPatternId = null;
        try {

            // Open the tileset file.
            File tilesetFile = Project.getTilesetFile(tilesetId);
            PrintWriter out = new PrintWriter(new BufferedWriter(new FileWriter(tilesetFile)));

            // Background color.
            out.println("background_color{ "
                    + backgroundColor.getRed()
                    + ", "
                    + backgroundColor.getGreen()
                    + ", "
                    + backgroundColor.getBlue()
                    + " }");

            // Tile patterns.
            for (String id: getTilePatternIds()) {
                lastPatternId = id;
                TilePattern tilePattern = getTilePattern(id);

                TilePattern.Animation animation = tilePattern.getAnimation();
                int width = tilePattern.getWidth();
                int height = tilePattern.getHeight();
                String x;
                String y;
                if (tilePattern.isMultiFrame()) {
                    int x1 = tilePattern.getX();
                    int x2 = x1;
                    int x3 = x1;
                    int y1 = tilePattern.getY();
                    int y2 = y1;
                    int y3 = y1;
                    TilePattern.AnimationSeparation separation = tilePattern.getAnimationSeparation();
                    if (separation == TilePattern.AnimationSeparation.HORIZONTAL) {
                        x2 = x1 + width;
                        x3 = x2 + width;
                    }
                    else {
                        y2 = y1 + height;
                        y3 = y2 + height;
                    }
                    if (animation == TilePattern.Animation.SEQUENCE_012
                            || animation == TilePattern.Animation.SEQUENCE_012_PARALLAX) {
                        x = "{ " + x1 + ", " + x2 + ", " + x3 + " }";
                        y = "{ " + y1 + ", " + y2 + ", " + y3 + " }";
                            }
                    else {
                        x = "{ " + x1 + ", " + x2 + ", " + x3 + ", " + x2 + " }";
                        y = "{ " + y1 + ", " + y2 + ", " + y3 + ", " + y2 + " }";
                    }
                }
                else {
                    x = Integer.toString(tilePattern.getX());
                    y = Integer.toString(tilePattern.getY());
                }

                // TODO store the scrolling string in the enum
                String scrolling = null;
                if (animation == TilePattern.Animation.SELF_SCROLLING) {
                    scrolling = "self";
                }
                else if (animation == TilePattern.Animation.PARALLAX_SCROLLING
                        || animation == TilePattern.Animation.SEQUENCE_012_PARALLAX
                        || animation == TilePattern.Animation.SEQUENCE_0121_PARALLAX) {
                    scrolling = "parallax";
                        }

                out.println("tile_pattern{");
                out.println("  id = \"" + id + "\",");
                out.println("  ground = \"" + tilePattern.getGround().getName() + "\",");
                out.println("  default_layer = " + tilePattern.getDefaultLayer().getId() + ",");
                out.println("  x = " + x + ",");
                out.println("  y = " + y + ",");
                out.println("  width = " + width + ",");
                out.println("  height = " + height + ",");
                if (scrolling != null) {
                    out.println("  scrolling = \"" + scrolling + "\",");
                }
                out.println("}");
                out.println();
            }

            out.close();

            setSaved(true);
        }
        catch (Exception ex) {
            String message = "";
            if (lastPatternId != null) {
                message = "Failed to save tile '" + lastPatternId + "': ";
            }
            message += ex.getMessage();
            throw new QuestEditorException(message);
        }
    }

    /**
     * @brief Lua function background_color() called by the tileset data file.
     */
    private class BackgroundColorFunction extends OneArgFunction {

        public LuaValue call(LuaValue arg) {

            LuaTable colorTable = arg.checktable();
            setBackgroundColor(new Color(
                        colorTable.get(1).checkint(),
                        colorTable.get(2).checkint(),
                        colorTable.get(3).checkint()
                        ));
            return LuaValue.NIL;
        }
    }

    /**
     * @brief Lua function tile_pattern() called by the tileset data file.
     */
    private class TilePatternFunction extends OneArgFunction {

        public LuaValue call(LuaValue arg) {

            String id = null;
            Layer defaultLayer = null;
            int width = 0;
            int height = 0;
            int[] x = { -1, -1, -1, -1 };
            int[] y = { -1, -1, -1, -1 };
            Ground ground = null;
            String scrolling = null;
            int i = 0;
            int j = 0;

            try {
                LuaTable tilePatternTable = arg.checktable();
                LuaValue key = LuaValue.NIL;
                while (true) {
                    Varargs keyValue = tilePatternTable.next(key);
                    key = keyValue.arg1();
                    if (key.isnil()) {
                        break;
                    }
                    LuaValue value = keyValue.arg(2);

                    String keyString = key.checkjstring();
                    if (keyString.equals("id")) {
                        id = value.checkjstring();
                    }
                    else if (keyString.equals("ground")) {
                        String groundName = value.checkjstring();
                        ground = Ground.getByName(groundName);
                    }
                    else if (keyString.equals("default_layer")) {
                        defaultLayer = Layer.get(value.checkint());
                    }
                    else if (keyString.equals("x")) {
                        if (value.isint()) {
                            // Single frame.
                            x[0] = value.checkint();
                            i = 1;
                        }
                        else {
                            // Multi-frame.
                            LuaTable xTable = value.checktable();
                            LuaValue current;
                            do {
                                current = xTable.get(i + 1);
                                if (current.isint()) {
                                    x[i] = current.checkint();
                                    i++;
                                }
                            } while (i < 4 && current.isint());
                        }
                    }
                    else if (keyString.equals("y")) {
                        if (value.isint()) {
                            // Single frame.
                            y[0] = value.checkint();
                            j = 1;
                        }
                        else {
                            // Multi-frame.
                            LuaTable yTable = value.checktable();
                            LuaValue current;
                            do {
                                current = yTable.get(j + 1);
                                if (current.isint()) {
                                    y[j] = current.checkint();
                                    j++;
                                }
                            } while (j < 4 && current.isint());
                        }
                    }
                    else if (keyString.equals("width")) {
                        width = value.checkint();
                    }
                    else if (keyString.equals("height")) {
                        height = value.checkint();
                    }
                    else if (keyString.equals("scrolling")) {
                        scrolling = value.checkjstring();
                    }
                    else {
                        throw new LuaError("Unknown key '" + keyString + "'");
                    }
                }

                // TODO check data

                // Create the tile pattern.
                Rectangle positionInTileset = new Rectangle();
                positionInTileset.x = x[0];
                positionInTileset.y = y[0];
                positionInTileset.width = width;
                positionInTileset.height = height;
                TilePattern.AnimationSeparation separation = TilePattern.AnimationSeparation.HORIZONTAL;
                TilePattern.Animation animation = TilePattern.Animation.NONE;
                if (i > 1) {
                    // Multi-frame.
                    if (x[0] == x[1]) {
                        separation = TilePattern.AnimationSeparation.VERTICAL;
                        positionInTileset.height *= 3;
                    }
                    else {
                        separation = TilePattern.AnimationSeparation.HORIZONTAL;
                        positionInTileset.width *= 3;
                    }
                    if (scrolling != null && scrolling.equals("parallax")) {
                        animation = (i == 3) ?
                            TilePattern.Animation.SEQUENCE_012_PARALLAX :
                            TilePattern.Animation.SEQUENCE_0121_PARALLAX;
                    }
                    else {
                        animation = (i == 3) ?
                            TilePattern.Animation.SEQUENCE_012 :
                            TilePattern.Animation.SEQUENCE_0121;
                    }
                }
                else {
                    // Single frame.
                    if (scrolling == null) {
                        animation = TilePattern.Animation.NONE;
                    }
                    else if (scrolling.equals("parallax")) {
                        animation = TilePattern.Animation.PARALLAX_SCROLLING;
                    }
                    else if (scrolling.equals("self")) {
                        animation = TilePattern.Animation.SELF_SCROLLING;
                    }
                }

                // Add the tile pattern.
                addTilePattern(id, new TilePattern(
                            positionInTileset,
                            defaultLayer,
                            ground,
                            animation,
                            separation
                            ));
            }
            catch (Exception ex) {
                String message = "";
                if (id != null) {
                    message += "Failed to load tile '" + id + "': ";
                }
                message += ex.getMessage();
                throw new LuaError(message);
            }

            // If the tile pattern id is an integer, check the maximum integer id.
            int intId;
            try {
                intId = Integer.parseInt(id);
                if (intId > maxId) {
                    maxId = intId;
                }
            }
            catch (NumberFormatException ex) {
            }

            setSaved(true);

            return LuaValue.NIL;
        }
    }
}

