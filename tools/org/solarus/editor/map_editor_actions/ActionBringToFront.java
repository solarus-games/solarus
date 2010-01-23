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
 * Bringing some entities to front.
 */
public class ActionBringToFront extends MapEditorAction {

    private LinkedList<MapEntity> entities;
    private MapEntities[] allEntitiesBefore;
    
    /**
     * Constructor.
     * @param map the map
     * @param entities the entities to bring to front
     */
    public ActionBringToFront(Map map, LinkedList<MapEntity> entities) {
	super(map);
	
	this.entities = new LinkedList<MapEntity>(entities);
	this.allEntitiesBefore = new MapEntities[Layer.values().length];

	// copy allEntities
	MapEntities[] allEntities = map.getAllEntities();
	for (int i = 0; i < allEntitiesBefore.length; i++ ) {
	    allEntitiesBefore[i] = new MapEntities(allEntities[i]);
	}
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.bringToFront(entities);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setAllEntities(allEntitiesBefore);
    }

}
