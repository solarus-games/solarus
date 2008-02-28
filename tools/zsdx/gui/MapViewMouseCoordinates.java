package zsdx.gui;

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
			setCoordinates(mapView.getMouseInMapX(e), mapView.getMouseInMapY(e));
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

	int xShown = GuiTools.round8(x);
	int yShown = GuiTools.round8(y);
	setText("(" + xShown + ", " + yShown + ")");
    }
    
}
