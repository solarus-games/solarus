package zsdx.gui;

import java.awt.event.*;
import javax.swing.*;
import java.util.LinkedList;
import zsdx.*;
import zsdx.entities.*;
import zsdx.map_editor_actions.*;

/**
 * The popup menu shown when the user right clicks on the selected entities in the map view.
 */
public class MapViewPopupMenu extends JPopupMenu {

    /**
     * The map.
     */
    private Map map;

    /**
     * The entities selected.
     */
    private MapEntitySelection selection;

    /**
     * The map view where the popup menu is shown.
     */
    private MapView mapView;

    /**
     * Name of the layers, for the items in the popup menu.
     */
    private static final String[] layerNames = {"Low layer", "Intermediate layer", "High layer"};

    /**
     * Creates a shows a popup menu.
     * @param map view the map view
     */
    public MapViewPopupMenu(MapView theMapView) {
	super();

	this.mapView = theMapView;
	this.map = theMapView.getMap();

	selection = map.getEntitySelection();
	buildMenu();
    }

    /**
     * Creates the menu.
     * The options are:
     * Edit Resize | Create | Cut Copy Paste | Direction Layer Bring to front Bring to back | Destroy
     */
    private void buildMenu() {
	JMenuItem item;

	if (!selection.isEmpty()) {
	    // edit
	    item = new JMenuItem("Edit");
	    item.addActionListener(new ActionListenerEditEntity());
	    item.setEnabled(true);
	    add(item);

	    // resize
	    item = new JMenuItem("Resize");
	    item.setEnabled(selection.isResizable());
	    item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent e) {
		    mapView.startResizingEntity();
		}
	    });
	    add(item);

	    addSeparator();
	}

	// create
	add(new AddEntitiesMenu(mapView, "Create"));
	addSeparator();

	if (!selection.isEmpty()) {
	    // cut
	    item = new JMenuItem("Cut");
	    item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    mapView.cutSelectedEntities();
		}
	    });
	    add(item);

	    // copy
	    item = new JMenuItem("Copy");
	    item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    mapView.copySelectedEntities();
		}
	    });
	    add(item);
	}

	// paste
	item = new JMenuItem("Paste");
	item.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    mapView.paste();
		}
	    });
	item.setEnabled(mapView.canPaste());
	add(item);

	if (!selection.isEmpty()) {

	    addSeparator();

	    // direction
	    buildDirectionSubmenu();

	    // layer
	    buildLayerSubmenu();

	    // bring to front / to back
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
    }

    /**
     * Builds the "Direction" menu item if necessary.
     */
    private void buildDirectionSubmenu() {

	int nbDirections = selection.getNbDirections();
	if (nbDirections == 0) {
	    return;
	}

	JMenu menuDirection = new JMenu("Direction");
	
	JRadioButtonMenuItem[] itemsDirections = new JRadioButtonMenuItem[nbDirections + 1];
	ButtonGroup itemsDirectionsGroup = new ButtonGroup();

	for (int i = 0; i < nbDirections; i++) {
	    String name = (nbDirections == 4) ? DirectionChooser.directionNames4[i] : DirectionChooser.directionNames8[i];
	    itemsDirections[i] = new JRadioButtonMenuItem(name);
	    itemsDirections[i].addActionListener(new ActionListenerChangeDirection(i));
	    menuDirection.add(itemsDirections[i]);
	    itemsDirectionsGroup.add(itemsDirections[i]);
	}
	itemsDirections[nbDirections] = new JRadioButtonMenuItem();
	itemsDirections[nbDirections].addActionListener(new ActionListenerChangeDirection(nbDirections));
	itemsDirectionsGroup.add(itemsDirections[MapEntity.LAYER_NB]);

	add(menuDirection);

	int direction = selection.getDirection();

	// select the appropriate direction item
	if (direction != -1) {
	    // if all the selected entities have the same direction, we check its item
	    itemsDirections[direction].setSelected(true);
	}
	else {
	    // otherwise we select no item
	    itemsDirections[nbDirections].setSelected(true);
	}
    }

    /**
     * Builds the "Layer" menu item
     */
    public void buildLayerSubmenu() {
	JRadioButtonMenuItem[] itemsLayers = new JRadioButtonMenuItem[MapEntity.LAYER_NB + 1];
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
    }

    /**
     * Shows the popup menu.
     * @param x x coordinate of where the popup menu has to be shown
     * @param y y coordinate of where the popup menu has to be shown
     */
    public void display(int x, int y) {
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
		LinkedList<MapEntity> entities = map.getEntitySelection().getEntities();
		map.getHistory().doAction(new ActionChangeLayer(map, entities, layer));
	    }
	    catch (ZSDXException e) {
		GuiTools.errorDialog("Cannot change the layer: " + e.getMessage());
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
		LinkedList<MapEntity> entities = map.getEntitySelection().getEntities();
		map.getHistory().doAction(new ActionChangeDirection(map, entities, direction));
	    }
	    catch (ZSDXException e) {
		GuiTools.errorDialog("Cannot change the direction: " + e.getMessage());
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
		LinkedList<MapEntity> entities = map.getEntitySelection().getEntities();
		map.getHistory().doAction(new ActionBringToFront(map, entities));
	    }
	    catch (ZSDXException e) {
		GuiTools.errorDialog("Cannot bring the entities to front: " + e.getMessage());
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
		LinkedList<MapEntity> entities = map.getEntitySelection().getEntities();
		map.getHistory().doAction(new ActionBringToBack(map, entities));
	    }
	    catch (ZSDXException e) {
		GuiTools.errorDialog("Cannot bring the entities to back: " + e.getMessage());
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
