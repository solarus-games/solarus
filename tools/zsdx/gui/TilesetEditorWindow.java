package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import zsdx.*;

/**
 * Main window of the tileset editor.
 */
public class TilesetEditorWindow extends JFrame {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * The list of tiles.
     */
    private TilesView tilesView;

    /**
     * The tileset image.
     */
    private TilesetImageView tilesetImageView;

    // menus or menu items memorized to enable it later
    private JMenu menuTileset;
    private JMenuItem menuItemSave;

    /**
     * Creates a new window.
     */
    public TilesetEditorWindow() {
	super("Zelda Solarus Deluxe - Tileset Editor");

	// set a nice look and feel
	setLookAndFeel();

	// create the menu bar
	createMenuBar();

	// tile list and tileset image

	// tile list
	tilesView = new TilesView();
	tilesView.setAlignmentY(Component.TOP_ALIGNMENT);
 	tilesView.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));

	// tileset image
	tilesetImageView = new TilesetImageView(true);
	JScrollPane tilesetImageScroller = new JScrollPane(tilesetImageView);
	tilesetImageScroller.setAlignmentY(Component.TOP_ALIGNMENT);
 	tilesetImageScroller.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));

	JSplitPane tilesetPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, tilesView, tilesetImageScroller);
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
	
	new ActionLoadProject().actionPerformed(null);
    }

    /**
     * Tries to set a nice look and feel.
     */
    private void setLookAndFeel() {

	// set Windows look and feel by default
	try {
	    UIManager.setLookAndFeel("com.sun.java.swing.plaf.windows.WindowsLookAndFeel");
	    return;
	}
	catch (Exception e) {
	}

	// try Mac OS
	try {
	    UIManager.setLookAndFeel("it.unitn.ing.swing.plaf.macos.MacOSLookAndFeel");
	    return;
	}
	catch (Exception e) {
	}

	// otherwise, try GTK
// 	try {
// 	    UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel");
// 	    return;
// 	}
// 	catch (Exception e) {
// 	}
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
	item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
	item.addActionListener(new ActionLoadProject());
	menu.add(item);

	menu.addSeparator();

	item = new JMenuItem("Quit");
	item.setMnemonic(KeyEvent.VK_Q);
	item.getAccessibleContext().setAccessibleDescription("Exit the tileset editor");
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

	item = new JMenuItem("Load...");
	item.setMnemonic(KeyEvent.VK_O);
	item.getAccessibleContext().setAccessibleDescription("Open an existing tileset");
	item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
	item.addActionListener(new ActionOpen());
	menuTileset.add(item);

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
     * Sets the current tileset. This method is called when the user opens a tileset,
     * closes the tileset, or creates a new one.
     * @param tileset the new tileset
     */
    private void setTileset(Tileset tileset) {
	// if there was already a tileset, remove its observers
	if (this.tileset != null) {
	    tileset.deleteObservers();
	}
	
	this.tileset = tileset;

	// enable the menu item
	menuItemSave.setEnabled(true);

	// notify the views
	tilesView.setTileset(tileset);
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
     * Action performed when the user clicks on Tileset > New.
     * Creates a new tileset, asking to the user the tileset name and the tileset file.
     */
    private class ActionNew implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {

	    if (!checkCurrentFileSaved()) {
		return;
	    }

	    try {
		Tileset tileset = new Tileset();
		setTileset(tileset);
	    }
	    catch (ZSDXException ex) {
		WindowTools.errorDialog("Cannot create the tileset: " + ex.getMessage());
	    }
	}
    }

    /**
     * Action performed when the user clicks on Tileset > Load.
     * Opens an existing tileset, asking to the user the tileset name.
     */
    private class ActionOpen implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {

	    if (!checkCurrentFileSaved()) {
		return;
	    }

	    ResourceChooserDialog dialog = new ResourceChooserDialog(ResourceDatabase.RESOURCE_TILESET);
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
		WindowTools.errorDialog("Could not load the tileset: " + ex.getMessage());
	    }
	}
    }

    /**
     * Action performed when the user clicks on Tileset > Save.
     * Saves the tileset into its file.
     */
    private class ActionSave implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    try {
		tileset.save();
	    }
	    catch (ZSDXException ex) {
		WindowTools.errorDialog("Could not save the tileset: " + ex.getMessage());
	    }
	}
    }

    /**
     * Action performed when the user clicks on Project > New project.
     * Creates a new project, asking to the user the project path.
     */
    private class ActionNewProject implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {

	    if (!checkCurrentFileSaved()) {
		return;
	    }

	    ProjectFileChooser chooser = new ProjectFileChooser();
	    String projectPath = chooser.getProjectPath();

	    if (projectPath != null) {
		Project project = Project.createNew(projectPath);
		
		if (project == null) {
		    WindowTools.warningDialog("A project already exists in this directory.");
		}
		else {
		    menuTileset.setEnabled(true);
		}
	    }
	}
    }

    /**
     * Action performed when the user clicks on Project > Load project.
     * Loads an existing project, asking to the user the project path.
     */
    private class ActionLoadProject implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {

	    if (!checkCurrentFileSaved()) {
		return;
	    }
	    
	    ProjectFileChooser chooser = new ProjectFileChooser();
	    String projectPath = chooser.getProjectPath();

	    if (projectPath != null) {
		try {
		    Project project = Project.createExisting(projectPath);
		    
		    if (project == null) {
			if (WindowTools.yesNoDialog("No project was found in this directory. Do you want to create a new one?")) {
			    Project.createNew(projectPath);

			    if (project == null) {
				WindowTools.warningDialog("A project already exists in this directory.");
			    }
			    else {
				menuTileset.setEnabled(true);
			    }
			}
		    }
		    else {
			menuTileset.setEnabled(true);
		    }
		}
		catch (ZSDXException ex) {
		    WindowTools.errorDialog("Cannot load the project: " + ex.getMessage());
		}
	    }
	}
    }
}
