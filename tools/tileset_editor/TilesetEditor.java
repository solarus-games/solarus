package tileset_editor;

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
	window.setDefaultCloseOperation(JFrame.EXIT_ON_CLOSE);
	window.pack();
	window.setVisible(true);
    }

}
