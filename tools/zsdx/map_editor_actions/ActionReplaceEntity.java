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

import zsdx.*;
import zsdx.entities.*;

/**
 * Replacing an entity by a new one on the map.
 */
public class ActionReplaceEntity extends MapEditorAction {

    private MapEntity oldEntity;
    private MapEntity newEntity;

    /**
     * Creates an action to add an entity.
     * @param map the map
     * @param oldEntity the entity to replace
     * @param newEntity the new entity
     */
    public ActionReplaceEntity(Map map, MapEntity oldEntity, MapEntity newEntity) {
	super(map);
	this.oldEntity = oldEntity;
	this.newEntity = newEntity;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	newEntity.ensureNameIsUnique();
	map.removeEntity(oldEntity);
	map.addEntity(newEntity);
	map.getEntitySelection().unselectAll();
	map.getEntitySelection().select(newEntity);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	map.removeEntity(newEntity);
	map.addEntity(oldEntity);
	map.getEntitySelection().unselectAll();
	map.getEntitySelection().select(oldEntity);
    }

}
