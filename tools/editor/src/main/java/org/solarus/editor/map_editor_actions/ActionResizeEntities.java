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
package org.solarus.editor.map_editor_actions;

import java.awt.Rectangle;
import java.util.HashMap;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;

/**
 * Resizing map entities.
 */
public class ActionResizeEntities extends MapEditorAction {

    private HashMap<MapEntity, Rectangle> positionsBefore;
    private HashMap<MapEntity, Rectangle> positionsAfter;

    /**
     * Constructor.
     * @param map The map.
     * @param positions The new positions of entities (object will be copied).
     */
    public ActionResizeEntities(Map map, HashMap<MapEntity, Rectangle> positions) {
        super(map);

        this.positionsBefore = new HashMap<MapEntity, Rectangle>();
        this.positionsAfter = new HashMap<MapEntity, Rectangle>();

        for (java.util.Map.Entry<MapEntity, Rectangle> entry: positions.entrySet()) {
            MapEntity entity = entry.getKey();
            positionsBefore.put(entity, new Rectangle(entity.getPositionInMap()));
            positionsAfter.put(entity, new Rectangle(positions.get(entity)));
        }
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

        for (java.util.Map.Entry<MapEntity, Rectangle> entry: positionsAfter.entrySet()) {
            map.setEntityPosition(entry.getKey(), entry.getValue());
        }
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

        for (java.util.Map.Entry<MapEntity, Rectangle> entry: positionsBefore.entrySet()) {
            map.setEntityPosition(entry.getKey(), entry.getValue());
        }
    }

}
