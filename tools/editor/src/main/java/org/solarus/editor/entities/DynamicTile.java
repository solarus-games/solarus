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
package org.solarus.editor.entities;

import org.solarus.editor.*;

/**
 * A dynamic tile is a tile that can be enabled or disabled dynamically
 * on the map during the game.
 */
public class DynamicTile extends Tile {

    /**
     * Description of the default image representing this kind of entity.
     */
    public static final EntityImageDescription[] generalImageDescriptions = null;

    /**
     * Creates a new dynamic tile tile on the map.
     * @param map the map
     * @throws MapException if the tile is not valid
     */
    public DynamicTile(Map map) throws MapException {
        super(map);
    }

    /**
     * Creates a new dynamic tile from a regular tile.
     * @param tile A non dynamic tile to copy.
     * @throws MapException if the tile is not valid.
     */
    public DynamicTile(Tile regularTile) throws MapException {
        this(regularTile.getMap());
        setIntegerProperty("pattern", regularTile.getTilePatternId());
        setLayer(regularTile.getLayer());
        setPositionInMap(regularTile.getPositionInMap());
        setName("tile");
        updateImageDescription();
    }

    /**
     * Returns whether this kind of entity is allowed to have a name.
     * @return true.
     */
    public boolean canHaveName() {
        // As DynamicTile inherits Tile, we have to redefine this method.
        return true;
    }

    /**
     * Declares all properties specific to the current entity type and sets
     * their initial values.
     */
    public void createProperties() throws MapException {
        super.createProperties();
        createBooleanProperty("enabled_at_start", false, true);
    }

    /**
     * Creates a static tile with the same properties than this dynamic tile.
     * @return the static tile corresponding to this dynamic tile
     * @throws QuestEditorException if the static tile could not be created
     */
    public final Tile createStaticTile() throws QuestEditorException {
        return new Tile(this);
    }
}

