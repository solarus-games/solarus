package zsdx.gui;

import java.awt.event.*;
import javax.swing.*;
import zsdx.entities.*;

/**
 * A menu with items to create any kind of entity.
 */
public class AddEntitiesMenu extends JMenu {

    private MapView mapView;

    /**
     * Constructor.
     * @param mapView the map view
     * @param title name of the menu
     */
    public AddEntitiesMenu(MapView mapView, String title) {
	super(title);
	this.mapView = mapView;

	JMenuItem item;

	// entrance
	item = new JMenuItem("Entrance");
	item.setMnemonic(KeyEvent.VK_E);
	item.getAccessibleContext().setAccessibleDescription("Add an entrance on the map");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_ENTRANCE));
	add(item);

	// exit
	item = new JMenuItem("Exit");
	item.setMnemonic(KeyEvent.VK_X);
	item.getAccessibleContext().setAccessibleDescription("Add an exit on the map");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_EXIT));
	add(item);

	// pickable item
	item = new JMenuItem("Pickable item");
	item.setMnemonic(KeyEvent.VK_P);
	item.getAccessibleContext().setAccessibleDescription("Add a pickable item (rupee, heart, fairy, key...)");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_PICKABLE_ITEM));
	add(item);

	// destructible item
	item = new JMenu("Destructible item");
	item.setMnemonic(KeyEvent.VK_D);
	item.getAccessibleContext().setAccessibleDescription("Add a destructible item (pot, bush...)");
	buildDestructibleItemsSubmenu((JMenu) item);
	add(item);

	// chest
	item = new JMenuItem("Chest");
	item.setMnemonic(KeyEvent.VK_C);
	item.getAccessibleContext().setAccessibleDescription("Add a chest");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_CHEST));
	add(item);
    }

    /**
     * Buils a submenu allowing to add some types of pickable item
     * @param submenu the submenu to fill
     */
    private void buildDestructibleItemsSubmenu(JMenu submenu) {

	JMenuItem item;

	item = new JMenuItem("Grass");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_DESTRUCTIBLE_ITEM, DestructibleItem.GRASS));
	submenu.add(item);

	item = new JMenuItem("Bush");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_DESTRUCTIBLE_ITEM, DestructibleItem.BUSH));
	submenu.add(item);

	item = new JMenuItem("Pot");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_DESTRUCTIBLE_ITEM, DestructibleItem.POT));
	submenu.add(item);
    }

    /**
     * Action performed when the user wants to add an entity.
     */
    class ActionListenerAddEntity implements ActionListener {

	// type of entity to add
	private int entityType;
	private int entitySubtype;

	public ActionListenerAddEntity(int entityType) {
	    this(entityType, 0);
	}

	public ActionListenerAddEntity(int entityType, int entitySubtype) {
	    this.entityType = entityType;
	    this.entitySubtype = entitySubtype;
	}

	public void actionPerformed(ActionEvent ev) {
	    mapView.startAddingEntity(entityType, entitySubtype);
	}
    }

}
