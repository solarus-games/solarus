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
     * The tileset list is reloaded using the game resource file.
     */
    public void update(Observable o, Object obj) {

	removeAllItems();

	if (showEmptyOption) {
	    addItem(new KeyValue("-1", ""));
	}

	try {
	
	    GameResourceList resourceList = GameResourceList.getInstance();
	    int[] tilesetIds = resourceList.getTilesetIds();
	    String name;
	    
	    for (int i = 0; i < tilesetIds.length; i++) {
		name = resourceList.getTilesetName(tilesetIds[i]);
		addItem(new KeyValue(Integer.toString(tilesetIds[i]), name));
	    }
	}
	catch (ZSDXException ex) {
	    WindowTools.errorDialog("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
    }

    /**
     * Returns the id of the currently selected tileset.
     * @return the id of the selected tileset, or -1 if no tileset is selected
     */
    public int getSelectedTilesetId() {

	KeyValue item = (KeyValue) getSelectedItem();
	return Integer.parseInt(item.getKey());
    }

    /**
     * Changes the currently selected tileset.
     * @param the id of the tileset you want to make selected in the combo box,
     * or -1 to select no tileset
     */
    public void setSelectedTilesetId(int id) {

	KeyValue item = new KeyValue(Integer.toString(id), null);
	setSelectedItem(item);
    }
}
