package editors;

import java.awt.*;
import javax.swing.*;
import java.util.*;

/**
 * This component shows the tileset and allows the user to pick a tile
 * in order to put it into the map.
 */
public class TilePicker extends JComponent implements Observer {

    /**
     * The current map.
     */
    private Map map;

    /**
     * Constructor.
     */
    public TilePicker() {
	super();

	Configuration.getInstance().addObserver(this);
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
     * This function is called when the map or the configuration changes.
     */
    public void update(Observable o, Object obj) {

	if (o instanceof Map) {
	    // the map has changed
	    
	    // redraw the image
	    repaint();
	}
	else if (o instanceof Configuration && map != null) {
	    // TODO
	}
    }

    /**
     * This function is called to display the component.
     * @param g the graphic context
     */
    public void paint(Graphics g) {

    }
}
