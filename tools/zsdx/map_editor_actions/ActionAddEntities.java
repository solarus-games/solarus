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
 * Adding some entities on the map.
 */
public class ActionAddEntities extends MapEditorAction {

    private LinkedList<MapEntity> entitiesToAdd;

    /**
     * Creates an action to add an entity.
     * @param map the map
     * @param entity the entity to add
     */
    public ActionAddEntities(Map map, MapEntity entityToAdd) {
	super(map);
	this.entitiesToAdd = new LinkedList<MapEntity>();
	this.entitiesToAdd.add(entityToAdd);
    }

    /**
     * Creates an action to add several entities.
     * @param map the map
     * @param entitiesToAdd the entities to add
     */
    public ActionAddEntities(Map map, List<MapEntity> entitiesToAdd) {
	super(map);
	this.entitiesToAdd = new LinkedList<MapEntity>(entitiesToAdd);
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	for (MapEntity entity: entitiesToAdd) {
	    entity.ensureNameIsUnique();
	    map.addEntity(entity);
	}
	map.getEntitySelection().unselectAll();
	map.getEntitySelection().select(entitiesToAdd);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	for (MapEntity entity: entitiesToAdd) {
	    map.removeEntity(entity);
	}
    }

}
