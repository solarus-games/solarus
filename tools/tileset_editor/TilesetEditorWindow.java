package tileset_editor;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import java.io.*;

/**
 * Main window of the tileset editor.
 */
public class TilesetEditorWindow extends JFrame implements Observer {

    /**
     * The current tileset.
     */
    private Tileset tileset;

    /**
     * The configuration panel (with the text field to set ZSDX root path).
     */
    private ConfigurationPanel configurationPanel;

    /**
     * The list of tiles.
     */
    private TileList tileList;

    /**
     * The tileset image.
     */
    private TilesetImageView tilesetImageView;

    /**
     * Current file of the tileset (null if this is a new tileset).
     */
    private File tilesetFile;

    // menu items memorized to enable them later
    private JMenuItem menuItemSave;
    private JMenuItem menuItemSaveAs;
    private JMenuItem menuItemGenerate;

    /**
     * Creates a new window.
     */
    public TilesetEditorWindow() {
	super("Zelda Solarus Deluxe - Tileset Editor");

	// set a nice look and feel
	//	setLookAndFeel();

	// create the menu bar
	createMenuBar();

	// create the panels
	Box globalPanel = new Box(BoxLayout.Y_AXIS);

	// tile table and tileset image
	Box tilesetPanel = new Box(BoxLayout.X_AXIS);
	tilesetPanel.setAlignmentX(Component.LEFT_ALIGNMENT);

	// tile table
	tileList = new TileList();
	tileList.setAlignmentY(Component.TOP_ALIGNMENT);

	// tileset image
	tilesetImageView = new TilesetImageView();
	JScrollPane tilesetImageViewScroll = new JScrollPane(tilesetImageView);
	tilesetImageViewScroll.setAlignmentY(Component.TOP_ALIGNMENT);

	tilesetPanel.add(tileList);
	tilesetPanel.add(Box.createRigidArea(new Dimension(5, 0)));
	tilesetPanel.add(tilesetImageViewScroll);

	// ZSDX root path configuration
	configurationPanel = new ConfigurationPanel();
 	configurationPanel.setMaximumSize(new Dimension(500, 80));
	configurationPanel.setAlignmentX(Component.LEFT_ALIGNMENT);

	// add the panels
	globalPanel.add(configurationPanel);
	globalPanel.add(Box.createRigidArea(new Dimension(0, 5)));
	globalPanel.add(tilesetPanel);

	// we must put our global panel in another panel
	// otherwise the background color of the window is bad
	JPanel rootPanel = new JPanel(new BorderLayout());
	rootPanel.add(globalPanel);
	setContentPane(rootPanel);
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

	// otherwise, try GTK
	try {
	    UIManager.setLookAndFeel("com.sun.java.swing.plaf.gtk.GTKLookAndFeel");
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
    }

    /**
     * Creates the menu bar and adds it to the window.
     */
    private void createMenuBar() {
	JMenuBar menuBar = new JMenuBar();

	JMenu menu;
	JMenuItem item;

	// menu File
	menu = new JMenu("Tileset");
	menu.setMnemonic(KeyEvent.VK_T);
	
	item = new JMenuItem("New");
	item.setMnemonic(KeyEvent.VK_N);
	item.getAccessibleContext().setAccessibleDescription("Create a new tileset");
	item.addActionListener(new ActionNew());
	menu.add(item);

	item = new JMenuItem("Open...");
	item.setMnemonic(KeyEvent.VK_O);
	item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
	item.addActionListener(new ActionOpen());
	menu.add(item);

	menuItemSave = new JMenuItem("Save");
	menuItemSave.setMnemonic(KeyEvent.VK_S);
	menuItemSave.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
	menuItemSave.addActionListener(new ActionSave());
	menuItemSave.setEnabled(false);
	menu.add(menuItemSave);

	menuItemSaveAs = new JMenuItem("Save as...");
	menuItemSaveAs.addActionListener(new ActionSaveAs());
	menuItemSaveAs.setEnabled(false);
	menu.add(menuItemSaveAs);

	menuItemGenerate = new JMenuItem("Generate C++");
	menuItemGenerate.setMnemonic(KeyEvent.VK_G);
	menuItemGenerate.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, ActionEvent.CTRL_MASK));
	menuItemGenerate.setEnabled(false);
	menu.add(menuItemGenerate);

