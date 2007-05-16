package tileset_editor;

import java.awt.*;
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

	setLookAndFeel();

	Container container = getContentPane();
	CommandPanel commandPanel = new CommandPanel();
	TileList tileList = new TileList();
	TilesetImageView tilesetImageView = new TilesetImageView();

	container.add(commandPanel, BorderLayout.NORTH);
	container.add(tileList, BorderLayout.WEST);
	container.add(tilesetImageView, BorderLayout.CENTER);
    }

    /**
     * Tries to set a nice look and feel.
     */
    public void setLookAndFeel() {

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
}
