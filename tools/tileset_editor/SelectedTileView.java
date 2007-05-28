package tileset_editor;

import java.awt.*;
import javax.swing.*;
import java.util.*;

/**
 * This component shows information about the current tile and lets the user edit it.
 */
public class SelectedTileView extends JPanel implements Observer {

    /**
     * The tile shown.
     */
    private Tile tile;

    /**
     * Constructor.
     */
    public SelectedTileView() {
	super();
	add(new JLabel("selected tile view"));
    }

    /**
     * Changes the tile shown.
     * @param tile the new current tile (can be null)
     */
    public void setCurrentTile(Tile tile) {
	if (this.tile != null) {
	    this.tile.deleteObserver(this);
	}

	this.tile = tile;
	
	if (tile != null) {
	    tile.addObserver(this);
	}
    }

    /**
     * This function is called when the tile changes.
     */
    public void update(Observable o, Object params) {

    }
}
