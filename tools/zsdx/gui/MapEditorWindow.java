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
public class MapEditorWindow extends JFrame implements Observer, ProjectObserver {

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
    private JMenu menuEdit;
    private JMenu menuEntity;
    private JMenuItem menuItemClose;
    private JMenuItem menuItemSave;
    private JMenuItem menuItemUndo;
    private JMenuItem menuItemRedo;

    /**
     * Creates a new window.
     */
    public MapEditorWindow() {
	super("Zelda Solarus Deluxe - Map Editor");
	Project.addProjectObserver(this);

	// set a nice look and feel
	GuiTools.setLookAndFeel();

	// create the menu bar
	createMenuBar();

	// left panel : the map properties and the tile picker
	JPanel leftPanel = new JPanel();
	leftPanel.setLayout(new BoxLayout(leftPanel, BoxLayout.Y_AXIS));
	leftPanel.setBorder(BorderFactory.createEmptyBorder(5, 5, 5, 5));

	mapPropertiesView = new MapPropertiesView();
	mapPropertiesView.setAlignmentX(Component.LEFT_ALIGNMENT);
	Dimension size = new Dimension(300, 350);
//	mapPropertiesView.setMinimumSize(size);
	mapPropertiesView.setPreferredSize(size);
//	mapPropertiesView.setMaximumSize(size);

	tilePicker = new TilePicker();
	tilePicker.setAlignmentX(Component.LEFT_ALIGNMENT);
	
	leftPanel.add(mapPropertiesView);
	leftPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	leftPanel.add(tilePicker);

	mapView = new MapView();
	JScrollPane mapViewScroller = new JScrollPane(mapView);

	MapViewMouseCoordinates mapViewMouseCoordinates =
	    new MapViewMouseCoordinates(mapView);

	MapViewHeader mapViewHeader = new MapViewHeader(mapView);
	
	JPanel rightPanel = new JPanel(new BorderLayout());
	rightPanel.add(mapViewHeader, BorderLayout.NORTH);
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
	
	setMap(null);
	new ActionListenerLoadProject().actionPerformed(null);
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
	menuMap = new JMenu("Map");
	menuMap.setMnemonic(KeyEvent.VK_T);
	menuMap.setEnabled(false);
	
	item = new JMenuItem("New");
	item.setMnemonic(KeyEvent.VK_N);
	item.getAccessibleContext().setAccessibleDescription("Create a new map");
	item.addActionListener(new ActionListenerNew());
	menuMap.add(item);

	item = new JMenuItem("Open...");
	item.setMnemonic(KeyEvent.VK_O);
	item.getAccessibleContext().setAccessibleDescription("Open an existing map");
	item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
	item.addActionListener(new ActionListenerOpen());
	menuMap.add(item);

	menuItemClose = new JMenuItem("Close");
	menuItemClose.setMnemonic(KeyEvent.VK_C);
	menuItemClose.getAccessibleContext().setAccessibleDescription("Close the current map");
	menuItemClose.addActionListener(new ActionListenerClose());
	menuMap.add(menuItemClose);

	menuItemSave = new JMenuItem("Save");
	menuItemSave.setMnemonic(KeyEvent.VK_S);
	menuItemSave.getAccessibleContext().setAccessibleDescription("Save the current map");
	menuItemSave.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
	menuItemSave.addActionListener(new ActionListenerSave());
	menuMap.add(menuItemSave);

	menuBar.add(menuMap);

	// menu Edit
	menuEdit = new JMenu("Edit");
	menuEdit.setMnemonic(KeyEvent.VK_E);
	
	menuItemUndo = new JMenuItem("Undo");
	menuItemUndo.setMnemonic(KeyEvent.VK_U);
	menuItemUndo.getAccessibleContext().setAccessibleDescription("Undo the last action");
	menuItemUndo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Z, ActionEvent.CTRL_MASK));
	menuItemUndo.addActionListener(new ActionListenerUndo());
	menuItemUndo.setEnabled(false);
	menuEdit.add(menuItemUndo);

	menuItemRedo = new JMenuItem("Redo");
	menuItemRedo.setMnemonic(KeyEvent.VK_R);
	menuItemRedo.getAccessibleContext().setAccessibleDescription("Redo the last action undone");
	menuItemRedo.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_Y, ActionEvent.CTRL_MASK));
	menuItemRedo.addActionListener(new ActionListenerRedo());
	menuItemRedo.setEnabled(false);
	menuEdit.add(menuItemRedo);

	menuBar.add(menuEdit);

	// menu Entity
	menuEntity = new JMenu("Entity");
	menuEntity.setMnemonic(KeyEvent.VK_E);
	
	item = new JMenuItem("Add entrance");
	item.setMnemonic(KeyEvent.VK_E);
	item.getAccessibleContext().setAccessibleDescription("Add an entrance on the map");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_ENTRANCE));
	menuEntity.add(item);

	item = new JMenuItem("Add exit");
	item.setMnemonic(KeyEvent.VK_X);
	item.getAccessibleContext().setAccessibleDescription("Add an exit on the map");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_EXIT));
	menuEntity.add(item);

	item = new JMenuItem("Add pickable item");
	item.setMnemonic(KeyEvent.VK_P);
	item.getAccessibleContext().setAccessibleDescription("Add a pickable item (rupee, heart, fairy, key...)");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_PICKABLE_ITEM));
	menuEntity.add(item);

	item = new JMenuItem("Add transportable item");
	item.setMnemonic(KeyEvent.VK_T);
	item.getAccessibleContext().setAccessibleDescription("Add a transportable item (pot, bush...)");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_TRANSPORTABLE_ITEM));
	menuEntity.add(item);
	
	menuBar.add(menuEntity);

	setJMenuBar(menuBar);
    }

    /**
     * This method is called just after a project is loaded.
     */
    public void currentProjectChanged() {
	menuMap.setEnabled(true);
	
	if (map != null) {
	    closeMap(); // close the map that was open with the previous project 
	}
    }

    /**
     * Sets the current map. This method is called when the user opens a map,
     * closes the map, or creates a new one.
     * @param map the new map, or null if there is no map loaded
     */
    private void setMap(Map map) {
	// if there was already a map, remove its observers
	if (this.map != null) {
	    this.map.deleteObservers();
	}
	
	this.map = map;

	// enable or disable the menu items
	menuItemClose.setEnabled(map != null);
	menuItemSave.setEnabled(map != null);
	menuEdit.setEnabled(map != null);
	menuEntity.setEnabled(map != null);

	// notify the views
	mapPropertiesView.setMap(map);
	tilePicker.setMap(map);
	mapView.setMap(map);

	if (map != null) {
	    // observe the history to enable or disable the items Save, Undo and Redo
	    map.getHistory().addObserver(this);
	    update(null, null);
	}
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
     * This function is called when the user wants to close the current map.
     * If the map is not saved, we propose to save it.
     * @return false if the user cancelled
     */
    private boolean checkCurrentFileSaved() {
	boolean result = true;

	if (map != null && !map.getHistory().isSaved()) {
	    int answer = JOptionPane.showConfirmDialog(this,
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
		    }
		}
	    }
	    catch (ZSDXException ex) {
		GuiTools.errorDialog("Cannot load the project: " + ex.getMessage());
	    }
	}
    }

    /**
     * Creates a new map in the project and sets it as the current map.
     */
    private void newMap() {

	if (!checkCurrentFileSaved()) {
	    return;
	}

	try {
	    Map map = new Map();
	    setMap(map);
	}
	catch (ZSDXException ex) {
	    GuiTools.errorDialog("Cannot create the map: " + ex.getMessage());
	}
    }

    /**
     * Loads a map of the project ans sets it as the current map.
     */
    private void openMap() {

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
		GuiTools.warningDialog("Some tiles of the map have been removed because they don't exist in the tileset.");
	    }
	    setMap(map);
	}
	catch (ZSDXException ex) {
	    GuiTools.errorDialog("Could not load the map: " + ex.getMessage());
	}
    }

    /**
     * Closes the current map.
     */
    private void closeMap() {

	if (!checkCurrentFileSaved()) {
	    return;
	}

	setMap(null);
    }

    /**
     * Saves the current map.
     */
    private void saveMap() {

	try {
	    map.save();
	}
	catch (ZSDXException ex) {
	    GuiTools.errorDialog("Could not save the map: " + ex.getMessage());
	}
    }

    /**
     * Action performed when the user clicks on Map > New.
     * Creates a new map, asking to the user the map name and the map file.
     */
    private class ActionListenerNew implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    newMap();
	}
    }

    /**
     * Action performed when the user clicks on Map > Load.
     * Opens an existing map, asking to the user the map file.
     */
    private class ActionListenerOpen implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    openMap();
	}
    }

    /**
     * Action performed when the user clicks on Map > Close.
     * Closes the current map.
     */
    private class ActionListenerClose implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    closeMap();
	}
    }
    /**
     * Action performed when the user clicks on Map > Save.
     * Saves the map into its file.
     */
    private class ActionListenerSave implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    saveMap();
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
		GuiTools.errorDialog("Cannot undo: " + ex.getMessage());
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
		GuiTools.errorDialog("Cannot redo: " + ex.getMessage());
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
     * Action performed when the user wants to add an entity.
     */
    private class ActionListenerAddEntity implements ActionListener {

	// type of entity to add
	private int entityType;
	
	public ActionListenerAddEntity(int entityType) {
	    this.entityType = entityType;
	}
	
	public void actionPerformed(ActionEvent ev) {
	    mapView.startAddingEntity(entityType);
	}
    }
}
