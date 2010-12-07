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

import javax.swing.ButtonGroup;
import javax.swing.JTabbedPane;
import javax.swing.JToolBar;

/**
 *
 */
public class EditorDesktop extends JTabbedPane {

    public static final long serialVersionUID = 1L;
    /**
     * The JDesktopPane use to contained the editors
     */
    private JTabbedPane desktopPane;
    /**
     * The tool bar which allow access to the editors
     */
    private JToolBar toolBar;
    /**
     * Button group of the tool bar
     */
    private ButtonGroup buttonGroup;

    public EditorDesktop() {
        setTabLayoutPolicy(SCROLL_TAB_LAYOUT);
//        addChangeListener(this);
    }

    public void addEditor(AbstractEditorWindow editor) {
        add(editor.getResourceName(), editor);
        setSelectedIndex(getTabCount()-1);
        repaint();
    }

    public void removeEditor(AbstractEditorWindow editor) {
        remove(editor);
        repaint();
    }

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

    public int countEditors() {
        //System.out.println("Comptage editeurs : "+desktopPane.getComponentCount());
        return getTabCount();
    }

//    }
}
