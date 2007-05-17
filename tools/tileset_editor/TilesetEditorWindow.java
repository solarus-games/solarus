package tileset_editor;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * Main window of the tileset editor.
 */
public class TilesetEditorWindow extends JFrame {

    /**
     * Creates a new window.
     */
    public TilesetEditorWindow() {
	super("Zelda Solarus Deluxe - Tileset Editor");

	// set a nice look and feel
	setLookAndFeel();

	// create the menu bar
	JMenuBar menuBar = createMenuBar();
	setJMenuBar(menuBar);
	
	// create the panels
	Container container = getContentPane();
	ConfigurationPanel configurationPanel = new ConfigurationPanel();
	TileList tileList = new TileList();
	TilesetImageView tilesetImageView = new TilesetImageView();

	container.add(configurationPanel, BorderLayout.NORTH);
	container.add(tileList, BorderLayout.WEST);
	container.add(tilesetImageView, BorderLayout.CENTER);
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
     * Creates the menu bar.
     * @return the menu bar
     */
    private JMenuBar createMenuBar() {
	JMenuBar menuBar = new JMenuBar();

	JMenu menu;
	JMenuItem item;

	// menu File
	menu = new JMenu("Tileset");
	menu.setMnemonic(KeyEvent.VK_T);
	
	item = new JMenuItem("New");
	item.setMnemonic(KeyEvent.VK_N);
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
		public void actionPerformed(ActionEvent e) {
		    System.exit(0);
		}
	    });
	menu.add(item);

	menuBar.add(menu);

	return menuBar;
    }
}
