package editors;

import javax.swing.*;
import java.awt.*;
import java.awt.event.*;
import java.util.*;
import java.io.*;

/**
 * This component shows the map image and allow the user to modify it.
 */
public class MapView extends JComponent implements Observer {

    /**
     * The current map.
     */
    private Map map;

    /**
     * Constructor.
     */
    public MapView() {
	super();

	Configuration.getInstance().addObserver(this);

	setPreferredSize(new Dimension(500, 500));
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
