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
     * Creates a new window.
     */
    public TilesetEditorWindow() {
	super("Zelda Solarus Deluxe - Tileset Editor");

	// set a nice look and feel
	setLookAndFeel();

	// create the menu bar
	createMenuBar();
	
	// create the panels
	Container container = getContentPane();
	container.setLayout(new BoxLayout(container, BoxLayout.Y_AXIS));
	configurationPanel = new ConfigurationPanel();
 	configurationPanel.setMaximumSize(new Dimension(500, 80));
	configurationPanel.setAlignmentX(Component.LEFT_ALIGNMENT);

	Box tilesetPanel = new Box(BoxLayout.X_AXIS);
	tilesetPanel.setAlignmentX(Component.LEFT_ALIGNMENT);

	tileList = new TileList();
 	tileList.setMaximumSize(new Dimension(300, 500));
	tileList.setAlignmentY(Component.TOP_ALIGNMENT);

	tilesetImageView = new TilesetImageView();
	tilesetImageView.setAlignmentY(Component.TOP_ALIGNMENT);

	tilesetPanel.add(tileList);
	tilesetPanel.add(Box.createRigidArea(new Dimension(5, 0)));
	tilesetPanel.add(tilesetImageView);

	container.add(configurationPanel);
	container.add(Box.createRigidArea(new Dimension(0, 5)));
	container.add(tilesetPanel);
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
	menu.add(item);

	item = new JMenuItem("Save");
	item.setMnemonic(KeyEvent.VK_S);
	item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
	menu.add(item);

	item = new JMenuItem("Save as...");
	menu.add(item);

	item = new JMenuItem("Generate C++");
	item.setMnemonic(KeyEvent.VK_G);
	item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, ActionEvent.CTRL_MASK));
	menu.add(item);

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

	tileset.addObserver(this);
	tileList.setTileset(tileset);
	// TODO: add other GUI components as observers
    }

    /**
     * This function is called when the tileset changes.
     */
    public void update(Observable o, Object obj) {
	
    }

    /**
     * Action performed when the user clicks on Tileset > New.
     */
    private class ActionNew implements ActionListener {
	
	/**
	 * Creates a new tileset, asking to the user the tileset name and the tileset file.
	 */
	public void actionPerformed(ActionEvent ev) {
	    

	    // ask the tileset name
	    String name = JOptionPane.showInputDialog(TilesetEditorWindow.this,
						  "Please enter the name of your new tileset",
						  "Tileset name",
						  JOptionPane.QUESTION_MESSAGE);
	    
	    if (name == null || name.length() == 0) {
		return;
	    }

	    // ask the tileset file
	    String defaultTilesetPath = configurationPanel.getZsdxRootPath() +
		File.separator + "tools" + File.separator + "tileset_editor" +
		File.separator + "tilesets";
	    
	    String defaultTilesetFile = defaultTilesetPath + File.separator + name + ".zsd";

	    JFileChooser fileChooser = new JFileChooser(defaultTilesetPath);
	    fileChooser.setDialogTitle("Save tileset as...");
	    fileChooser.setDragEnabled(false);
	    fileChooser.setMultiSelectionEnabled(false);
	    fileChooser.setSelectedFile(new File(defaultTilesetFile));
	    int result = fileChooser.showSaveDialog(TilesetEditorWindow.this);

	    if (result != JFileChooser.APPROVE_OPTION) {
		return;
	    }

	    // create the tileset
	    try {
		Tileset tileset = new Tileset(fileChooser.getSelectedFile(), name);
		setTileset(tileset);
	    }
	    catch (IOException e) {
		JOptionPane.showMessageDialog(TilesetEditorWindow.this,
					      "Could not create the tileset file",
					      "Error",
					      JOptionPane.ERROR_MESSAGE);
	    }
	}
    }
}
