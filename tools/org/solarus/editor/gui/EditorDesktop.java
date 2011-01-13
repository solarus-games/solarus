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
package org.solarus.editor.gui;

import java.awt.event.KeyEvent;
import java.awt.event.KeyListener;
import javax.swing.JTabbedPane;

/**
 *
 */
public class EditorDesktop extends JTabbedPane {

    public static final long serialVersionUID = 1L;

    public EditorDesktop() {
        setTabLayoutPolicy(SCROLL_TAB_LAYOUT);
    }

    /**
     * Add an editor in the tabbedpane
     * @param editor the editor to add
     */
    public void addEditor(AbstractEditorWindow editor) {
        String title = editor.getResourceName();
        AbstractEditorWindow[] editors = getEditors();
        if (editors != null) {
            for (AbstractEditorWindow e : editors) {
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
     * Remove an editor of the tabbedpane
     * @param editor the editor to remove
     */
    public void removeEditor(AbstractEditorWindow editor) {
        if (editor.checkCurrentFileSaved()) {
            remove(editor);
        }
        repaint();
    }

    public void removeCurrentEditor() {
        if (getSelectedComponent() != null) {
            removeEditor((AbstractEditorWindow) getSelectedComponent());
        }
    }

    /**
     * Returns the editors currently opened
     * @return A table containing the editors currently opened
     */
    public AbstractEditorWindow[] getEditors() {
        int nb = getTabCount();
        if (nb > 0) {
            AbstractEditorWindow[] editors = new AbstractEditorWindow[nb];
            for (int i = 0; i < nb; i++) {
                editors[i] = (AbstractEditorWindow) getComponentAt(i);
            }
            return editors;
        }
        return null;
    }

    /**
     * Count the editors currently opened
     * @return the number of editos currently opened
     */
    public int countEditors() {
        return getTabCount();
    }
}
