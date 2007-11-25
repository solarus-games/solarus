package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * This components shows the coordinates of the mouse on the map view.
 */
public class MapViewMouseCoordinates extends JLabel {

    /**
     * Constructor.
     */
    public MapViewMouseCoordinates(final MapView mapView) {

	setCoordinates(0, 0);

	mapView.addMouseMotionListener(new MouseMotionAdapter() {

		public void mouseMoved(MouseEvent e) {
		    if (mapView.getMap() != null) {
			setCoordinates(e.getX() / mapView.getZoom(), e.getY() / mapView.getZoom());
		    }
		}

	    });
    }
    
    /**
     * Changes the coordinates shown in the label.
     * @param x the new x coordinate (will be rounded)
     * @param y the new y coordinate (will be rounded)
     */
    private void setCoordinates(double x, double y) {

	int xShown = 8 * ((int) (x + 4) / 8);
	int yShown = 8 * ((int) (y + 4) / 8);
	setText("(" + xShown + ", " + yShown + ")");
    }
    
}
