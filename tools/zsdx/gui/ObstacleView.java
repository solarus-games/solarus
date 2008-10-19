package zsdx.gui;

import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import zsdx.entities.*;

/**
 * Graphical component to select the obstacle properties of a tile:
 * - no obstacle
 * - obstacle
 * - obstacle top right
 * - obstacle top left
 * - obstacle bottom left
 * - obstacle bottom right
 */
public class ObstacleView extends JComboBox implements Observer, ActionListener {

    /**
     * The tile observed.
     */
    private Tile tile;

    /**
     * Constructor.
     */
    public ObstacleView() {
	super(ObstacleIcons.getIcons());
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
	setSelectedIndex(tile.getObstacle());
    }
    
    /**
     * This method is called when the selection the combo box is changed.
     * The tile is then updated.
     */
    public void actionPerformed(ActionEvent ev) {
	int listIndex = getSelectedIndex();
	if (listIndex != tile.getObstacle()) {
	    // the type of obstacle has changed
	    try {
		tile.setObstacle(listIndex);
	    }
	    catch (TilesetException e) {
		JOptionPane.showMessageDialog(null,
					      "Unable to set this type of obstacle: " + e.getMessage(),
					      "Error",
					      JOptionPane.ERROR_MESSAGE);
		update(null, null);
	    }
	}
    }
}
