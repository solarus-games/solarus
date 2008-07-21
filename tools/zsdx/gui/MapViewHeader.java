package zsdx.gui;

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
