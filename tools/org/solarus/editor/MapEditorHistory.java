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
package org.solarus.editor;

import java.util.*;

/**
 * The map editor history is a stack of the last actions done by the user.
 * The user can undo or redo the actions.
 */
public class MapEditorHistory extends Observable {

    /**
     * The stack of actions.
     */
    private Stack<MapEditorAction> actions;

    /**
     * Index of the next action.
     */
    private int nextActionIndex;

    /**
     * Value of nextActionIndex when the map was saved (-1: none).
     */
    private int lastSave;

    /**
     * Maximum number of actions the history can contain.
     */
    private static final int maxSize = 50;
    
    /**
     * Constructor.
     */
    public MapEditorHistory() {
	
	actions = new Stack<MapEditorAction>();
	nextActionIndex = 0;
    }

    /**
     * Executes an action and adds it into the history.
     * @param action the action to execute
     * @throws MapException if an error occurs while undoing the action
     */
    public void doAction(MapEditorAction action) throws ZSDXException {

	action.execute();
	actions.setSize(nextActionIndex + 1);
	actions.setElementAt(action, nextActionIndex);
	nextActionIndex++;

	if (lastSave >= nextActionIndex) {
	    lastSave = -1;
	}

	while (actions.size() > maxSize) {
	    actions.removeElementAt(0);
	    nextActionIndex--;
	    lastSave--;
	}

	setChanged();
	notifyObservers();
    }

    /**
     * Undoes the last action.
     * @throws MapException if there is no action to undo or an error
     * occurs while undoing the action
     */
    public void undo() throws ZSDXException {
	
	if (!canUndo()) {
	    throw new MapException("No further undo information");
	}
	
	MapEditorAction action = actions.get(nextActionIndex - 1);
	action.undo();

	nextActionIndex--;

	setChanged();
	notifyObservers();
    }

    /**
     * Redoes the last action undone.
     * @throws MapException if there is no action to redo or an error
     * occurs while redoing the action
     */
    public void redo() throws ZSDXException {

	if (!canRedo()) {
	    throw new MapException("No further redo information");
	}

	MapEditorAction action = actions.get(nextActionIndex);
	action.execute();

	nextActionIndex++;

	setChanged();
	notifyObservers();
    }

    /**
     * Returns whether or not there is some actions back in the history.
     * @return true if the user can undo an action
     */
    public boolean canUndo() {
	return nextActionIndex > 0;
    }

    /**
     * Returns whether or not there is some actions in the future.
     * @return true if the user can redo an action
     */
    public boolean canRedo() {
	return nextActionIndex < actions.size();
    }

    /**
     * Returns whether or not the map has changed since the last save.
     * @return true if there has been no modifications, false otherwise
     */
    public boolean isSaved() {
	return lastSave == nextActionIndex;
    }

    /**
     * Indicates that the map has just been saved.
     */
    public void setSaved() {
	
	if (!isSaved()) {
	    lastSave = nextActionIndex;
	    setChanged();
	    notifyObservers();
	}
    }
}
