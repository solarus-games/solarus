/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
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
     * Name of the tileset.
     */
    private String name;

    /**
     * Background color (default is black).
     */
    private Color backgroundColor;

    /**
     * The tile patterns.
     * The key of a tile pattern is its id in the tileset. The first id is 1.
     */
    private TreeMap<Integer,TilePattern> tilePatterns;

    /**
     * Maximum id of a tile in the map.
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
     * Id of the tile pattern currently selected by the user.
     * 0: no tile pattern is selected
     * 1 or more: an existing tile pattern is selected
     * -1: a new tile pattern is selected, ready to be created
     */
    private int selectedTilePatternId;

    /**
     * Position of the tile pattern the user is creating,
     * or null if there no new tile pattern selected.
     */
    private Rectangle newTilePatternArea;

    /**
     * True if the new tile pattern area is overlapping an existing tile pattern.
     * Is so, the tile pattern cannot be created.
     */
    private boolean isNewTilePatternAreaOverlapping;

    // TODO store this directly in the Obstacle enum
    private static final HashMap<String, Obstacle> obstaclesByName =
        new HashMap<String, Obstacle>();
    static {
        obstaclesByName.put("traversable", Obstacle.NONE);
        obstaclesByName.put("wall", Obstacle.OBSTACLE);
        obstaclesByName.put("wall_top_right", Obstacle.TOP_RIGHT);
        obstaclesByName.put("wall_top_left", Obstacle.TOP_LEFT);
        obstaclesByName.put("wall_bottom_left", Obstacle.BOTTOM_LEFT);
        obstaclesByName.put("wall_bottom_right", Obstacle.BOTTOM_RIGHT);
        obstaclesByName.put("empty", Obstacle.EMPTY);
        obstaclesByName.put("water_top_right", Obstacle.TOP_RIGHT_WATER);
        obstaclesByName.put("water_top_left", Obstacle.TOP_LEFT_WATER);
        obstaclesByName.put("water_bottom_left", Obstacle.BOTTOM_LEFT_WATER);
        obstaclesByName.put("water_bottom_right", Obstacle.BOTTOM_RIGHT_WATER);
        obstaclesByName.put("deep_water", Obstacle.DEEP_WATER);
        obstaclesByName.put("shallow_water", Obstacle.SHALLOW_WATER);
        obstaclesByName.put("hole", Obstacle.HOLE);
        obstaclesByName.put("ladder", Obstacle.LADDER);
        obstaclesByName.put("prickles", Obstacle.PRICKLE);
        obstaclesByName.put("lava", Obstacle.LAVA);
    }

    /**
     * Creates or loads a tileset.
     * @param tilesetId Id of the tileset to create (may be a new one or an existing one).
     * @throws QuestEditorException if the tileset could not be loaded
     */
    public Tileset(String tilesetId) throws QuestEditorException {

        if (!isValidId(tilesetId)) {
            throw new MapException("Invalid tileset ID: '" + tilesetId + "'");
        }

        this.backgroundColor = Color.BLACK;
        this.isSaved = false;
        this.maxId = 0;
        this.selectedTilePatternId = 0; // none
        this.tilePatterns = new TreeMap<Integer,TilePattern>();
        this.tilesetId = tilesetId;

        Resource tilesetResource = Project.getResource(ResourceType.TILESET);
        if (tilesetResource.exists(tilesetId)) {
            load();
        }
        else {
            this.name = "New tileset";
            reloadImage();
            setSaved(true);
            setChanged();
            notifyObservers();
        }
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
     * @param mapId The id to check.
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
     * Returns the name of the tileset.
     * @return the name of the tileset, for example "Light World"
     */
    public String getName() {
        return name;
    }

    public String toString() {
        return name;
    }

    /**
     * Changes the name of the tileset.
     * @param name the name of the tileset, for example "Light World"
     */
    public void setName(String name) {

        if (!name.equals(this.name)) {
            this.name = name;
            setSaved(false);
            setChanged();
            notifyObservers();
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
            scaledImages = new BufferedImage[4];

            image = ImageIO.read(getImageFile());
            int width = image.getWidth(null);
            int height = image.getHeight(null);

            scaledImages[0] = createScaledImage(image, width / 4, height / 4);

            scaledImages[1] = createScaledImage(image, width / 2, height / 2);

            scaledImages[2] = image;

            scaledImages[3] = createScaledImage(image, width * 2, height * 2);
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
     * @param zoom an integer representing the scale (0: 25%, 1: 50%, 2: 100%, 3: 200%)
     * @return the scaled tileset image, or null if the image is not loaded
     */
    public BufferedImage getScaledImage(int zoom) {
        return scaledImages == null ? null : scaledImages[zoom];
    }

    /**
     * Returns a scaled version of the tileset image, previously loaded by reloadImage().
     * @param zoom the scale (0.25, 0.5, 1 or 2)
     * @return the scaled tileset image, or null if the image is not loaded
     */
    public BufferedImage getScaledImage(double zoom) {

        if (scaledImages == null) {
            return null;
        }

        int index;
        if (zoom == 0.25) {
            index = 0;
        }
        else if (zoom == 0.5) {
            index = 1;
        }
        else if (zoom == 1.0) {
            index = 2;
        }
        else {
            index = 3;
        }

        return scaledImages[index];
    }

    /**
     * Returns the 200% scaled version of the tileset's image, previously loaded by reloadImage().
     * @return the tileset's image in 200%, or null if the image is not loaded
     */
    public BufferedImage getDoubleImage() {
        return scaledImages == null ? null : scaledImages[3];
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
    public Set<Integer> getTilePatternIds() {
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
    public TilePattern getTilePattern(int id) throws NoSuchElementException {

        TilePattern tilePattern = tilePatterns.get(id);

        if (tilePattern == null) {
            throw new NoSuchElementException("There is no tile pattern with id #" + id + " in the tileset.");
        }

        return tilePattern;
    }

    /**
     * Returns the id of the tile pattern at a location in the tileset,
     * or 0 if there is no tile pattern there.
     * @param x x coordinate of the point
     * @param y y coordinate of the point
     * @return id of the tile pattern at this point, or 0 if there is no tile pattern here
     */
    public int getTilePatternIdAt(int x, int y) {

        for (int id: getTilePatternIds()) {
            Rectangle patternRectangle = getTilePattern(id).getPositionInTileset();
            if (patternRectangle.contains(x, y)) {
                return id; // a tile pattern was found at this point
            }
        }

        return 0; // no tile pattern found
    }

    /**
     * Returns the id of the selected tile pattern.
     * @return 0 if no tile pattern is selected, 1 or more if an existing tile pattern is selected,
     * or -1 if a new tile pattern is selected
     */
    public int getSelectedTilePatternId() {
        return selectedTilePatternId;
    }

    /**
     * Selects a tile patterns and notifies the observers.
     * @param selectedTilePatternId 0 to select no tile pattern,
     * 1 or more to select the existing
     * tile pattern with this id or -1 if a new tile pattern is selected
     */
    public void setSelectedTilePatternId(int selectedTilePatternId) {
        if (selectedTilePatternId != this.selectedTilePatternId) {
            this.selectedTilePatternId = selectedTilePatternId;

            if (selectedTilePatternId != getNbTilePatterns()) {
                newTilePatternArea = null;
            }

            setChanged();
            notifyObservers();
        }
    }

    /**
     * Unselects the current tile pattern.
     * This is equivalent to call setSelectedTileId(0).
     */
    public void unselectTilePattern() {
        setSelectedTilePatternId(0);
    }

    /**
     * Starts the selection of a new tile pattern.
     * This is equivalent to call setSelectedTilePatternId(-1).
     */
    public void startSelectingNewTilePattern() {
        setSelectedTilePatternId(-1);
    }

    /**
     * Returns the selected tile pattern.
     * @return the selected tile pattern, or null if there is no selected
     * tile pattern or if doesn't exist yet
     */
    public TilePattern getSelectedTilePattern() {
        if (selectedTilePatternId > 0) {
            return getTilePattern(selectedTilePatternId);
        }
        else {
            return null;
        }
    }

    /**
     * Returns whether or not the user is selecting a new tile pattern.
     * @return true if the user is selecting a new tile pattern,
     * i.e. if getSelectedTilePatternId() == -1
     */
    public boolean isSelectingNewTilePattern() {
        return selectedTilePatternId == -1;
    }

    /**
     * Returns the rank of a tile pattern, knowing its id.
     * The rank is the position of the tile pattern if you consider all tile patterns sorted by their ids.
     * It is different from the id because all ids don't exist necessarily.
     * @param id id of the tile pattern in the tileset
     * @return rank of this tile pattern, in [0, getNbTilePatterns()[.
     */
    public int idToRank(int id) {

        // count the tiles until we find the right one
        int rank = 0;
        for (int idFound: getTilePatternIds()) {

            if (idFound == id) {
                return rank;
            }
            rank++;
        }

        throw new NoSuchElementException("There is no tile pattern at id " + id + " in the tileset.");
    }

    /**
     * Returns the id of a tile pattern knowing its rank.
     * The rank is the position of the tile pattern if you consider all tile patterns sorted by their ids.
     * It is different from the id because all ids don't exist necessarily.
     * @param rank rank of the tile pattern considered, in [0, getNbTilePatterns()[
     * @return the id of the tile pattern with this rank
     */
    public int rankToId(int rank) {

        // count rank tiles
        int i = 0;
        for (int currentId: getTilePatternIds()) {

            if (i == rank) {
                return currentId;
            }
            i++;
        }

        throw new NoSuchElementException("There is no tile pattern with rank " + rank + " in the tileset.");
    }

    /**
     * Returns the position of the tile pattern the user is creating.
     * @return position of the new tile pattern, or null if there no new tile pattern selected
     */
    public Rectangle getNewTilePatternArea() {
        return newTilePatternArea;
    }

    /**
     * Changes the position of the tile pattern the user is creating.
     * If the specified area is the same than before, nothing is done.
     * @param newTileArea position of the new tile, or null if there is currently no new tile selected
     */
    public void setNewTilePatternArea(Rectangle newTilePatternArea) {
        if (!newTilePatternArea.equals(this.newTilePatternArea)) {

            this.newTilePatternArea = newTilePatternArea;

            // determine whether or not the new tile pattern area is overlapping an existing tile pattern
            isNewTilePatternAreaOverlapping = false;
            for (TilePattern pattern: getTilePatterns()) {

                if (pattern.getPositionInTileset().intersects(newTilePatternArea)) {
                    isNewTilePatternAreaOverlapping = true;
                    break;
                }
            }

            setChanged();
            notifyObservers();
        }
    }

    /**
     * Returns whether or not the area selected by the user to make a new tile pattern
     * is overlapping an existing tile pattern.
     * @return true if the new tile pattern area is overlapping an existing tile pattern, false otherwise
     */
    public boolean isNewTilePatternAreaOverlapping() {
        return isNewTilePatternAreaOverlapping;
    }

    /**
     * Creates the tile pattern specified by the current selection area
     * and adds it to the tileset.
     * The observers are notified with the created TilePattern as parameter.
     * @param obstacle obstacle property of the created tile pattern
     * @throws TilesetException if the tile size is incorrect
     */
    public void addTilePattern(Obstacle obstacle) throws TilesetException {

        if (isSelectingNewTilePattern() && !isNewTilePatternAreaOverlapping) {
            TilePattern tilePattern = new TilePattern(newTilePatternArea, Layer.LOW, obstacle);

            maxId++;
            tilePatterns.put(maxId, tilePattern);

            setSelectedTilePatternId(maxId);

            isSaved = false;

            setChanged();
            notifyObservers(tilePattern); // indicates that a tile pattern has been created
        }
    }

    /**
     * Adds a tile pattern to the tileset.
     * @param tilePatternId id of the tile pattern
     * @param tilePattern the tile pattern to add
     */
    private void addTilePattern(int tilePatternId, TilePattern tilePattern) {
        tilePatterns.put(tilePatternId, tilePattern);
    }

    /**
     * Removes the selected tile pattern.
     * The oberservers are notified with the removed tile pattern as parameter.
     */
    public void removeTilePattern() {
        Integer id = new Integer(getSelectedTilePatternId());

        if (id > 0) {
            tilePatterns.remove(id);
            setSelectedTilePatternId(0);

            isSaved = false;

            setChanged();
            notifyObservers(id); // indicate that the tile has been removed
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
     * @param isSaved true if there has been no modifications, false otherwise
     */
    public void setSaved(boolean isSaved) {
        this.isSaved = isSaved;
    }

    /**
     * Loads the tileset from its file.
     * @throws QuestEditorException if the file could not be read
     */
    public void load() throws QuestEditorException {

        try {
            // get the tileset name in the game resource database
            Resource tilesetResource = Project.getResource(ResourceType.TILESET);
            setName(tilesetResource.getElementName(tilesetId));

            File tilesetFile = Project.getTilesetFile(tilesetId);
            LuaC.install();
            LuaTable environment = LuaValue.tableOf();

            environment.set("background_color", new BackgroundColorFunction());
            environment.set("tile_pattern", new TilePatternFunction());

            LuaFunction code = LoadState.load(new FileInputStream(tilesetFile),
                "tileset", environment);
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

        int lastId = -1;
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
            for (int id: getTilePatternIds()) {
                lastId = id;
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
                out.println("  id = " + id + ",");
                out.println("  ground = \"" + getObstacleName(tilePattern.getObstacle()) + "\",");
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

            // Also update the tileset name in the global resource list.
            Resource tilesetResource = Project.getResource(ResourceType.TILESET);
            tilesetResource.setElementName(tilesetId, name);
            Project.getResourceDatabase().save();
        }
        catch (Exception ex) {
            String message = "";
            if (lastId != -1) {
                message = "Failed to save tile '" + lastId + "': ";
            }
            message += ex.getMessage();
            throw new QuestEditorException(message);
        }
    }

    /**
     * @brief Turns a Lua obstacle name into an Obstacle enum value.
     * @param name The Lua name.
     * @return The corresponding enum value or null.
     */
    private Obstacle getObstacleByName(String name) throws QuestEditorException {

        Obstacle obstacle = obstaclesByName.get(name);
        if (obstacle == null) {
            throw new QuestEditorException("Invalid obstacle name: '" + name);
        }
        return obstacle;
    }

    /**
     * @brief Turns an obstacle enum value into a Lua obstacle nam.
     * @param obstacle The obstacle value.
     * @return The corresponding name.
     */
    private String getObstacleName(Obstacle obstacle) throws QuestEditorException {

        for (java.util.Map.Entry<String, Obstacle> keyValue: obstaclesByName.entrySet()) {

            if (keyValue.getValue() == obstacle) {
                return keyValue.getKey();
            }
        }
        throw new QuestEditorException("No name for obstacle " + obstacle);
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

            int id = -1;
            Layer defaultLayer = null;
            int width = 0;
            int height = 0;
            int[] x = { -1, -1, -1, -1 };
            int[] y = { -1, -1, -1, -1 };
            Obstacle ground = null;
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
                        id = value.checkint();
                    }
                    else if (keyString.equals("ground")) {
                        String groundName = value.checkjstring();
                        ground = getObstacleByName(groundName);
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
                System.out.println("exception");
                String message = "";
                if (id != -1) {
                    message += "Failed to load tile '" + id + "': ";
                }
                message += ex.getMessage();
                throw new LuaError(message);
            }

            if (id > maxId) {
                maxId = id;
            }
            setSaved(true);

            return LuaValue.NIL;
        }
    }
}

