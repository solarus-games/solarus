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

import java.util.LinkedList;
import zsdx.*;
import zsdx.entities.*;

/**
 * Changing the layer of some entities.
 */
public class ActionChangeLayer extends MapEditorAction {

    private LinkedList<MapEntity> entities;
    private int[] layersBefore; // the layers before the action can be different
    private int layerAfter;

    /**
     * Constructor.
     * @param map the map
     * @param entities the entities for which the layer will be changed
     * @param layer the layer to set
     */
    public ActionChangeLayer(Map map, LinkedList<MapEntity> entities, int layer) {
	super(map);
	this.entities = new LinkedList<MapEntity>(entities);
	this.layerAfter = layer;

	this.layersBefore = new int[entities.size()];
	int i = 0;
	for (MapEntity entity: entities) {
	    this.layersBefore[i] = entity.getLayer();
	    i++;
	}
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	for (MapEntity entity: entities) {
	    map.setEntityLayer(entity, layerAfter);
	}
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	int i = 0;
	for (MapEntity entity: entities) {
	    map.setEntityLayer(entity, layersBefore[i]);
	    i++;
	}
    }

}
