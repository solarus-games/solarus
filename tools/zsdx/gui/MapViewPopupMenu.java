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
     * Edit Resize | Create Convert | Cut Copy Paste | Direction Layer Bring to front Bring to back | Destroy
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

	if (selection.getNbEntitiesSelected() == 1) {

	    MapEntity entity = selection.getEntity(0);

	    if (entity instanceof Tile) {

		boolean toDynamic = !(entity instanceof DynamicTile);

		String text = toDynamic ? "Convert to dynamic tile" :
		    "Convert to static tile";

		item = new JMenuItem(text);
		item.addActionListener(new ActionListenerConvertTile((Tile) entity));
		add(item);
	    }
	}

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
	itemsDirectionsGroup.add(itemsDirections[nbDirections]);

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

	int nbLayers = Layer.values().length;
	JRadioButtonMenuItem[] itemsLayers = new JRadioButtonMenuItem[nbLayers + 1];
	ButtonGroup itemsLayersGroup = new ButtonGroup();

	for (int i = 0; i < nbLayers; i++) {
	    itemsLayers[i] = new JRadioButtonMenuItem(layerNames[i]);
	    itemsLayers[i].addActionListener(new ActionListenerChangeLayer(Layer.get(i)));
	    add(itemsLayers[i]);
	    itemsLayersGroup.add(itemsLayers[i]);
	}
	itemsLayers[nbLayers] = new JRadioButtonMenuItem();
	itemsLayersGroup.add(itemsLayers[nbLayers]);

	// select the appropriate layer item
	Layer layer = selection.getLayer();

	if (layer != null) {
	    // if all the selected entities have the same layer, we check its item
	    itemsLayers[layer.getId()].setSelected(true);
	}
	else {
	    // otherwise we select no item
	    itemsLayers[nbLayers].setSelected(true);
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
	private Layer layer;

	/**
	 * Constructor.
	 * @param layer layer to set when the action is invoked.
	 */
	public ActionListenerChangeLayer(Layer layer) {
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

    /**
     * Action listener invoked when the user clicks on "Convert tile".
     */
    private class ActionListenerConvertTile implements ActionListener {

	private Tile tile;

	/**
	 * Constructor.
	 */
	public ActionListenerConvertTile(Tile tile) {
	    this.tile = tile;
	}

	/**
	 * Method called when the user clicks on the item.
	 */
	public void actionPerformed(ActionEvent ev) {

	    try {
		MapEntity newTile;
		if (tile instanceof DynamicTile) {
		    newTile = ((DynamicTile) tile).createStaticTile(); 
		}
		else {
		    newTile = tile.createDynamicTile();
		}
		// create a dynamic tile
		map.getHistory().doAction(new ActionReplaceEntity(map, tile, newTile));
	    }
	    catch (ZSDXException ex) {
		GuiTools.errorDialog("Cannot convert this tile: " + ex.getMessage());
	    }
	}
    }
}
