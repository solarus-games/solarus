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

import java.awt.Dimension;
import org.solarus.editor.*;

/**
 * Changing the size of the map.
 */
public class ActionChangeMapSize extends MapEditorAction {

    private Dimension sizeBefore;
    private Dimension sizeAfter;

    /**
     * Constructor.
     * @param map the map
     * @param size the new size of the map, in pixels
     */
    public ActionChangeMapSize(Map map, Dimension size) {
	super(map);
	this.sizeAfter = size;
	this.sizeBefore = map.getSize();
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {
	map.setSize(sizeAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {
	map.setSize(sizeBefore);
    }

}
