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
package zsdx.map_editor_actions;

import java.util.List;
import java.util.LinkedList;
import zsdx.*;
import zsdx.entities.*;

/**
 * Removing some entities from the map.
 */
public class ActionRemoveEntities extends MapEditorAction {

    private List<MapEntity> entitiesToRemove;

    /**
     * Creates an action to remove one entity.
     * @param map the map
     * @param entityToRemove the entity to remove (this object will be copied)
     */
    public ActionRemoveEntities(Map map, MapEntity entityToRemove) {
	super(map);
	this.entitiesToRemove = new LinkedList<MapEntity>();
	this.entitiesToRemove.add(entityToRemove);
    }

    /**
     * Creates an action to remove several entities.
     * @param map the map
     * @param entitiesToRemove the entities to remove
     */
    public ActionRemoveEntities(Map map, List<MapEntity> entitiesToRemove) {
	super(map);
	this.entitiesToRemove = map.getSortedEntities(entitiesToRemove);
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	for (MapEntity entity: entitiesToRemove) {
	    map.removeEntity(entity);
	}
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	for (MapEntity entity: entitiesToRemove) {
	    map.addEntity(entity);
	}
	map.getEntitySelection().unselectAll();
	map.getEntitySelection().select(entitiesToRemove);
    }
}
