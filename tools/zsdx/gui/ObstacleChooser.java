package zsdx.gui;

import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import zsdx.entities.*;

/**
 * Graphical component to select the obstacle properties of a tile pattern:
 * - no obstacle
 * - obstacle
 * - obstacle top right
 * - obstacle top left
 * - obstacle bottom left
 * - obstacle bottom right
 * - shallow water
 * - deep water
 */
public class ObstacleChooser extends JComboBox implements Observer, ActionListener {

    /**
     * The tile pattern observed.
     */
    private TilePattern tilePattern;

    /**
     * Constructor.
     */
    public ObstacleChooser() {
	super(ObstacleIcons.getIcons());
	addActionListener(this);
    }

    /**
     * Sets the current tile pattern observed.
     */
    public void setCurrentTilePattern(TilePattern tile) {
	if (this.tilePattern != null) {
	    this.tilePattern.deleteObserver(this);
	}

	this.tilePattern = tile;
	
	if (tilePattern != null) {
	    tilePattern.addObserver(this);
	    setEnabled(true);
	    update(tilePattern, null);
	}
	else {
	    setEnabled(false);
	}
    }

    /**
     * This method is called when the tile pattern is changed.
     * The selection is then updated.
     */
    public void update(Observable o, Object params) {
	setSelectedIndex(tilePattern.getObstacle());
    }
    
    /**
     * This method is called when the selection the combo box is changed.
     * The tile pattern is then updated.
     */
    public void actionPerformed(ActionEvent ev) {
	int listIndex = getSelectedIndex();
	if (listIndex != tilePattern.getObstacle()) {
	    // the type of obstacle has changed
	    try {
		tilePattern.setObstacle(listIndex);
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
