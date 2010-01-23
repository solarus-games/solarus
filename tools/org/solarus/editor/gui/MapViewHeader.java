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

import java.awt.*;
import javax.swing.*;

/**
 * This componenent shows the header of the map view.
 * It contains a tool bar to add new entities, and some options
 * to choose how the map is displayed.
 */
public class MapViewHeader extends JPanel {

    /**
     * Constructor.
     * @param mapView the map view
     */
    MapViewHeader(MapView mapView) {
	super(new BorderLayout());
	
	MapViewRenderingOptionsView renderingOptionsView =
	    new MapViewRenderingOptionsView(mapView.getRenderingOptions());
	
	AddEntitiesToolbar addEntitiesToolbar = new AddEntitiesToolbar(mapView);

	add(renderingOptionsView, BorderLayout.CENTER);
	add(addEntitiesToolbar, BorderLayout.SOUTH);
    }
}
