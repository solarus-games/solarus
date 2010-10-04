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

import java.awt.BorderLayout;
import java.util.HashMap;
import javax.swing.BorderFactory;
import javax.swing.ButtonGroup;
import javax.swing.JDesktopPane;
import javax.swing.JInternalFrame;
import javax.swing.JPanel;
import javax.swing.JToggleButton;
import javax.swing.JToolBar;
import javax.swing.event.ChangeEvent;
import javax.swing.event.ChangeListener;

/**
 *
 */
public class EditorDesktop extends JPanel {

    public static final long serialVersionUID = 1L;
    /**
     * The JDesktopPane use to contained the editors
     */
    private JDesktopPane desktopPane;
    /**
     * The tool bar which allow access to the editors
     */
    private JToolBar toolBar;
    /**
     * Button group of the tool bar
     */
    private ButtonGroup buttonGroup;
    /**
     * Map
     */
    private HashMap<AbstractEditorWindow, CustomToggleButton> editorToButton;

    public EditorDesktop() {
        super(new BorderLayout());
        editorToButton = new HashMap<AbstractEditorWindow, CustomToggleButton>();
        buttonGroup = new ButtonGroup();
        desktopPane = new JDesktopPane();
        toolBar = new JToolBar(JToolBar.HORIZONTAL);
        //toolBar.add(new JLabel("Editeurs ouverts"));
        toolBar.setBorder(BorderFactory.createTitledBorder("Editeurs ouverts"));
        toolBar.setFloatable(false);
        add(desktopPane, BorderLayout.CENTER);
        add(toolBar, BorderLayout.SOUTH);
    }

    public void addEditor(AbstractEditorWindow editor) {
        final CustomToggleButton ctb = new CustomToggleButton(editor);
        editor.setVisible(true);
        buttonGroup.add(ctb);
        desktopPane.add(editor);
        toolBar.add(ctb);
        ctb.setSelected(true);
        editorToButton.put(editor, ctb);
        repaint();
    }

    public void removeEditor(AbstractEditorWindow editor) {
        CustomToggleButton ctb = editorToButton.get(editor);
        editor.dispose();
        toolBar.remove(ctb);
        buttonGroup.remove(ctb);
        repaint();
//        try {
//        ((CustomToggleButton)toolBar.getComponent(0)).setSelected(true);
//        }
//        catch(Exception e) {// to prevent index problems
//
//        }
    }

    public AbstractEditorWindow[] getEditors() {
        JInternalFrame[] frames = desktopPane.getAllFrames();
        AbstractEditorWindow[] editors = new AbstractEditorWindow[frames.length];
        for(int i = 0; i < frames.length; i++) {
            editors[i] = (AbstractEditorWindow) frames[i];
        }
        return editors ;
    }

    public int countEditors() {
        //System.out.println("Comptage editeurs : "+desktopPane.getComponentCount());
        return desktopPane.getComponentCount();
    }


    class CustomToggleButton extends JToggleButton implements ChangeListener {

        public static final long serialVersionUID = 1L;
        private AbstractEditorWindow editor;

        public CustomToggleButton(AbstractEditorWindow editor) {
            super(editor.getTitle());
            this.editor = editor;
            addChangeListener(this);
        }

        public void stateChanged(ChangeEvent e) {
            this.editor.setVisible(isSelected());
            try {
                this.editor.setMaximum(true);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }
}
