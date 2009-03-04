package zsdx.gui;

import java.awt.event.*;
import javax.swing.*;
import zsdx.entities.*;

/**
 * A menu with items to create any kind of entity.
 */
public class AddEntitiesMenu extends JMenu {

    /**
     * All menu items.
     */
    private static final ItemDefinition[] itemDefinitions = {
	new ItemDefinition(EntityType.DESTINATION_POINT, KeyEvent.VK_D,
		DestinationPoint.Subtype.INVISIBLE,
		DestinationPoint.Subtype.GRAY),
	new ItemDefinition(EntityType.TELETRANSPORTER, KeyEvent.VK_T,
		Teletransporter.Subtype.INVISIBLE,
		Teletransporter.Subtype.YELLOW),
	new ItemDefinition(EntityType.PICKABLE_ITEM, KeyEvent.VK_P,
		PickableItem.Subtype.RUPEE_1,
		PickableItem.Subtype.RUPEE_5,
		PickableItem.Subtype.RUPEE_20,
		PickableItem.Subtype.FAIRY,
		PickableItem.Subtype.PIECE_OF_HEART,
		PickableItem.Subtype.SMALL_KEY
		),
	new ItemDefinition(EntityType.DESTRUCTIBLE_ITEM, KeyEvent.VK_S,
		DestructibleItem.Subtype.GRASS,
		DestructibleItem.Subtype.BUSH,
		DestructibleItem.Subtype.POT,
		DestructibleItem.Subtype.STONE_SMALL_WHITE,
		DestructibleItem.Subtype.STONE_SMALL_BLACK
	),
	new ItemDefinition(EntityType.CHEST, KeyEvent.VK_C),
	new ItemDefinition(EntityType.ENEMY, KeyEvent.VK_E, new EntitySubtype[] {
		Enemy.Subtype.SIMPLE_GREEN_SOLDIER
	}),
	new ItemDefinition(EntityType.INTERACTIVE, KeyEvent.VK_I, new EntitySubtype[] {
		InteractiveEntity.Subtype.CUSTOM,
		InteractiveEntity.Subtype.NON_PLAYING_CHARACTER,
		InteractiveEntity.Subtype.SIGN,
		InteractiveEntity.Subtype.WATER_FOR_BOTTLE
	}),	
    };

    /**
     * Defines an item of the menu.
     */
    private static class ItemDefinition {
	private EntityType type;
	private EntitySubtype[] subtypes;
	private int key;

	/**
	 * Creates an item.
	 * @param type type of the entity to create
	 * @param key a mnemonic key (like KeyEvent.VK_A)
	 * @param subtypes subtypes to propose (none if there is no subtype)
	 */
	public ItemDefinition(EntityType type, int key, EntitySubtype ... subtypes) {
	    this.type = type;
	    this.subtypes = subtypes;
	    this.key = key;

	    if (!hasSubtypes() && type.hasSubtype()) {
		System.err.println("Subtypes missing in entities menu for type " + type);
		System.exit(1);
	    }
	}

	public EntityType getEntityType() {
	    return type;
	}

	public boolean hasSubtypes() {
	    return subtypes.length != 0;
	}

	public EntitySubtype[] getEntitySubtypes() {
	    return subtypes;
	}

	public int getKey() {
	    return key;
	}
    }

    /**
     * The map view.
     */
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
	for (ItemDefinition def: itemDefinitions) {
	    EntityType type = def.getEntityType();
	    if (!def.hasSubtypes()) {
		item = new JMenuItem(type.getName());
		item.setMnemonic(def.getKey());
		item.addActionListener(new ActionListenerAddEntity(type));
	    }
	    else {
		item = new JMenu(type.getName());
		item.setMnemonic(def.getKey());

		for (EntitySubtype subtype: def.getEntitySubtypes()) {
		    JMenuItem subitem = new JMenuItem(type.getSubtypeName(subtype));
		    subitem.addActionListener(new ActionListenerAddEntity(type, subtype));
		    item.add(subitem);
		}
	    }
	    add(item);
	}
    }

    /**
     * Action performed when the user wants to add an entity.
     */
    private class ActionListenerAddEntity implements ActionListener {

	// type of entity to add
	private EntityType entityType;
	private EntitySubtype entitySubtype;

	public ActionListenerAddEntity(EntityType entityType) {
	    this(entityType, null);
	}

	public ActionListenerAddEntity(EntityType entityType, EntitySubtype entitySubtype) {
	    this.entityType = entityType;
	    this.entitySubtype = entitySubtype;
	}

	public void actionPerformed(ActionEvent ev) {
	    mapView.startAddingEntity(entityType, entitySubtype);
	}
    }
}
