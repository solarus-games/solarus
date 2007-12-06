package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import java.io.*;

/**
 * A combo box component to select a tileset.
 * This component shows the tileset list.
 */
public class TilesetChooser extends JComboBox implements Observer {

    /**
     * Indicates whether the first element of the combo box should be
     * an empty option (showing that no tileset is selected).
     */
    private boolean showEmptyOption;

    /**
     * Constructor.
     */
    public TilesetChooser(boolean showEmptyOption) {

	super();
	this.showEmptyOption = showEmptyOption;

	Configuration.getInstance().addObserver(this);
	update(Configuration.getInstance(), null);
    }

    /**
     * This function is called when the configuration is changed.
     * The tileset list is reloaded using ZSDX root path.
     */
    public void update(Observable o, Object obj) {

	removeAllItems();

	if (showEmptyOption) {
	    addItem("");
	}
	
	String tilesetPath = Configuration.getInstance().getTilesetPath();
	File[] tilesetFiles = FileTools.getFilesWithExtension(tilesetPath, "zsd");
	
	if (tilesetFiles == null) {
	    return;
	}
	
	for (int i = 0; i < tilesetFiles.length; i++) {
	    String tilesetName = FileTools.getFileNameWithoutExtension(tilesetFiles[i]);
	    addItem(tilesetName);
	}
    }
}
