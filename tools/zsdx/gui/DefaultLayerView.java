package zsdx.gui;

import java.awt.event.*;
import javax.swing.*;
import java.util.*;
import zsdx.entities.*;


/**
 * Graphical component to select the default layer of a tile pattern in the tileset editor:
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
     * The tile pattern observed.
     */
    private TilePattern tilePattern;

    /**
     * Constructor.
     */
    public DefaultLayerView() {
	super(items);
	addActionListener(this);
    }

    /**
     * Sets the current tile pattern observed.
     */
    public void setCurrentTilePattern(TilePattern tilePattern) {
	if (this.tilePattern != null) {
	    this.tilePattern.deleteObserver(this);
	}

	this.tilePattern = tilePattern;
	
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
	setSelectedIndex(tilePattern.getDefaultLayer());
    }
    
    /**
     * This method is called when the selection the combo box is changed.
     * The tile pattern is then updated.
     */
    public void actionPerformed(ActionEvent e) {
	int listIndex = getSelectedIndex();
	if (listIndex != tilePattern.getDefaultLayer()) {
	    // the layer has changed
	    tilePattern.setDefaultLayer(listIndex);
	}
    }
}
