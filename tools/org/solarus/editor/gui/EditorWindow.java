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
import java.awt.event.ActionEvent;
import java.awt.event.ActionListener;
import java.awt.event.KeyEvent;
import java.awt.event.WindowAdapter;
import java.awt.event.WindowEvent;
import javax.swing.JFrame;
import javax.swing.JMenu;
import javax.swing.JMenuBar;
import javax.swing.JMenuItem;
import javax.swing.KeyStroke;
import org.solarus.editor.Project;
import org.solarus.editor.ProjectObserver;
import org.solarus.editor.ZSDXException;

/**
 * Main window of the editor.
 */
public class EditorWindow extends JFrame implements ProjectObserver {

   
    private EditorDesktop desktop;

    private JMenu menuMap;
    /**
     * The quest to load in the editor, for use in the sub-editors
     */
    private String quest;

      /**
     * Creates a new window.
     * @param quest name of a quest to load, or null to show a dialog to select the quest
     */
    public EditorWindow(String quest) {
        super("Solarus - Editor");
        this.quest = quest;

        Project.addProjectObserver(this);
        // set a nice look and feel
        GuiTools.setLookAndFeel();

        desktop = new EditorDesktop();
        getContentPane().add(desktop, BorderLayout.CENTER);

        // add a window listener to confirm when the user closes the window
        setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
        addWindowListener(new WindowAdapter() {
            public void windowClosing(WindowEvent e) {
                if (desktop.countEditors() > 0) {
                    for (AbstractEditorWindow editor : desktop.getEditors()) {
                        if (editor.checkCurrentFileSaved()) {
                            System.exit(0);
                        }
                    }
                }
            }
        });

        // create the menu bar
        createMenuBar();

        if (quest == null) {
            new ActionListenerLoadProject().actionPerformed(null);
        } else {
            try {
                Project.createExisting("../quests/zsdx");
            } catch (ZSDXException ex) {
                new ActionListenerLoadProject().actionPerformed(null);
            }
        }
    }

    public void createMenuBar() {
        JMenuBar menuBar = new JMenuBar();

        JMenu menu;
        JMenuItem item;

        // menu Project
        menu = new JMenu("Project");
        menu.setMnemonic(KeyEvent.VK_P);

        item = new JMenuItem("New project...");
        item.setMnemonic(KeyEvent.VK_N);
        item.getAccessibleContext().setAccessibleDescription("Create a new ZSDX project");
        item.addActionListener(new ActionListenerNewProject());
        menu.add(item);

        item = new JMenuItem("Load project...");
        item.setMnemonic(KeyEvent.VK_O);
        item.getAccessibleContext().setAccessibleDescription("Open an existing ZSDX project");
        item.addActionListener(new ActionListenerLoadProject());
        menu.add(item);

        menu.addSeparator();

        item = new JMenuItem("Quit");
        item.setMnemonic(KeyEvent.VK_Q);
        item.getAccessibleContext().setAccessibleDescription("Teletransporter the tileset editor");
        item.addActionListener(new ActionListener() {

            public void actionPerformed(ActionEvent ev) {
                boolean noChange = true;
                if (desktop.countEditors() > 0) {
                    for (AbstractEditorWindow editor : desktop.getEditors()) {
                        noChange = noChange && editor.checkCurrentFileSaved();
                    }
                }
                if(noChange) System.exit(0);
            }
        });
        menu.add(item);

        menuBar.add(menu);

        // menu Map
        menuMap = new JMenu("Map");
        menuMap.setMnemonic(KeyEvent.VK_T);
        menuMap.setEnabled(false);

        item = new JMenuItem("New");
        item.setMnemonic(KeyEvent.VK_N);
        item.getAccessibleContext().setAccessibleDescription("Create a new map");
        item.addActionListener(new ActionListenerNewMap());
        menuMap.add(item);

        item = new JMenuItem("Open...");
        item.setMnemonic(KeyEvent.VK_O);
        item.getAccessibleContext().setAccessibleDescription("Open an existing map");
        item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
        item.addActionListener(new ActionListenerOpenMap());
        menuMap.add(item);


        menuBar.add(menuMap);
        setJMenuBar(menuBar);

    }

    /**
     * This method is called just after a project is loaded.
     */
    public void currentProjectChanged() {
	menuMap.setEnabled(true);

//	if (map != null) {
//	    closeMap(); // close the map that was open with the previous project
//	}
    }

    /**
     * Prompts the user for a directory and creates a new project
     * in that directory.
     */
    private void newProject() {
        if (desktop.countEditors() > 0) {
            for (AbstractEditorWindow editor : desktop.getEditors()) {
                if (editor.checkCurrentFileSaved()) {
                    return;
                }
            }
        }
        ProjectFileChooser chooser = new ProjectFileChooser();
        String projectPath = chooser.getProjectPath();

        if (projectPath != null) {
            Project project = Project.createNew(projectPath);

            if (project == null) {
                GuiTools.warningDialog("A project already exists in this directory.");
            }
        }
    }

    /**
     * Prompts the user for a directory and loads the project
     * located in that directory.
     */
    private void loadProject() {
        if (desktop.countEditors() > 0) {
            for (AbstractEditorWindow editor : desktop.getEditors()) {
                if (editor.checkCurrentFileSaved()) {
                    return;
                }
            }
        }

        ProjectFileChooser chooser = new ProjectFileChooser();
        String projectPath = chooser.getProjectPath();

        if (projectPath != null) {
            try {
                Project project = Project.createExisting(projectPath);

                if (project == null) {
                    if (GuiTools.yesNoDialog("No project was found in this directory. Do you want to create a new one?")) {
                        Project.createNew(projectPath);

                        if (project == null) {
                            GuiTools.warningDialog("A project already exists in this directory.");
                        }
                    }
                }
            } catch (ZSDXException ex) {
                GuiTools.errorDialog("Cannot load the project: " + ex.getMessage());
            }
        }
    }

    /**
     * Action performed when the user clicks on Project > New project.
     * Creates a new project, asking to the user the project path.
     */
    private class ActionListenerNewProject implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            newProject();
        }
    }

    /**
     * Action performed when the user clicks on Project > Load project.
     * Loads an existing project, asking to the user the project path.
     */
    private class ActionListenerLoadProject implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            loadProject();
        }
    }

    /**
     * Action performed when the user clicks on Map > New.
     * Creates a new map, asking to the user the map name and the map file.
     */
    private class ActionListenerNewMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            MapEditorWindow mapEditor = new MapEditorWindow(EditorWindow.this.quest);
            mapEditor.newMap();
            EditorWindow.this.desktop.addEditor(mapEditor);
        }
    }

    /**
     * Action performed when the user clicks on Map > Load.
     * Opens an existing map, asking to the user the map file.
     */
    private class ActionListenerOpenMap implements ActionListener {

        public void actionPerformed(ActionEvent ev) {
            MapEditorWindow mapEditor = new MapEditorWindow(EditorWindow.this.quest);
            mapEditor.openMap();
            EditorWindow.this.desktop.addEditor(mapEditor);
        }
    }
}
