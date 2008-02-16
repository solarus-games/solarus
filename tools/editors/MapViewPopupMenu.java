package editors;

import java.awt.*;
import java.awt.event.*;
import javax.swing.*;

/**
 * The popup menu shown when the user right clicks on the selected entities in the map view.
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
     * Item in the popup menu to edit the selected entity.
     */
    private JMenuItem itemEdit;

    /**
     * Item in the popup menu to resize the selected entity.
     */
    private JMenuItem itemResize;

    /**
     * Items for the layers in the popup menu.
     */
    private JRadioButtonMenuItem[] itemsLayers;

    /**
     * Submenu to select the direction of the selected entities.
     */
    private JMenu menuDirection;

    /**
     * Items for the directions in the submenu.
     */
    private JRadioButtonMenuItem[] itemsDirections;

    /**
     * Name of the layers, for the items in the popup menu.
     */
    private static final String[] layerNames = {"Low layer", "Intermediate layer", "High layer"};

    /**
     * Name of the directions, for the items in the sub menu.
     */
    private static final String[] directionNames = {"Right", "Up", "Left", "Down"};

    /**
     * Constructor
     */
    public MapViewPopupMenu(MapView theMapView) {
	super();

	JMenuItem item;

	this.mapView = theMapView;

	// items: edit, resize, layer, direction, bring to front, bring to back, destroy

	itemEdit = new JMenuItem("Edit");
	itemEdit.addActionListener(new ActionListenerEditEntity());
	add(itemEdit);

	itemResize = new JMenuItem("Resize");
	itemResize.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    mapView.startResizingEntity();
		}
	    });
	add(itemResize);

	addSeparator();

	itemsLayers = new JRadioButtonMenuItem[MapEntity.LAYER_NB + 1];
	ButtonGroup itemsLayersGroup = new ButtonGroup();
	
	for (int i = 0; i < MapEntity.LAYER_NB; i++) {
	    itemsLayers[i] = new JRadioButtonMenuItem(layerNames[i]);
	    itemsLayers[i].addActionListener(new ActionListenerChangeLayer(i));
	    add(itemsLayers[i]);
	    itemsLayersGroup.add(itemsLayers[i]);
	}
	itemsLayers[MapEntity.LAYER_NB] = new JRadioButtonMenuItem();
	itemsLayers[MapEntity.LAYER_NB].addActionListener(new ActionListenerChangeLayer(MapEntity.LAYER_NB));
	itemsLayersGroup.add(itemsLayers[MapEntity.LAYER_NB]);

	addSeparator();

	menuDirection = new JMenu("Direction");
	
	itemsDirections = new JRadioButtonMenuItem[5];
	ButtonGroup itemsDirectionsGroup = new ButtonGroup();
	
	for (int i = 0; i < 4; i++) {
	    itemsDirections[i] = new JRadioButtonMenuItem(directionNames[i]);
	    itemsDirections[i].addActionListener(new ActionListenerChangeDirection(i));
	    menuDirection.add(itemsDirections[i]);
	    itemsDirectionsGroup.add(itemsDirections[i]);
	}
	itemsDirections[4] = new JRadioButtonMenuItem();
	itemsDirections[4].addActionListener(new ActionListenerChangeDirection(4));
	itemsDirectionsGroup.add(itemsDirections[MapEntity.LAYER_NB]);

	add(menuDirection);

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
		    mapView.destroySelectedEntities();
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
     * @param x x coordinate of where the popup menu has to be shown
     * @param y y coordinate of where the popup menu has to be shown
     */
    public void show(int x, int y) {

	MapEntitySelection selection = map.getEntitySelection();
	int nbSelected = selection.getNbEntitiesSelected();
	
	// enable or not the item "Edit"
	itemResize.setEnabled(nbSelected == 1 && selection.hasName());

	// enable or not the item "Resize"
	itemResize.setEnabled(selection.isResizable());

	// select the appropriate layer item
	int layer = selection.getLayer();

	if (layer != -1) {
	    // if all the selected entities have the same layer, we check its item
	    itemsLayers[layer].setSelected(true);
	}
	else {
	    // otherwise we select no item
	    itemsLayers[MapEntity.LAYER_NB].setSelected(true);
	}

	// enable or not the item "Direction"
	boolean hasDirection = selection.hasDirection();

	menuDirection.setEnabled(hasDirection);
	if (hasDirection) {

	    int direction = selection.getDirection();
	    
	    // select the appropriate direction item
	    
	    if (direction != -1) {
		// if all the selected entities have the same direction, we check its item
		itemsDirections[direction].setSelected(true);
	    }
	    else {
		// otherwise we select no item
		itemsDirections[4].setSelected(true);
	    }
	}

	// show the popup menu
	show(mapView, x, y);
    }

    /**
     * Action listener invoked when the user changes the layer of the selected
     * entities from the popup menu after a right click.
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
	 * Method called when the user sets the layer of the selected entities.
	 */
	public void actionPerformed(ActionEvent ev) {
	    try {
		map.getEntitySelection().setLayer(layer);
	    }
	    catch (ZSDXException e) {
		WindowTools.errorDialog("Cannot change the layer: " + e.getMessage());
	    }
	}
    }

    /**
     * Action listener invoked when the user changes the direction of the selected
     * entities from the popup menu after a right click.
     */
    private class ActionListenerChangeDirection implements ActionListener {

	/**
	 * Direction to set when the action is invoked.
	 */
	private int direction;

	/**
	 * Constructor.
	 * @param direction direction to set when the action is invoked.
	 */
	public ActionListenerChangeDirection(int direction) {
	    this.direction = direction;
	}

	/**
	 * Method called when the user sets the direction of the selected entities.
	 */
	public void actionPerformed(ActionEvent ev) {
	    try {
		map.getEntitySelection().setDirection(direction);
	    }
	    catch (ZSDXException e) {
		WindowTools.errorDialog("Cannot change the direction: " + e.getMessage());
	    }
	}
    }

    /**
     * Action listener invoked when the user clicks on "Bring to front".
     * The selected entities are moved to the front in their layer.
     */
    private class ActionListenerBringToFront implements ActionListener {

	/**
	 * Method called when the action is performed.
	 */
	public void actionPerformed(ActionEvent ev) {
	    try {
		map.getEntitySelection().bringToFront();
	    }
	    catch (ZSDXException e) {
		WindowTools.errorDialog("Cannot bring the entities to front: " + e.getMessage());
	    }
	}
    }

    /**
     * Action listener invoker when the user clicks on "Bring to back".
     * The selected entities are moved to the back in their layer.
     */
    private class ActionListenerBringToBack implements ActionListener {

	/**
	 * Method called when the action is performed.
	 */
	public void actionPerformed(ActionEvent ev) {
	    try {
		map.getEntitySelection().bringToBack();
	    }
	    catch (ZSDXException e) {
		WindowTools.errorDialog("Cannot bring the entities to back: " + e.getMessage());
	    }
	}
    }

    /**
     * Action listener invoked when the user clicks on "Edit".
     * A popup menu to edit the selected entity is shown.
     */
    private class ActionListenerEditEntity implements ActionListener {

	/**
	 * Constructor.
	 */
	public ActionListenerEditEntity() {
	    
	}

	/**
	 * Method called when the user clicks on "Edit".
	 */
	public void actionPerformed(ActionEvent ev) {

	    MapEntity entity = map.getEntitySelection().getEntity(0);

	    EditEntityDialog dialog = new EditEntityDialog(map, entity);
	    dialog.setLocationRelativeTo(null);
	    dialog.pack();
	    dialog.setVisible(true);
	}
    }
}
