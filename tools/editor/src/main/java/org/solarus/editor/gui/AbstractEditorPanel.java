/*
 * Copyright (C) 2006-2013 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * Solarus Quest Editor is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */

package org.solarus.editor.gui;

import javax.swing.JPanel;

/**
 * Main component of an editor. Every editor (map, tileset...)
 * extends this class.
 */
public abstract class AbstractEditorPanel extends JPanel {

    /**
     * A name identifying this editor.
     * This id is used to determine whether another editor for the same
     * element exists.
     */
    private final String id;

    /**
     * The main window of the quest editor.
     */
    private EditorWindow mainWindow;

    /**
     * Creates an editor panel.
     * @param mainWindow The main window of the quest editor.
     * @param id A name identifying the file being edited.
     */
    protected AbstractEditorPanel(EditorWindow mainWindow, String id) {
        this.mainWindow = mainWindow;
        this.id = id;
    }

    /**
     * This function is called when the user wants to close the editor.
     * If the file is not saved, you should propose to save it.
     * @return false to cancel the closing operation.
     */
    public abstract boolean checkCurrentFileSaved();

    /**
     * Returns a user-friendly title for this editor.
     * It is used in the tab label.
     */
    public abstract String getTitle();

    /**
     * Saves the current resource.
     */
    public abstract void save();

    /**
     * Closes this editor without confirmation.
     * You should clean everything in this method.
     */
    public abstract void close();

    /**
     * Returns the main window of the quest editor.
     * @return The main window.
     */
    public final EditorWindow getMainWindow() {
        return mainWindow;
    }

    /**
     * Returns an id that identifies the file open in this editor.
     * You can use it to determine whether this file is already open in
     * another editor.
     */
    public final String getId() {
        return id;
    }
}

