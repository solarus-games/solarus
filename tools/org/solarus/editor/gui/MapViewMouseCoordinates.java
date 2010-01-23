/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package org.solarus.editor.gui;

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
