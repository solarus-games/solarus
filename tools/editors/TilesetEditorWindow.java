package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.io.*;

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

	// menu Tileset
	menu = new JMenu("Tileset");
	menu.setMnemonic(KeyEvent.VK_T);
	
	item = new JMenuItem("New");
	item.setMnemonic(KeyEvent.VK_N);
	item.getAccessibleContext().setAccessibleDescription("Create a new tileset");
	item.addActionListener(new ActionNew());
	menu.add(item);

	item = new JMenuItem("Open...");
	item.setMnemonic(KeyEvent.VK_O);
	item.getAccessibleContext().setAccessibleDescription("Open an existing tileset");
	item.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_O, ActionEvent.CTRL_MASK));
	item.addActionListener(new ActionOpen());
	menu.add(item);

	menuItemSave = new JMenuItem("Save");
	menuItemSave.setMnemonic(KeyEvent.VK_S);
	menuItemSave.getAccessibleContext().setAccessibleDescription("Save the current tileset");
	menuItemSave.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_S, ActionEvent.CTRL_MASK));
	menuItemSave.addActionListener(new ActionSave());
	menuItemSave.setEnabled(false);
	menu.add(menuItemSave);

	menuItemSaveAs = new JMenuItem("Save as...");
	menuItemSaveAs.getAccessibleContext().setAccessibleDescription("Save the tileset into a new file");
	menuItemSaveAs.addActionListener(new ActionSaveAs());
	menuItemSaveAs.setEnabled(false);
	menu.add(menuItemSaveAs);

	menu.addSeparator();

	menuItemGenerate = new JMenuItem("Generate C++");
	menuItemGenerate.setMnemonic(KeyEvent.VK_G);
	menuItemGenerate.getAccessibleContext().setAccessibleDescription("Generate the C++ code for the current tileset");
	menuItemGenerate.setAccelerator(KeyStroke.getKeyStroke(KeyEvent.VK_G, ActionEvent.CTRL_MASK));
	menuItemGenerate.addActionListener(new ActionGenerate());
	menuItemGenerate.setEnabled(false);
	menu.add(menuItemGenerate);

	item = new JMenuItem("Configuration...");
	item.setMnemonic(KeyEvent.VK_C);
	item.getAccessibleContext().setAccessibleDescription("Changes some settings");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    ConfigurationDialog dialog = new ConfigurationDialog();
		    dialog.setLocationRelativeTo(TilesetEditorWindow.this);
		    dialog.pack();
		    dialog.setVisible(true);
		}
	    });
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

	// enable the menu items
	menuItemSave.setEnabled(true);
	menuItemSaveAs.setEnabled(true);
	menuItemGenerate.setEnabled(true);

	// notify the views
	tilesView.setTileset(tileset);
	tilesetImageView.setTileset(tileset);
    }

    /**
     * Returns a file chooser adapted to select a tileset file.
     * TODO: make a filter
     * @param dialogTitle title of the dialog box
     */
    private static JFileChooser getTilesetFileChooser(String dialogTitle) {

	JFileChooser fileChooser = new JFileChooser(Configuration.getInstance().getDefaultTilesetPath());
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

	    if (!checkCurrentFileSaved()) {
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
		String path = Configuration.getInstance().getDefaultTilesetPath() +
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

    /**
     * Action performed when the user clicks on Tileset > Generate C++.
     * Generates the C++ code for the current tileset.
     */
    private class ActionGenerate implements ActionListener {
	
	public void actionPerformed(ActionEvent ev) {
	    TilesetCodeGenerator generator = new TilesetCodeGenerator();
	    
	    try {
		generator.generate(tileset);
		JOptionPane.showMessageDialog(TilesetEditorWindow.this,
					      "Tileset code generated!",
					      "Done",
					      JOptionPane.INFORMATION_MESSAGE);
	    }
	    catch (IOException e) {
		errorDialog("Could not generate the C++ code: " + e.getMessage());
	    }
	}
    }
}
