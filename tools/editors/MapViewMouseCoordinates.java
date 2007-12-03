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
			setCoordinates((int) (e.getX() / mapView.getZoom()), (int) (e.getY() / mapView.getZoom()));
		    }
		}

		public void mouseDragged(MouseEvent e) {
		    mouseMoved(e);
		}

	    });
    }
    
    /**
     * Changes the coordinates shown in the label.
     * @param x the new x coordinate (will be rounded)
     * @param y the new y coordinate (will be rounded)
     */
    private void setCoordinates(int x, int y) {

	int xShown = (x + 4) / 8 * 8;
	int yShown = (y + 4) / 8 * 8;
	setText("(" + xShown + ", " + yShown + ")");
    }
    
}
