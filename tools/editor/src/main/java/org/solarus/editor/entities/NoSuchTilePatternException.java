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

import org.solarus.editor.*;

/**
 * Exception thrown if a tile placed on a map references a non-existing
 * tile pattern in the tileset or a tile pattern with unexpected properties.
 */
public class NoSuchTilePatternException extends MapException {

    private String tilePatternId;

    /**
     * Constructor.
     * @param message Error message.
     * @param tilePatternId Id of the tile pattern not found or not valid.
     */
    public NoSuchTilePatternException(String message, String tilePatternId) {
        super(message);
        this.tilePatternId = tilePatternId;
    }

    /**
     * Returns the id of the tile pattern not found or not valid.
     * @return tilePatternId;
     */
    public String getTilePatternId() {
        return tilePatternId;
    }
}
