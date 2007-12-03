package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * The popup menu shown when the user right clicks on the selected tiles in the map view.
 */
public class MapViewPopupMenu extends JPopupMenu {

    /**
     * The map.
     */
    private Map map;

    /**
     * The map view where the popup menu is shown.
     */
    private MapView mapView;

    /**
     * Item in the popup menu to resize the selected tile.
     */
    private JMenuItem itemResize;

    /**
     * Items for the layers in the popup menu.
     */
    private JRadioButtonMenuItem[] itemsLayers;

    /**
     * Name of the layers, for the items in the popup menu.
     */
    private static final String[] layerNames = {"Low layer", "Intermediate layer", "High layer"};

    /**
     * Constructor
     */
    public MapViewPopupMenu(MapView theMapView) {
	super();

	JMenuItem item;

	this.mapView = theMapView;

	// items: resize, layer, bring to front, bring to back, destroy

	itemResize = new JMenuItem("Resize");
	itemResize.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    mapView.startResizingTile();
		}
	    });
	add(itemResize);

	addSeparator();

	itemsLayers = new JRadioButtonMenuItem[Tile.LAYER_NB + 1];
	ButtonGroup itemsLayersGroup = new ButtonGroup();
	    
	for (int i = 0; i < Tile.LAYER_NB; i++) {
	    itemsLayers[i] = new JRadioButtonMenuItem(layerNames[i]);
	    itemsLayers[i].addActionListener(new ActionListenerChangeLayer(i));
	    add(itemsLayers[i]);
	    itemsLayersGroup.add(itemsLayers[i]);
	}
	itemsLayers[Tile.LAYER_NB] = new JRadioButtonMenuItem();
	itemsLayers[Tile.LAYER_NB].addActionListener(new ActionListenerChangeLayer(Tile.LAYER_NB));
	itemsLayersGroup.add(itemsLayers[Tile.LAYER_NB]);

	addSeparator();

	item = new JMenuItem("Bring to front");
	item.addActionListener(new ActionListenerBringToFront());
	add(item);

	item = new JMenuItem("Bring to back");
	item.addActionListener(new ActionListenerBringToBack());
	add(item);

	addSeparator();

	item = new JMenuItem("Destroy");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    mapView.destroySelectedTiles();
		}
	    });
	add(item);
    }

    /**
     * Sets the observed map.
     */
    public void setMap(Map map) {

	this.map = map;
    }


    /**
     * Shows the popup menu.
     * @param map the map
     * @param x x coordinate of where the popup menu has to be shown
     * @param y y coordinate of where the popup menu has to be shown
     */
    public void show(int x, int y) {

	MapTileSelection tileSelection = map.getTileSelection();
	
	// enable or not the item "Resize"
	boolean enable = (tileSelection.getNbTilesSelected() == 1);
	itemResize.setEnabled(enable);

	// select the appropriate layer item
	int layer = map.getTileSelection().getLayer();

	if (layer != -1) {
	    // if all the selected tiles have the same layer, we check its item
	    itemsLayers[layer].setSelected(true);
	}
	else {
	    // otherwise we select no item
	    itemsLayers[Tile.LAYER_NB].setSelected(true);
	}

	// show the popup menu
	show(mapView, x, y);
    }

    /**
     * Action listener invoked when the user changes the layer of a tile
     * from the popup menu after a right click.
     */
    private class ActionListenerChangeLayer implements ActionListener {

	/**
	 * Layer to set when the action is invoked.
	 */
	private int layer;

	/**
	 * Constructor.
	 * @param layer layer to set when the action is invoked.
	 */
	public ActionListenerChangeLayer(int layer) {
	    this.layer = layer;
	}

	/**
	 * Method called when the user sets the layer of the selected tiles.
	 */
	public void actionPerformed(ActionEvent ev) {
	    try {
		map.getTileSelection().setLayer(layer);
	    }
	    catch (MapException e) {
		WindowTools.errorDialog("Cannot change the layer: " + e.getMessage());
	    }
	}
    }

    /**
     * Action listener invoker when the user clicks on "Bring to front".
     * The selected tiles are moved to the front in their layer.
     */
    private class ActionListenerBringToFront implements ActionListener {

	/**
	 * Method called when the action is performed.
	 */
	public void actionPerformed(ActionEvent ev) {
	    try {
		map.getTileSelection().bringToFront();
	    }
	    catch (MapException e) {
		WindowTools.errorDialog("Cannot bring the tiles to front: " + e.getMessage());
	    }
	}
    }

    /**
     * Action listener invoker when the user clicks on "Bring to back".
     * The selected tiles are moved to the back in their layer.
     */
    private class ActionListenerBringToBack implements ActionListener {

	/**
	 * Method called when the action is performed.
	 */
	public void actionPerformed(ActionEvent ev) {
	    try {
		map.getTileSelection().bringToBack();
	    }
	    catch (MapException e) {
		WindowTools.errorDialog("Cannot bring the tiles to back: " + e.getMessage());
	    }
	}
    }
}