	item = new JMenuItem("Quit");
	item.setMnemonic(KeyEvent.VK_Q);
	item.getAccessibleContext().setAccessibleDescription("Exit the tileset editor");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    System.exit(0);
		}
	    });
	menu.add(item);

	menuBar.add(menu);

	setJMenuBar(menuBar);
    }

    /**
     * Sets the current tileset. This method is called when the user opens a tileset
     * close the tileset, or creates a new one.
     * @param tileset the new tileset
     */
    private void setTileset(Tileset tileset) {
	// if there was already a tileset, remove its observers
	if (this.tileset != null) {
	    tileset.deleteObservers();
	}
	
	this.tileset = tileset;

	// add the tileset as observer
	tileset.addObserver(this);
	update(tileset, null);
	
	// enable the menu items
	menuItemSave.setEnabled(true);
	menuItemSaveAs.setEnabled(true);
	menuItemGenerate.setEnabled(true);

	// notify the views
	tileList.setTileset(tileset);
	tilesetImageView.setTileset(tileset);
    }

    /**
     * This function is called when the tileset changes.
     */
    public void update(Observable o, Object obj) {
	
    }

    /**
     * Returns a file chooser adapted to select a tileset file.
     * TODO: make a filter
     * @param dialogTitle title of the dialog box
     */
    private static JFileChooser getTilesetFileChooser(String dialogTitle) {

	JFileChooser fileChooser = new JFileChooser(Tileset.getDefaultTilesetPath());
	fileChooser.setDialogTitle(dialogTitle);
	fileChooser.setDragEnabled(false);
	fileChooser.setMultiSelectionEnabled(false);

	return fileChooser;
    }

    /**
     * Shows a dialog box with an error message.
     * @param error message the error message to show
     */
    private void errorDialog(String errorMessage) {
	JOptionPane.showMessageDialog(this,
				      errorMessage,
				      "Error",
				      JOptionPane.ERROR_MESSAGE);
    }

    /**
     * This function is called when the user wants to open a tileset or create a new one.
     * If the current tileset is not saved, we propose to save it.
     * @return false if the user cancelled
     */
    private boolean checkCurrentTilesetSaved() {
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

	    if (!checkCurrentTilesetSaved()) {
		return;
	    }

	    // ask the tileset name
	    String name = JOptionPane.showInputDialog(TilesetEditorWindow.this,
						      "Please enter the name of your new tileset",
						      "Tileset name",
						      JOptionPane.QUESTION_MESSAGE);

	    if (name == null || name.length() == 0) {
		return;
	    }

	    tilesetFile = null;
	    tileset = new Tileset(name);
	    setTileset(tileset);
	}
    }

    /**
     * Action performed when the user clicks on Tileset > Open.
     * Opens an existing tileset, asking to the user the tileset file.
     */
    private class ActionOpen implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {

	    if (!checkCurrentTilesetSaved()) {
		return;
	    }

	    // ask the tileset file
	    JFileChooser fileChooser = getTilesetFileChooser("Open a tileset");
	    try {
		if (fileChooser.showOpenDialog(TilesetEditorWindow.this) == JFileChooser.APPROVE_OPTION) {
		    tilesetFile = fileChooser.getSelectedFile();
		    Tileset tileset = Tileset.load(tilesetFile);
		    setTileset(tileset);
		}
	    }
	    catch (IOException e) {
		errorDialog("Could not open the tileset file: " + e.getMessage());
	    }
	}
    }

    /**
     * Action performed when the user clicks on Tileset > Save.
     * Saves the tileset into its file.
     */
    private class ActionSave implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    // if this is a new tileset, make a "save as..."
	    if (tilesetFile == null) {
		// default file
		String path = Tileset.getDefaultTilesetPath() +
		    File.separator + tileset.getName() + ".zsd";

		tilesetFile = new File(path);
		new ActionSaveAs().actionPerformed(ev);
	    }
	    else {
		try {
		    Tileset.save(tilesetFile, tileset);
		}
		catch (IOException e) {
		    errorDialog("Could not save the tileset file: " + e.getMessage());
		}
	    }
	}
    }

    /**
     * Action performed when the user clicks on Tileset > Save as.
     * Saves the tileset into its file.
     */
    private class ActionSaveAs implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {

	    // ask the tileset file
	    JFileChooser fileChooser = getTilesetFileChooser("Save the tileset as...");
	    fileChooser.setSelectedFile(tilesetFile);
	    try {
		if (fileChooser.showSaveDialog(TilesetEditorWindow.this) == JFileChooser.APPROVE_OPTION) {
		    tilesetFile = fileChooser.getSelectedFile();
		    Tileset.save(tilesetFile, tileset);
		}
	    }
	    catch (IOException e) {
		errorDialog("Could not save the tileset file: " + e.getMessage());
	    }
	}
    }
}
