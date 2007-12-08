package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import java.io.*;

/**
 * A combo box component to select a map.
 * This component shows the map list.
 */
public class MapChooser extends JComboBox implements Observer {

    /**
     * Constructor.
     */
    public MapChooser() {

	super();

	Configuration.getInstance().addObserver(this);
	update(Configuration.getInstance(), null);
    }

    /**
     * This function is called when the configuration is changed.
     * The map list is reloaded using ZSDX root path.
     */
    public void update(Observable o, Object obj) {

	removeAllItems();
	
	String mapPath = Configuration.getInstance().getMapPath();
	File[] mapFiles = FileTools.getFilesWithExtension(mapPath, "zsd");
	
	if (mapFiles == null) {
	    return;
	}
	
	for (int i = 0; i < mapFiles.length; i++) {
	    String mapName = FileTools.getFileNameWithoutExtension(mapFiles[i]);
	    addItem(mapName);
	}
    }
}
