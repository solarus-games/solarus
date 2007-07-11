package editors;

import javax.swing.*;

/**
 * Main class of the tileset editor.
 */
public class TilesetEditor {

    /**
     * Entry point.
     */
    public static void main(String[] args) {

	// create the main window
	TilesetEditorWindow window = new TilesetEditorWindow();
	window.pack();
	window.setVisible(true);
    }

}
