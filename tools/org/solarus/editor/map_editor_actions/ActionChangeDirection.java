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

import java.util.LinkedList;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;

/**
 * Changing the direction of some entities.
 */
public class ActionChangeDirection extends MapEditorAction {

    private LinkedList<MapEntity> entities;
    private int[] directionsBefore; // the directions before the action can be different
    private int directionAfter;

    /**
     * Constructor.
     * @param map the map
     * @param entities the entities for which the direction will be changed
     * @param direction the direction to set
     */
    public ActionChangeDirection(Map map, LinkedList<MapEntity> entities, int direction) {
	super(map);
	this.entities = new LinkedList<MapEntity>(entities);
	this.directionAfter = direction;

	this.directionsBefore = new int[entities.size()];
	int i = 0;
	for (MapEntity entity: entities) {
	    this.directionsBefore[i] = entity.getDirection();
	    i++;
	}
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	for (MapEntity entity: entities) {
	    map.setEntityDirection(entity, directionAfter);
	}
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	int i = 0;
	for (MapEntity entity: entities) {
	    map.setEntityDirection(entity, directionsBefore[i]);
	    i++;
	}
    }

}
