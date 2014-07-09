/*
 * Copyright (C) 2006-2014 Christopho, Solarus - http://www.solarus-games.org
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

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import javax.swing.plaf.basic.BasicButtonUI;
import java.util.ArrayList;
import org.solarus.editor.*;

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

        add(editor);

        int index = getTabCount() - 1;
        JComponent tabComponent = new TabComponent(editor);
        setTabComponentAt(index, tabComponent);

        setSelectedIndex(index);
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
     * @param editorId Id of editor to remove. Nothing happens if there is no editor
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
        // TODO return an ArrayList instead.
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
     * Returns all editors of the specified resource type currently open.
     * @param resourceType A type of resource.
     * @return A list of the editors of this resource type currently open.
     */
    public ArrayList<AbstractEditorPanel> getEditors(ResourceType resourceType) {

        ArrayList<AbstractEditorPanel> editors = new ArrayList<AbstractEditorPanel>();
        for (int i = 0; i < getTabCount(); i++) {
            AbstractEditorPanel editor = (AbstractEditorPanel) getComponentAt(i);
            if (editor.getResourceType() == resourceType) {
                editors.add(editor);
            }
        }
        return editors;
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
    @Override
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

    /**
     * Refreshs the title of a specified editor.
     * @param id The index of the editor
     */
    public void refreshEditorTitle(String id) {

        AbstractEditorPanel editor = getEditor(id);

        if (editor != null) {
            TabComponent tabComponent = (TabComponent) getTabComponentAt(
                    indexOfComponent(editor));
            tabComponent.setTitle(editor.getTitle());
        }
    }

    @Override
    public void mousePressed(MouseEvent e) {
    }

    @Override
    public void mouseReleased(MouseEvent e) {
    }

    @Override
    public void mouseEntered(MouseEvent e) {
    }

    @Override
    public void mouseExited(MouseEvent e) {
    }

    @Override
    public void stateChanged(ChangeEvent e) {
        if (countEditors() > 0) {
            Component editor = getSelectedComponent();
            if (editor instanceof MapEditorPanel) {
                ((MapEditorPanel) editor).getMapView().requestFocus();
            }
        }
    }

    /**
     * Component, with a title and a close button, used in tabs.
     */
    private class TabComponent extends JPanel {

        /**
         * The title of this tab component.
         */
        final private JLabel title;

        /**
         * Constructor.
         * @param title the title of the tab component
         * @param editor the editor of the tab (used to close the tab)
         */
        public TabComponent(final AbstractEditorPanel editor) {

            setLayout(new BorderLayout());
            setBorder(BorderFactory.createEmptyBorder(3, 0, 0, 0));
            setOpaque(false);

            // title
            title = new JLabel(editor.getTitle());
            title.setBorder(BorderFactory.createEmptyBorder(0, 0, 0, 5));
            add(title, BorderLayout.WEST);

            // close
            final JButton close = new JButton(
                    Project.getEditorImageIconOrEmpty("icon_cross.png"));
            close.setPreferredSize(new Dimension(16, 16));
            close.setUI(new BasicButtonUI());
            close.setBorderPainted(false);
            close.setOpaque(false);
            close.addActionListener(new ActionListener() {

                @Override
                public void actionPerformed(ActionEvent ae) {
                    removeEditor(editor, true);
                }
            });
            close.addMouseListener(new MouseListener() {

                @Override
                public void mouseClicked(MouseEvent me) {}

                @Override
                public void mousePressed(MouseEvent me) {}

                @Override
                public void mouseReleased(MouseEvent me) {}

                @Override
                public void mouseEntered(MouseEvent me) {

                    close.setBorderPainted(true);
                }

                @Override
                public void mouseExited(MouseEvent me) {

                    close.setBorderPainted(false);
                }
            });
            add(close, BorderLayout.EAST);
        }

        /**
         * Set the title of this tab component.
         * @param title the new title
         */
        public void setTitle(String title) {

            this.title.setText(title);
        }
    }
}
