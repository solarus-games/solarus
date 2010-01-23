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

import java.awt.Rectangle;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;

/**
 * Resizing a map entity.
 */
public class ActionResizeEntity extends MapEditorAction {

    private MapEntity entity;
    private Rectangle positionBefore;
    private Rectangle positionAfter;
    
    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to resize
     * @param position the new entity size (this object is not modified by this class)
     */
    public ActionResizeEntity(Map map, MapEntity entity, Rectangle position) {
	super(map);
	
	this.entity = entity;
	this.positionBefore = new Rectangle(entity.getPositionInMap());
	this.positionAfter = new Rectangle(position);
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.setEntityPosition(entity, positionAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setEntityPosition(entity, positionBefore);
    }

}
