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
	

	try {
	
	    GameResourceList resourceList = GameResourceList.getInstance();
	    int[] mapIds = resourceList.getMapIds();
	    String name;
	    
	    for (int i = 0; i < mapIds.length; i++) {
		name = resourceList.getMapName(mapIds[i]);
		addItem(new KeyValue(Integer.toString(mapIds[i]), name));
	    }
	}
	catch (ZSDXException ex) {
	    WindowTools.errorDialog("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Returns the id of the currently selected map.
     * @return the id of the selected map, or -1 if no map is selected
     */
    public int getSelectedMapId() {

	KeyValue item = (KeyValue) getSelectedItem();
	return Integer.parseInt(item.getKey());
    }
}
