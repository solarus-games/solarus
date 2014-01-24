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

import java.awt.*;
import java.util.*;
import org.solarus.editor.*;
import org.solarus.editor.Map;

/**
 * Represents a tile for the map editor, i.e. a tile placed on the map.
 * A Tile is composed by a TilePattern (as created in the tileset editor)
 * and how the tile is placed on the map: its position, its layer and
 * how the pattern is repeated or not.
 */
public class Tile extends MapEntity {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = null;

    /**
     * Creates a new tile on the map.
     * @param map the map
     * @throws MapException if the tile is not valid
     */
    public Tile(Map map) throws MapException {
        super(map, 0, 0);
        setLayer(null);
    }

    /**
     * Creates a new tile from a dynamic tile.
     * @param dynamicTile A dynamic tile to copy.
     * @throws MapException if the tile is not valid.
     */
    public Tile(DynamicTile dynamicTile) throws MapException {
        this(dynamicTile.getMap());
        setIntegerProperty("pattern", dynamicTile.getTilePatternId());
        setLayer(dynamicTile.getLayer());
        setPositionInMap(dynamicTile.getPositionInMap());
        updateImageDescription();
    }

    /**
     * Notifies this entity that a property specific to its type has just changed.
     * Does nothing by default.
     * @param name Name of the property that has changed.
     * @param value The new value.
     */
    protected void notifyPropertyChanged(String name, String value) throws MapException {

        if (name.equals("pattern")) {
            if (getTileset() == null) {
                return;
            }

            try {
                TilePattern tilePattern = getTilePattern(); // get the tile pattern from the tileset

                if (tilePattern != null) {
                    if (getWidth() == 0) {
                        setSize(tilePattern.getWidth(), tilePattern.getHeight());
                    }

                    if (getLayer() == null) {
                        setLayer(tilePattern.getDefaultLayer());
                    }
                }
            }
            catch (NoSuchElementException ex) {
                throw new NoSuchTilePatternException(ex.getMessage());
            }
        }
    }

    /**
     * Notifies this entity that the tileset has changed.
     * The tile pattern from the new tileset
     * (i.e. the tile pattern with the same id)
     * must have exactly the same properties,
     * otherwise a NoSuchTilePatternException is thrown.
     * @param oldTileset The previous tileset or null.
     * @param newTileset The new tileset.
     * @throws NoSuchTilePatternException If the new tileset could not be applied
     * because the tile pattern doesn't exist in this tileset or is different.
     */
    public void notifyTilesetChanged(Tileset oldTileset, Tileset newTileset)
        throws NoSuchTilePatternException {

        if (newTileset == oldTileset) {
            return;
        }

        int tilePatternId = getTilePatternId();
        try {
            TilePattern newTilePattern = newTileset.getTilePattern(tilePatternId);

            // if a tileset was already defined, check that the
            // tile has the same properties
            if (oldTileset != null) {

                TilePattern oldTilePattern = oldTileset.getTilePattern(tilePatternId);

                if (!newTilePattern.equals(oldTilePattern)) {
                    throw new NoSuchTilePatternException("The tile pattern #" + tilePatternId + " is different in this tileset.");
                }

                // update the size on the map if the size in the tileset has changed
                try {
                    setSize(newTilePattern.getWidth() * getRepeatX(),
                            newTilePattern.getHeight() * getRepeatY());
                }
                catch (MapException ex) {
                    // Should not happen since we are precisely fixing the size.
                    ex.printStackTrace();
                }
            }
        }
        catch (NoSuchElementException e) {
            throw new NoSuchTilePatternException("Unable to apply the tileset because the tile pattern #" + tilePatternId + " doesn't exist in this tileset.");
        }
    }

    /**
     * Returns the id of the tile pattern in the tileset.
     * @return The id of the tile pattern in the tileset or null.
     */
    public Integer getTilePatternId() {
        return getIntegerProperty("pattern");
    }

    /**
     * Returns the tile pattern in the tileset.
     * @return the tile pattern in the tileset.
     */
    public TilePattern getTilePattern() {

        if (getTileset() == null || getTilePatternId() == null) {
            return null;
        }

        return getTileset().getTilePattern(getTilePatternId());
    }

    /**
     * Returns whether the entity sets a specific layer when it is created
     * on a map.
     * @return true
     */
    public boolean hasInitialLayer() {
        return true;
    }

    /**
     * Returns whether this kind of entity is allowed to have a name.
     * @return false.
     */
    public boolean canHaveName() {
        // Tiles are not allowed to have a name because they are optimized away
        // at runtime during the game.
        return false;
    }

    /**
     * Returns whether or not the entity is resizable.
     * A tile is resizable (i.e. its pattern can be replicated
     * horizontally or vertically).
     * @return true
     */
    public boolean isResizable() {
        return true;
    }

    /**
     * Returns the minimum size of the entity (for a resizable entity).
     * When the entity is resized, its new size must be a multiple of this minimum size.
     * @return the minimum size of the entity
     */
    public Dimension getUnitarySize() {

        if (getTileset() == null) { // special case when the tileset is not initialized yet
            return super.getUnitarySize();
        }

        TilePattern tilePattern = getTilePattern();
        return tilePattern.getSize();
    }

    /**
     * Returns the number of times the pattern is repeated on x.
     * @return the number of times the pattern is repeated on x
     */
    public int getRepeatX() {
        return getWidth() / getUnitarySize().width;
    }

    /**
     * Returns the number of times the pattern is repeated on y.
     * @return the number of times the pattern is repeated on y
     */
    public int getRepeatY() {
        return getHeight() / getUnitarySize().height;
    }

    /**
     * Draws the tile.
     * @param g graphic context
     * @param zoom zoom of the image (for example, 1: unchanged, 2: zoom of 200%)
     * @param showTransparency true to make transparent pixels,
     * false to replace them by a background color
     */
    public void paint(Graphics g, double zoom, boolean showTransparency) {

        TilePattern tilePattern = getTilePattern();

        int x = getX();
        int y = getY();
        int repeatX = getRepeatX();
        int repeatY = getRepeatY();
        int width = tilePattern.getWidth();
        int height = tilePattern.getHeight();

        for (int i = 0; i < repeatY; i++) {
            for (int j = 0; j < repeatX; j++) {
                tilePattern.paint(g, getTileset(), x, y, zoom, showTransparency);
                x += width;
            }
            y += height;
            x = getX();
        }
    }

    /**
     * Sets the default values of all properties specific to the current entity type.
     */
    public void createProperties() throws MapException {
        createIntegerProperty("pattern", false, null);
    }

    /**
     * Creates a dynamic tile with the same properties than this static tile.
     * @return the dynamic tile corresponding to this static tile
     * @throws QuestEditorException if the dynamic tile could not be created
     */
    public final DynamicTile createDynamicTile() throws QuestEditorException {
        return new DynamicTile(this);
    }
}

