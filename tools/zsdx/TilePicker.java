package zsdx;

import java.awt.*;
import javax.swing.*;
import java.util.*;

/**
 * This component shows the tileset and allows the user to pick a tile
 * in order to put it into the map.
 */
public class TilePicker extends JScrollPane implements Observer {

    /**
     * The current map.
     */
    private Map map;

    /**
     * The component in the scroll pane.
     */
    private TilesetImageView tilesetImageView;

    /**
     * Constructor.
     */
    public TilePicker() {
	super();

	tilesetImageView = new TilesetImageView(false);
	setViewportView(tilesetImageView);
    }

    /**
     * Sets the observed map.
     */
    public void setMap(Map map) {
	if (this.map != null) {
	    this.map.deleteObserver(this);
	}

	this.map = map;
	map.addObserver(this);

	update(map, null);
    }
    
    /**
     * This function is called when the map changes.
     */
    public void update(Observable o, Object obj) {
	if (map != null) {
	    tilesetImageView.setTileset(map.getTileset());
	}
    }
}
