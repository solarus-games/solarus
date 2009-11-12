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
package zsdx;

/**
 * An action of the user on the editor.
 * The actions are saved in a MapEditorHistory object, so that the user can
 * undo an redo them.
 */
public abstract class MapEditorAction {

    /**
     * The map.
     */
    protected final Map map;

    /**
     * Creates an action without specifying the map.
     */
    protected MapEditorAction() {
	this(null);
    }

    /**
     * Creates an action.
     * @param map the map
     */
    public MapEditorAction(Map map) {
	this.map = map;
    }

    /**
     * Executes the action.
     */
    public abstract void execute() throws ZSDXException;

    /**
     * Undoes the action.
     */
    public abstract void undo() throws ZSDXException;

}
