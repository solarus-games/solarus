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

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;

/**
 * Main window of the tileset editor.
 */
public class TilesetEditorWindow extends JFrame implements ProjectObserver {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * The list of tile patterns.
     */
    private TilePatternsView tilePatternsView;

    /**
     * The tileset image.
     */
    private TilesetImageView tilesetImageView;

    // menus or menu items memorized to enable it later
    private JMenu menuTileset;
    private JMenuItem menuItemClose;
    private JMenuItem menuItemSave;

    /**
     * Creates a new window.
     */
    public TilesetEditorWindow() {
	super("Zelda Solarus Deluxe - Tileset Editor");
	Project.addProjectObserver(this);

	// set a nice look and feel
	GuiTools.setLookAndFeel();

	// create the menu bar
	createMenuBar();

	// tile patterns list and tileset image

	// tile patterns list
	tilePatternsView = new TilePatternsView();
	tilePatternsView.setAlignmentY(Component.TOP_ALIGNMENT);
 	tilePatternsView.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));

	// tileset image
	tilesetImageView = new TilesetImageView(true);
	JScrollPane tilesetImageScroller = new JScrollPane(tilesetImageView);
	tilesetImageScroller.setAlignmentY(Component.TOP_ALIGNMENT);
 	tilesetImageScroller.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));

	JSplitPane tilesetPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, tilePatternsView, tilesetImageScroller);
	tilesetPanel.setContinuousLayout(true); 

	// we must put our main panel in another panel
	// otherwise the background color of the window is bad
	JPanel rootPanel = new JPanel(new BorderLayout());
	rootPanel.add(tilesetPanel);
	setContentPane(rootPanel);

	// add a window listener to confirm when the user closes the window
	setDefaultCloseOperation(JFrame.DO_NOTHING_ON_CLOSE);
	addWindowListener(new WindowAdapter() {
		public void windowClosing(WindowEvent e) {
		    if (checkCurrentFileSaved()) {
			dispose();
		    }
		}
	    });
	
	loadProject();
    }

    /**
     * Creates the menu bar and adds it to the window.
     */
    private void createMenuBar() {
	JMenuBar menuBar = new JMenuBar();

	JMenu menu;
	JMenuItem item;

	// menu Project
	menu = new JMenu("Project");
	menu.setMnemonic(KeyEvent.VK_P);

	item = new JMenuItem("New project...");
	item.setMnemonic(KeyEvent.VK_N);
	item.getAccessibleContext().setAccessibleDescription("Create a new ZSDX project");
	item.addActionListener(new ActionNewProject());
	menu.add(item);

	item = new JMenuItem("Load project...");
	item.setMnemonic(KeyEvent.VK_O);
	item.getAccessibleContext().setAccessibleDescription("Open an existing ZSDX project");
	item.addActionListener(new ActionLoadProject());
	menu.add(item);

	menu.addSeparator();

	item = new JMenuItem("Quit");
	item.setMnemonic(KeyEvent.VK_Q);
	item.getAccessibleContext().setAccessibleDescription("Teletransporter the tileset editor");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    if (checkCurrentFileSaved()) {
			dispose();
		    }
		}
	    });
	menu.add(item);

	menuBar.add(menu);

	// menu Tileset
	menuTileset = new JMenu("Tileset");
	menuTileset.setEnabled(false);
	menuTileset.setMnemonic(KeyEvent.VK_T);

	item = new JMenuItem("New");
	item.setMnemonic(KeyEvent.VK_N);
	item.getAccessibleContext().setAccessibleDescription("Create a new tileset");
	item.addActionListener(new ActionNew());
	menuTileset.add(item);

	item = new JMenuItem("Open...");
	item.setMnemonic(KeyEvent.VK_O);
	item.getAccessibleContext().setAccessibleDescription("Open an existing tileset");
	item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
	item.addActionListener(new ActionOpen());
	menuTileset.add(item);

	menuItemClose = new JMenuItem("Close");
	menuItemClose.setMnemonic(KeyEvent.VK_C);
	menuItemClose.getAccessibleContext().setAccessibleDescription("Close the current tileset");
	menuItemClose.addActionListener(new ActionClose());
	menuItemClose.setEnabled(false);
	menuTileset.add(menuItemClose);

	menuItemSave = new JMenuItem("Save");
	menuItemSave.setMnemonic(KeyEvent.VK_S);
	menuItemSave.getAccessibleContext().setAccessibleDescription("Save the current tileset");
	menuItemSave.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
	menuItemSave.addActionListener(new ActionSave());
	menuItemSave.setEnabled(false);
	menuTileset.add(menuItemSave);

	menuBar.add(menuTileset);

	setJMenuBar(menuBar);
    }

    /**
     * This method is called when a project has just been loaded.
     * The tileset menu is enabled.
     */
    public void currentProjectChanged() {
	menuTileset.setEnabled(true);

	if (tileset != null) {
	    closeTileset(); // close the tileset that was open with the previous project 
	}
    }

    /**
     * Sets the current tileset. This method is called when the user opens a tileset,
     * closes the tileset, or creates a new one.
     * @param tileset the new tileset, or null if no tileset is loaded
     */
    private void setTileset(Tileset tileset) {
	// if there was already a tileset, remove its observers
	if (this.tileset != null) {
	    this.tileset.deleteObservers();
	}

	this.tileset = tileset;

	// enable or disable the menu items
	menuItemClose.setEnabled(tileset != null);
	menuItemSave.setEnabled(tileset != null);

	// notify the views
	tilePatternsView.setTileset(tileset);
	tilesetImageView.setTileset(tileset);
    }

    /**
     * This function is called when the user wants to close the current tileset.
     * If the tileset is not saved, we propose to save it.
     * @return false if the user cancelled
     */
    private boolean checkCurrentFileSaved() {
	boolean result = true;

	if (tileset != null && !tileset.isSaved()) {
	    int answer =  JOptionPane.showConfirmDialog(this,
							"The tileset has been modified. Do you want to save it?",
							"Save the modifications",
							JOptionPane.YES_NO_CANCEL_OPTION,
							JOptionPane.WARNING_MESSAGE);
	    if (answer == JOptionPane.YES_OPTION) {
		new ActionSave().actionPerformed(null);
	    }
	    else if (answer == JOptionPane.CANCEL_OPTION) {
		result = false;
	    }
	}

	return result;
    }

    /**
     * Prompts the user for a directory and creates a new project
     * in that directory.
     */
    private void newProject() {

	if (!checkCurrentFileSaved()) {
	    return;
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

	if (!checkCurrentFileSaved()) {
	    return;
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
			else {
			    menuTileset.setEnabled(true);
			}
		    }
		}
	    }
	    catch (ZSDXException ex) {
		GuiTools.errorDialog("Cannot load the project: " + ex.getMessage());
	    }
	}
    }

    /**
     * Creates a new tileset in the project and sets it as the current tileset.
     */
    private void newTileset() {

	if (!checkCurrentFileSaved()) {
	    return;
	}

	try {
	    Tileset tileset = new Tileset();
	    setTileset(tileset);
	}
	catch (ZSDXException ex) {
	    GuiTools.errorDialog("Cannot create the tileset: " + ex.getMessage());
	}
    }

    /**
     * Loads a tileset of the project ans sets it as the current tileset.
     */
    private void openTileset() {

	if (!checkCurrentFileSaved()) {
	    return;
	}

	ResourceChooserDialog dialog = new ResourceChooserDialog(ResourceType.TILESET);
	dialog.setLocationRelativeTo(TilesetEditorWindow.this);
	dialog.pack();
	dialog.setVisible(true);
	String tilesetId = dialog.getSelectedId();

	if (tilesetId.length() == 0) {
	    return;
	}

	try {
	    Tileset tileset = new Tileset(tilesetId);
	    setTileset(tileset);
	}
	catch (ZSDXException ex) {
	    GuiTools.errorDialog("Could not load the tileset: " + ex.getMessage());
	}
    }

    /**
     * Closes the current tileset.
     */
    private void closeTileset() {

	if (!checkCurrentFileSaved()) {
	    return;
	}

	setTileset(null);
    }

    /**
     * Saves the current tileset.
     */
    private void saveTileset() {

	try {
	    tileset.save();
	}
	catch (ZSDXException ex) {
	    GuiTools.errorDialog("Could not save the tileset: " + ex.getMessage());
	}
    }

    /**
     * Action performed when the user clicks on Tileset > New.
     * Creates a new tileset, asking to the user the tileset name and the tileset file.
     */
    private class ActionNew implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {	    
	    newTileset();
	}
    }

    /**
     * Action performed when the user clicks on Tileset > Load.
     * Opens an existing tileset, asking to the user the tileset name.
     */
    private class ActionOpen implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {	    
	    openTileset();
	}
    }

    /**
     * Action performed when the user clicks on Tileset > Close.
     * Closes the current tileset.
     */
    private class ActionClose implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    closeTileset();
	}
    }
    
    /**
     * Action performed when the user clicks on Tileset > Save.
     * Saves the tileset into its file.
     */
    private class ActionSave implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    saveTileset();
	}
    }

    /**
     * Action performed when the user clicks on Project > New project.
     * Creates a new project, asking to the user the project path.
     */
    private class ActionNewProject implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    newProject();
	}
    }

    /**
     * Action performed when the user clicks on Project > Load project.
     * Loads an existing project, asking to the user the project path.
     */
    private class ActionLoadProject implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    loadProject();
	}
    }
}
