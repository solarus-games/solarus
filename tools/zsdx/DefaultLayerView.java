package zsdx;

import java.awt.event.*;
import javax.swing.*;
import java.util.*;

/**
 * Graphical component to select the default layer of a tile in the tileset editor:
 * - below
 * - intermediate
 * - above
 */
public class DefaultLayerView extends JComboBox implements Observer, ActionListener {

    /**
     * Text displayed in the list.
     */
    private static final String[] items = {
	"Below",
	"Intermediate",
	"Above"
    };

    /**
     * The tile observed.
     */
    private Tile tile;

    /**
     * Constructor.
     */
    public DefaultLayerView() {
	super(items);
	addActionListener(this);
    }

    /**
     * Sets the current tile observed.
     */
    public void setCurrentTile(Tile tile) {
	if (this.tile != null) {
	    this.tile.deleteObserver(this);
	}

	this.tile = tile;
	
	if (tile != null) {
	    tile.addObserver(this);
	    setEnabled(true);
	    update(tile, null);
	}
	else {
	    setEnabled(false);
	}
    }

    /**
     * This method is called when the tile is changed.
     * The selection is then updated.
     */
    public void update(Observable o, Object params) {
	setSelectedIndex(tile.getDefaultLayer());
    }
    
    /**
     * This method is called when the selection the combo box is changed.
     * The tile is then updated.
     */
    public void actionPerformed(ActionEvent e) {
	int listIndex = getSelectedIndex();
	if (listIndex != tile.getDefaultLayer()) {
	    // the layer has changed
	    tile.setDefaultLayer(listIndex);
	}
    }
}
