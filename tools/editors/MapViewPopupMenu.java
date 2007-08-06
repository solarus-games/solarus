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

	this.mapView = theMapView;

	// items: resize, layer, destroy

	itemResize = new JMenuItem("Resize");
	itemResize.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    mapView.startResizingSelectedTile();
		}
	    });
	add(itemResize);

	addSeparator();

	itemsLayers = new JRadioButtonMenuItem[Tile.LAYER_NB + 1];
	ButtonGroup itemsLayersGroup = new ButtonGroup();
	    
	for (int i = 0; i < Tile.LAYER_NB; i++) {
	    itemsLayers[i] = new JRadioButtonMenuItem(layerNames[i]);
	    itemsLayers[i].addActionListener(new ActionChangeLayer(i));
	    add(itemsLayers[i]);
	    itemsLayersGroup.add(itemsLayers[i]);
	}
	itemsLayers[Tile.LAYER_NB] = new JRadioButtonMenuItem();
	itemsLayers[Tile.LAYER_NB].addActionListener(new ActionChangeLayer(Tile.LAYER_NB));
	itemsLayersGroup.add(itemsLayers[Tile.LAYER_NB]);

	addSeparator();

	JMenuItem item = new JMenuItem("Destroy");
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
    private class ActionChangeLayer implements ActionListener {

	/**
	 * Layer to set when the action is invoked.
	 */
	private int layer;

	/**
	 * Constructor.
	 * @param layer layer to set when the action is invoked.
	 */
	public ActionChangeLayer(int layer) {
	    this.layer = layer;
	}

	/**
	 * Method called when the user sets the layer of the selected tiles.
	 */
	public void actionPerformed(ActionEvent e) {
	    map.getTileSelection().setLayer(layer);
	    repaint();
	}
    }
}
