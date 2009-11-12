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

/**
 * Changing the name of a map.
 */
public class ActionRenameMap extends MapEditorAction {

    private String nameBefore;
    private String nameAfter;

    /**
     * Creates an action to change a map name.
     * @param map the map
     * @param name new name of the map
     */
    public ActionRenameMap(Map map, String name) {
	super(map);
	this.nameBefore = map.getName();
	this.nameAfter = name;
    }

    /**
     * Executes the action.
     */
    public void execute() throws MapException {

	map.setName(nameAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws MapException {

	map.setName(nameBefore);
    }

}
