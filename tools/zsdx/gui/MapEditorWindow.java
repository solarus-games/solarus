package zsdx.gui;

import java.awt.*;
import java.awt.event.*;
import java.util.*;
import javax.swing.*;
import zsdx.*;
import zsdx.Map;

/**
 * Main window of the tileset editor.
 */
public class MapEditorWindow extends JFrame implements Observer {

    /**
     * The current map.
     */
    private Map map;

    // components
    private MapPropertiesView mapPropertiesView;
    private TilePicker tilePicker;
    private MapView mapView;

    // menu items memorized to enable them later
    private JMenu menuMap;
    private JMenuItem menuItemSave;
    private JMenuItem menuItemUndo;
    private JMenuItem menuItemRedo;

    /**
     * Creates a new window.
     */
    public MapEditorWindow() {
	super("Zelda Solarus Deluxe - Map Editor");

	// set a nice look and feel
	setLookAndFeel();

	// create the menu bar
	createMenuBar();

	// left panel : the map properties and the tile picker
	JPanel leftPanel = new JPanel();
	leftPanel.setLayout(new BoxLayout(leftPanel, BoxLayout.Y_AXIS));
	leftPanel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

	mapPropertiesView = new MapPropertiesView();
	mapPropertiesView.setAlignmentX(Component.LEFT_ALIGNMENT);
	mapPropertiesView.setMinimumSize(new Dimension(300, 240));
	mapPropertiesView.setPreferredSize(new Dimension(300, 240));
	mapPropertiesView.setMaximumSize(new Dimension(300, 240));

	tilePicker = new TilePicker();
	tilePicker.setAlignmentX(Component.LEFT_ALIGNMENT);
	tilePicker.setMaximumSize(new Dimension(Integer.MAX_VALUE, Integer.MAX_VALUE));
	
	leftPanel.add(mapPropertiesView);
	leftPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	leftPanel.add(tilePicker);

	mapView = new MapView();
	JScrollPane mapViewScroller = new JScrollPane(mapView);

	MapViewRenderingOptionsView mapViewRenderingOptionsView =
	    new MapViewRenderingOptionsView(mapView.getRenderingOptions());

	MapViewMouseCoordinates mapViewMouseCoordinates =
	    new MapViewMouseCoordinates(mapView);

	JPanel rightPanel = new JPanel(new BorderLayout());
	rightPanel.add(mapViewRenderingOptionsView, BorderLayout.NORTH);
	rightPanel.add(mapViewScroller, BorderLayout.CENTER);
	rightPanel.add(mapViewMouseCoordinates, BorderLayout.SOUTH);

	JSplitPane rootPanel = new JSplitPane(JSplitPane.HORIZONTAL_SPLIT, leftPanel, rightPanel);
	rootPanel.setContinuousLayout(true); 

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

	// menu Map
	menu = new JMenu("Map");
	menu.setMnemonic(KeyEvent.VK_T);
	
	item = new JMenuItem("New");
	item.setMnemonic(KeyEvent.VK_N);
	item.getAccessibleContext().setAccessibleDescription("Create a new map");
	item.addActionListener(new ActionListenerNew());
	menu.add(item);

	item = new JMenuItem("Load...");
	item.setMnemonic(KeyEvent.VK_O);
	item.getAccessibleContext().setAccessibleDescription("Open an existing map");
	item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
	item.addActionListener(new ActionListenerOpen());
	menu.add(item);

	menuItemSave = new JMenuItem("Save");
	menuItemSave.setMnemonic(KeyEvent.VK_S);
	menuItemSave.getAccessibleContext().setAccessibleDescription("Save the current map");
	menuItemSave.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
	menuItemSave.addActionListener(new ActionListenerSave());
	menuItemSave.setEnabled(false);
	menu.add(menuItemSave);

	menu.addSeparator();

	item = new JMenuItem("Quit");
	item.setMnemonic(KeyEvent.VK_Q);
	item.getAccessibleContext().setAccessibleDescription("Exit the map editor");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    if (checkCurrentFileSaved()) {
			dispose();
		    }
		}
	    });
	menu.add(item);

	menuBar.add(menu);

	// menu Edit
	menu = new JMenu("Edit");
	menu.setMnemonic(KeyEvent.VK_E);
	
	menuItemUndo = new JMenuItem("Undo");
	menuItemUndo.setMnemonic(KeyEvent.VK_U);
	menuItemUndo.getAccessibleContext().setAccessibleDescription("Undo the last action");
	menuItemUndo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Z, ActionEvent.CTRL_MASK));
	menuItemUndo.addActionListener(new ActionListenerUndo());
	menuItemUndo.setEnabled(false);
	menu.add(menuItemUndo);

	menuItemRedo = new JMenuItem("Redo");
	menuItemRedo.setMnemonic(KeyEvent.VK_R);
	menuItemRedo.getAccessibleContext().setAccessibleDescription("Redo the last action undone");
	menuItemRedo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, ActionEvent.CTRL_MASK));
	menuItemRedo.addActionListener(new ActionListenerRedo());
	menuItemRedo.setEnabled(false);
	menu.add(menuItemRedo);

	menuBar.add(menu);

	setJMenuBar(menuBar);
    }

    /**
     * Sets the current map. This method is called when the user opens a map,
     * closes the map, or creates a new one.
     * @param map the new map
     */
    private void setMap(Map map) {
	// if there was already a map, remove its observers
	if (this.map != null) {
	    map.deleteObservers();
	}
	
	this.map = map;

	// enable the menu items
	menuItemSave.setEnabled(true);

	// notify the views
	mapPropertiesView.setMap(map);
	tilePicker.setMap(map);
	mapView.setMap(map);

	// observe the history to enable or disable the items Save, Undo and Redo
	map.getHistory().addObserver(this);
	update(null, null);
    }

    /**
     * This function is called when the history changes.
     * @param o the history
     * @param obj additional parameter
     */
    public void update(Observable o, Object obj) {

	MapEditorHistory history = map.getHistory();

	menuItemSave.setEnabled(!history.isSaved());
	menuItemUndo.setEnabled(history.canUndo());
	menuItemRedo.setEnabled(history.canRedo());
    }
    
    /**
     * This method is called just after a project is loaded.
     */
    private void projectJustLoaded() {
	menuMap.setEnabled(true);
    }

    /**
     * This function is called when the user wants to close the current map.
     * If the map is not saved, we propose to save it.
     * @return false if the user cancelled
     */
    private boolean checkCurrentFileSaved() {
	boolean result = true;

	if (map != null && !map.getHistory().isSaved()) {
	    int answer =  JOptionPane.showConfirmDialog(this,
							"The map has been modified. Do you want to save it?",
							"Save the modifications",
							JOptionPane.YES_NO_CANCEL_OPTION,
							JOptionPane.WARNING_MESSAGE);
	    if (answer == JOptionPane.YES_OPTION) {
		new ActionListenerSave().actionPerformed(null);
	    }
	    else if (answer == JOptionPane.CANCEL_OPTION) {
		result = false;
	    }
	}

	return result;
    }


    /**
     * Action performed when the user clicks on Map > New.
     * Creates a new map, asking to the user the map name and the map file.
     */
    private class ActionListenerNew implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {

	    if (!checkCurrentFileSaved()) {
		return;
	    }

	    try {
		Map map = new Map();
		setMap(map);
	    }
	    catch (ZSDXException ex) {
		WindowTools.errorDialog("Cannot create the map: " + ex.getMessage());
	    }
	}
    }

    /**
     * Action performed when the user clicks on Map > Load.
     * Opens an existing map, asking to the user the map file.
     */
    private class ActionListenerOpen implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {

	    if (!checkCurrentFileSaved()) {
		return;
	    }

	    ResourceChooserDialog dialog = new ResourceChooserDialog(ResourceDatabase.RESOURCE_MAP);
	    dialog.setLocationRelativeTo(MapEditorWindow.this);
	    dialog.pack();
	    dialog.setVisible(true);
	    String mapId = dialog.getSelectedId();

	    if (mapId.length() == 0) {
		return;
	    }

	    try {
		Map map = new Map(mapId);
		
		if (map.badTiles()) {
		    WindowTools.warningDialog("Some tiles of the map have been removed because they don't exist in the tileset.");
		}
		setMap(map);
	    }
	    catch (ZSDXException ex) {
		WindowTools.errorDialog("Could not load the map: " + ex.getMessage());
	    }
	}
    }

    /**
     * Action performed when the user clicks on Map > Save.
     * Saves the map into its file.
     */
    private class ActionListenerSave implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    
	    try {
		map.save();
	    }
	    catch (ZSDXException ex) {
		WindowTools.errorDialog("Could not save the map: " + ex.getMessage());
	    }
	}
    }

    /**
     * Action performed when user the user clicks on Edit > Undo or presses Ctrl + Z.
     * The last action (if any) on the map is cancelled.
     */
    private class ActionListenerUndo implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    try {
		map.getHistory().undo();
	    }
	    catch (ZSDXException ex) {
		WindowTools.errorDialog("Cannot undo: " + ex.getMessage());
	    }
	}
    }

    /**
     * Action performed when user the user clicks on Edit > Redo or presses Ctrl + Y.
     * The last action cancelled (if any) is done again.
     */
    private class ActionListenerRedo implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    try {
		map.getHistory().redo();
	    }
	    catch (ZSDXException ex) {
		WindowTools.errorDialog("Cannot redo: " + ex.getMessage());
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
		    projectJustLoaded();
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
				projectJustLoaded();
			    }
			}
		    }
		    else {
			projectJustLoaded();
		    }
		}
		catch (ZSDXException ex) {
		    WindowTools.errorDialog("Cannot load the project: " + ex.getMessage());
		}
	    }
	}
    }
}
