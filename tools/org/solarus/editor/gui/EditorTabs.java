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

import java.awt.Point;
import java.awt.event.MouseEvent;
import java.awt.event.MouseListener;
import javax.swing.JTabbedPane;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 * The main tabbed pane with all editors currently open. 
 */
public class EditorTabs extends JTabbedPane implements MouseListener, ChangeListener {

    public static final long serialVersionUID = 1L;

    public EditorTabs() {
        setTabLayoutPolicy(SCROLL_TAB_LAYOUT);
        addMouseListener(this);
        addChangeListener(this);
    }

    /**
     * Add an editor, unless it already exists:
     * in that case, the existing editor is made active instead.
     * @param key Key that identifies this editor. Used to determined whether
     * the same editor already exists.
     * @param editor The editor to add.
     */
    public void addEditor(String key, AbstractEditorPanel editor) {
        String title = editor.getResourceName();
        AbstractEditorPanel[] editors = getEditors();
        if (editors != null) {
            for (AbstractEditorPanel e: editors) {
                if (e.getResourceName().equals(title)) {
                    setSelectedComponent(e);
                    return;
                }
            }
        }
        add(title, editor);
        setSelectedIndex(getTabCount() - 1);
        repaint();
    }

    /**
     * Remove an editor unless the user is not okay with that.
     * @param editor The editor to remove.
     */
    public void removeEditor(AbstractEditorPanel editor) {
        if (editor.checkCurrentFileSaved()) {
            remove(editor);
            repaint();
        }
    }

    /**
     * Removes the current editor if any, unless the user is not okay with that.
     */
    public void removeCurrentEditor() {
        if (getSelectedComponent() != null) {
            removeEditor((AbstractEditorPanel) getSelectedComponent());
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
     * @return A table containing the editors currently opened
     */
    public AbstractEditorPanel[] getEditors() {
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
                removeEditor(editor);
            }
        }
    }

    @Override
    public void repaint() {
        // TODO Don't do this in repaint().
        super.repaint();
        for (int i = 0; i < getTabCount(); i++) {
            setTitleAt(i, ((AbstractEditorPanel) getComponentAt(i)).getResourceName());
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
        try {
            if (countEditors() > 0) {
                ((MapEditorPanel) getSelectedComponent()).getMapView().requestFocus();
            }
        } catch (ClassCastException cce) {
        }
    }
}
