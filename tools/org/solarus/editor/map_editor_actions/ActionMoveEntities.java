/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
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
package org.solarus.editor.map_editor_actions;

import java.util.List;
import java.util.LinkedList;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;

/**
 * Moving some tiles on the map.
 */
public class ActionMoveEntities extends MapEditorAction {

    private List<MapEntity> entitiesToMove;
    private int dx;
    private int dy;

    /**
     * Creates an action to move one entity.
     * @param map the map
     * @param entityToMove the entity to move
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     */
    public ActionMoveEntities(Map map, MapEntity entityToMove, int dx, int dy) {
	super(map);
	this.entitiesToMove = new LinkedList<MapEntity>();
	this.entitiesToMove.add(entityToMove);
	this.dx = dx;
	this.dy = dy;
    }

    /**
     * Creates an action to move several entities.
     * @param map the map
     * @param entitiesToMove the entities to move
     * @param dx number of pixels to move on x
     * @param dy number of pixels to move on y
     */
    public ActionMoveEntities(Map map, List<MapEntity> entitiesToMove, int dx, int dy) {
	super(map);
	this.entitiesToMove = new LinkedList<MapEntity>(entitiesToMove);
	this.dx = dx;
	this.dy = dy;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	map.moveEntities(entitiesToMove, dx, dy);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	map.moveEntities(entitiesToMove, -dx, -dy);
    }

}
