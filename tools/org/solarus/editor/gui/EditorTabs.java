/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
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
package org.solarus.editor.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;

/**
 * The main tab pane with all editors currently open. 
 */
public class EditorTabs extends JTabbedPane implements MouseListener, ChangeListener {

    /**
     * Creates an empty tab pane of editor.
     */
    public EditorTabs() {
        setTabLayoutPolicy(SCROLL_TAB_LAYOUT);
        addMouseListener(this);
        addChangeListener(this);
    }

    /**
     * Adds an editor.
     * No other editor with the same id must exist.
     * @param editor The editor to add.
     */
    public void addEditor(AbstractEditorPanel editor) {

        if (getEditor(editor.getId()) != null) {
            throw new IllegalArgumentException(
                    "There is already an editor with id '" + editor.getId() + "'");
        }

        add(editor.getTitle(), editor);
        setSelectedIndex(getTabCount() - 1);
        repaint();
    }

    /**
     * Returns an existing editor with the specified id if any.
     * @param id Id of the editor to search.
     * @return The existing editor with this id, or null if there is no editor
     * with this id.
     */
    public AbstractEditorPanel getEditor(String id) {

        // TODO store editors in a map.
        AbstractEditorPanel[] editors = getEditors();

        if (editors != null) {
            for (AbstractEditorPanel editor: editors) {
                if (editor.getId().equals(id)) {
                    return editor;
                }
            }
        }
        return null;
    }

    /**
     * Removes an editor.
     * @param promptSave true to let the user save the element if necessary,
     * false to close it without confirmation.
     * @param editor The editor to remove.
     */
    public void removeEditor(AbstractEditorPanel editor, boolean promptSave) {
        if (!promptSave || editor.checkCurrentFileSaved()) {
            editor.close();
            remove(editor);
            repaint();
        }
    }

    /**
     * Removes an editor if it exists.
     * @param promptSave true to let the user save the element if necessary,
     * false to close it without confirmation.
     * @param id Id of editor to remove. Nothing happens if there is no editor
     * with this id.
     */
    public void removeEditor(String editorId, boolean promptSave) {
        AbstractEditorPanel editor = getEditor(editorId);
        if (editor != null) {
            removeEditor(editor, promptSave);
        }
    }

    /**
     * Removes the current editor if any, unless the user is not okay with that.
     * @param promptSave true to let the user save the element if necessary,
     * false to close it without confirmation.
     */
    public void removeCurrentEditor(boolean promptSave) {
        if (getSelectedComponent() != null) {
            removeEditor((AbstractEditorPanel) getSelectedComponent(), promptSave);
        }
    }

    /**
     * Saves the resource element of the current editor if any.
     */
    public void saveCurrentEditor() {
        if (getSelectedComponent() != null) {
            ((AbstractEditorPanel) getSelectedComponent()).save();
        }
    }

    /**
     * Returns all editors currently open.
     * @return An array containing the editors currently open.
     */
    public AbstractEditorPanel[] getEditors() {
        // TODO store this.
        int nb = getTabCount();
        if (nb > 0) {
            AbstractEditorPanel[] editors = new AbstractEditorPanel[nb];
            for (int i = 0; i < nb; i++) {
                editors[i] = (AbstractEditorPanel) getComponentAt(i);
            }
            return editors;
        }
        return null;
    }

    /**
     * Count the editors currently opened
     * @return The number of editors currently opened.
     */
    public int countEditors() {
        return getTabCount();
    }

    /**
     * Allow to close the editor with a middle click on his tab.
     * @param e The event.
     */
    public void mouseClicked(MouseEvent e) {
        if (e.getButton() == MouseEvent.BUTTON2 && countEditors() > 0) {
            Point clic = e.getPoint();
            int idx = indexAtLocation(clic.x, clic.y);
            if (idx != -1) {
                // Middle click on a tab was clicked: close it.
                AbstractEditorPanel editor = (AbstractEditorPanel) getComponentAt(idx);
                removeEditor(editor, true);
            }
        }
    }

    @Override
    public void repaint() {
        // TODO Don't do this in repaint().
        super.repaint();
        for (int i = 0; i < getTabCount(); i++) {
            setTitleAt(i, ((AbstractEditorPanel) getComponentAt(i)).getTitle());
        }
    }

    public void mousePressed(MouseEvent e) {
    }

    public void mouseReleased(MouseEvent e) {
    }

    public void mouseEntered(MouseEvent e) {
    }

    public void mouseExited(MouseEvent e) {
    }

    public void stateChanged(ChangeEvent e) {
        if (countEditors() > 0) {
            Component editor = getSelectedComponent();
            if (editor instanceof MapEditorPanel) {
                ((MapEditorPanel) editor).getMapView().requestFocus();
            }
        }
    }
}
