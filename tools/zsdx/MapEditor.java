package zsdx;

import zsdx.gui.*;

/**
 * Main class of the map editor.
 */
public class MapEditor {

    /**
     * Entry point.
     */
    public static void main(String[] args) {

	// create the main window
	MapEditorWindow window = new MapEditorWindow();
	window.pack();
	window.setExtendedState(MapEditorWindow.MAXIMIZED_BOTH);
	window.setVisible(true);
    }

}
