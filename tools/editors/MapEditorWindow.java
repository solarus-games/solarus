package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.io.*;
import java.util.*;

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

    /**
     * Current file of the map (null if this is a new map).
     */
    private File mapFile;

    // menu items memorized to enable them later
    private JMenuItem menuItemSave;
    private JMenuItem menuItemSaveAs;
    private JMenuItem menuItemGenerate;
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
	mapPropertiesView.setMinimumSize(new Dimension(300, 200));
	mapPropertiesView.setPreferredSize(new Dimension(300, 200));
	mapPropertiesView.setMaximumSize(new Dimension(300, 200));

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

	// menu Map
	menu = new JMenu("Map");
	menu.setMnemonic(KeyEvent.VK_T);
	
	item = new JMenuItem("New");
	item.setMnemonic(KeyEvent.VK_N);
	item.getAccessibleContext().setAccessibleDescription("Create a new map");
	item.addActionListener(new ActionListenerNew());
	menu.add(item);

	item = new JMenuItem("Open...");
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

	menuItemSaveAs = new JMenuItem("Save as...");
	menuItemSaveAs.getAccessibleContext().setAccessibleDescription("Save the map into a new file");
	menuItemSaveAs.addActionListener(new ActionListenerSaveAs());
	menuItemSaveAs.setEnabled(false);
	menu.add(menuItemSaveAs);

	menu.addSeparator();

	menuItemGenerate = new JMenuItem("Generate C++");
	menuItemGenerate.setMnemonic(KeyEvent.VK_G);
	menuItemGenerate.getAccessibleContext().setAccessibleDescription("Generate the C++ code for the current map");
	menuItemGenerate.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, ActionEvent.CTRL_MASK));
	menuItemGenerate.addActionListener(new ActionListenerGenerate());
	menuItemGenerate.setEnabled(false);
	menu.add(menuItemGenerate);

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

	menu.addSeparator();

	item = new JMenuItem("Configuration...");
	item.setMnemonic(KeyEvent.VK_C);
	item.getAccessibleContext().setAccessibleDescription("Changes some settings");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    ConfigurationDialog dialog = new ConfigurationDialog();
		    dialog.setLocationRelativeTo(MapEditorWindow.this);
		    dialog.pack();
		    dialog.setVisible(true);
		}
	    });
	menu.add(item);

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
	menuItemSaveAs.setEnabled(true);
	menuItemGenerate.setEnabled(true);

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
     * Returns a file chooser adapted to select a map file.
     * @param dialogTitle title of the dialog box
     */
    private static JFileChooser getMapFileChooser(String dialogTitle) {

	JFileChooser fileChooser = new JFileChooser(Configuration.getInstance().getMapPath());
	fileChooser.setDialogTitle(dialogTitle);
	fileChooser.setDragEnabled(false);
	fileChooser.setMultiSelectionEnabled(false);

	return fileChooser;
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

	    // ask the tileset name
	    String name = JOptionPane.showInputDialog(MapEditorWindow.this,
						      "Please enter the name of your new map",
						      "Map name",
						      JOptionPane.QUESTION_MESSAGE);

	    if (name == null || name.length() == 0) {
		return;
	    }

	    mapFile = null;
	    map = new Map(name);
	    // map.reloadImage();
	    setMap(map);
	}
    }

    /**
     * Action performed when the user clicks on Map > Open.
     * Opens an existing map, asking to the user the map file.
     */
    private class ActionListenerOpen implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {

	    if (!checkCurrentFileSaved()) {
		return;
	    }

	    // ask the tileset file
	    JFileChooser fileChooser = getMapFileChooser("Open a map");
	    try {
		if (fileChooser.showOpenDialog(MapEditorWindow.this) == JFileChooser.APPROVE_OPTION) {
		    mapFile = fileChooser.getSelectedFile();
		    Map map = Map.load(mapFile);
		    // map.reloadImage();

		    if (map.badTiles()) {
			JOptionPane.showMessageDialog(MapEditorWindow.this,
						      "Some tiles of the map have been removed because they don't exist in the tileset.",
						      "Warning",
						      JOptionPane.WARNING_MESSAGE);
		    }
		    setMap(map);
		}
	    }
	    catch (IOException e) {
		WindowTools.errorDialog("Could not open the map file: " + e.getMessage());
	    }
	    catch (MapException e) {
		WindowTools.errorDialog("Could not load the map: " + e.getMessage());
	    }
	}
    }

    /**
     * Action performed when the user clicks on Map > Save.
     * Saves the map into its file.
     */
    private class ActionListenerSave implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    // if this is a new map, make a "save as..."
	    if (mapFile == null) {
		// default file
		mapFile = Configuration.getInstance().getMapFile(map.getName());
		new ActionListenerSaveAs().actionPerformed(ev);
	    }
	    else {
		try {
		    Map.save(mapFile, map);
		}
		catch (IOException e) {
		    WindowTools.errorDialog("Could not save the map file: " + e.getMessage());
		}
	    }
	}
    }

    /**
     * Action performed when the user clicks on Map > Save as.
     * Saves the map into its file.
     */
    private class ActionListenerSaveAs implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {

	    // ask the map file
	    JFileChooser fileChooser = getMapFileChooser("Save the map as...");
	    fileChooser.setSelectedFile(mapFile);
	    try {
		if (fileChooser.showSaveDialog(MapEditorWindow.this) == JFileChooser.APPROVE_OPTION) {
		    mapFile = fileChooser.getSelectedFile();
		    Map.save(mapFile, map);
		}
	    }
	    catch (IOException e) {
		WindowTools.errorDialog("Could not save the map file: " + e.getMessage());
	    }
	}
    }

    /**
     * Action performed when the user clicks on Map > Generate C++.
     * Generates the C++ code for the current map.
     */
    private class ActionListenerGenerate implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    MapCodeGenerator generator = new MapCodeGenerator();
	    
	    try {
		generator.generate(map);
		JOptionPane.showMessageDialog(MapEditorWindow.this,
					      "Map code generated!",
					      "Done",
					      JOptionPane.INFORMATION_MESSAGE);
	    }
	    catch (MapException e) {
		WindowTools.errorDialog("Could not generate the C++ code: " + e.getMessage());
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
	    catch (MapException e) {
		WindowTools.errorDialog("Cannot undo: " + e.getMessage());
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
	    catch (MapException e) {
		WindowTools.errorDialog("Cannot redo: " + e.getMessage());
	    }
	}
    }
}
