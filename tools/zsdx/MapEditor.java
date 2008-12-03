package zsdx;

import zsdx.gui.*;

/**
 * Main class of the map editor.
 */
public class MapEditor {

	// initialization
	/* Java 1.6 Update 10 introduces by default a Direct3D handling for the Java2D operations,
	 * but under Windows drawing an image without transparency becomes extremely slow
	 * so we disable Direct3D until the bug is fixed.
	 */
	static {
		System.setProperty("sun.java2d.d3d", "false"); 
	}

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
