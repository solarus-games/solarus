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

	// destination point
	item = new JMenuItem("Destination point");
	item.setMnemonic(KeyEvent.VK_D);
	item.getAccessibleContext().setAccessibleDescription("Add a destination point on the map");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_DESTINATION_POINT));
	add(item);

	// teletransporter
	item = new JMenuItem("Teletransporter");
	item.setMnemonic(KeyEvent.VK_T);
	item.getAccessibleContext().setAccessibleDescription("Add a teletransporter on the map");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_TELETRANSPORTER));
	add(item);

	// pickable item
	item = new JMenuItem("Pickable item");
	item.setMnemonic(KeyEvent.VK_P);
	item.getAccessibleContext().setAccessibleDescription("Add a pickable item (rupee, heart, fairy, key...)");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_PICKABLE_ITEM));
	add(item);

	// destructible item
	item = new JMenu("Destructible item");
	item.setMnemonic(KeyEvent.VK_E);
	item.getAccessibleContext().setAccessibleDescription("Add a destructible item (pot, bush...)");
	buildDestructibleItemSubmenu((JMenu) item);
	add(item);

	// chest
	item = new JMenuItem("Chest");
	item.setMnemonic(KeyEvent.VK_C);
	item.getAccessibleContext().setAccessibleDescription("Add a chest");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_CHEST));
	add(item);

	// jump sensor
	item = new JMenuItem("Jump sensor");
	item.setMnemonic(KeyEvent.VK_J);
	item.getAccessibleContext().setAccessibleDescription("Add a jump sensor");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_JUMP_SENSOR));
	add(item);
	
	// enemy
	item = new JMenuItem("Enemy");
	item.setMnemonic(KeyEvent.VK_Y);
	item.getAccessibleContext().setAccessibleDescription("Add an enemy");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_ENEMY));
	add(item);

	// interactive entity
	item = new JMenu("Interactive entity");
	item.setMnemonic(KeyEvent.VK_I);
	item.getAccessibleContext().setAccessibleDescription("Add an interactive entity");
	buildInteractiveEntitySubmenu((JMenu) item);
	add(item);
    }

    /**
     * Buils a submenu allowing to add some types of destructible items
     * @param submenu the submenu to fill
     */
    private void buildDestructibleItemSubmenu(JMenu submenu) {

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

	item = new JMenuItem("White stone");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_DESTRUCTIBLE_ITEM, DestructibleItem.STONE_SMALL_WHITE));
	submenu.add(item);

	item = new JMenuItem("Black stone");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_DESTRUCTIBLE_ITEM, DestructibleItem.STONE_SMALL_BLACK));
	submenu.add(item);
    }

    /**
     * Buils a submenu allowing to add some types of interactive entities
     * @param submenu the submenu to fill
     */
    private void buildInteractiveEntitySubmenu(JMenu submenu) {

	JMenuItem item;

	item = new JMenuItem("Custom");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_INTERACTIVE, InteractiveEntity.Subtype.CUSTOM.ordinal()));
	submenu.add(item);

	item = new JMenuItem("Non playing character");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_INTERACTIVE, InteractiveEntity.Subtype.NON_PLAYING_CHARACTER.ordinal()));
	submenu.add(item);

	item = new JMenuItem("Sign");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_INTERACTIVE, InteractiveEntity.Subtype.SIGN.ordinal()));
	submenu.add(item);

	item = new JMenuItem("Water for bottle");
	item.addActionListener(new ActionListenerAddEntity(MapEntity.ENTITY_INTERACTIVE, InteractiveEntity.Subtype.WATER_FOR_BOTTLE.ordinal()));
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
