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

import java.awt.event.*;
import javax.swing.*;
import org.solarus.editor.entities.*;

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
		DestinationPoint.Subtype.GRAY
		),
	new ItemDefinition(EntityType.TELETRANSPORTER, KeyEvent.VK_T,
		Teletransporter.Subtype.INVISIBLE,
		Teletransporter.Subtype.YELLOW
		),
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
		DestructibleItem.Subtype.STONE_SMALL_BLACK,
		DestructibleItem.Subtype.BOMB_FLOWER
	),
	new ItemDefinition(EntityType.CHEST, KeyEvent.VK_C),
	new ItemDefinition(EntityType.ENEMY, KeyEvent.VK_E,
	        Enemy.Subtype.values()
	),
	new ItemDefinition(EntityType.INTERACTIVE, KeyEvent.VK_I,
		InteractiveEntity.Subtype.CUSTOM,
		InteractiveEntity.Subtype.NON_PLAYING_CHARACTER,
		InteractiveEntity.Subtype.SIGN,
		InteractiveEntity.Subtype.WATER_FOR_BOTTLE
	),	
	new ItemDefinition(EntityType.BLOCK, KeyEvent.VK_B,
		Block.Subtype.NORMAL_BLOCK,
		Block.Subtype.STATUE
	),
	new ItemDefinition(EntityType.SWITCH, KeyEvent.VK_W,
		Switch.Subtype.INVISIBLE,
		Switch.Subtype.NORMAL
	),
	new ItemDefinition(EntityType.CUSTOM_OBSTACLE, KeyEvent.VK_O),
	new ItemDefinition(EntityType.SENSOR, KeyEvent.VK_N,
		Sensor.Subtype.CUSTOM,
		Sensor.Subtype.CHANGE_LAYER,
		Sensor.Subtype.RETURN_FROM_BAD_GROUND
	),
	new ItemDefinition(EntityType.CRYSTAL_SWITCH, KeyEvent.VK_Y),
	new ItemDefinition(EntityType.CRYSTAL_SWITCH_BLOCK, KeyEvent.VK_N,
		CrystalSwitchBlock.Subtype.ORANGE,
		CrystalSwitchBlock.Subtype.BLUE
	),
	new ItemDefinition(EntityType.SHOP_ITEM, KeyEvent.VK_M),
	new ItemDefinition(EntityType.CONVEYOR_BELT, KeyEvent.VK_V),
	new ItemDefinition(EntityType.DOOR, KeyEvent.VK_D,
                Door.Subtype.CLOSED,
		Door.Subtype.SMALL_KEY,
		Door.Subtype.SMALL_KEY_BLOCK,
		Door.Subtype.BIG_KEY,
		Door.Subtype.BOSS_KEY,
		Door.Subtype.WEAK,
		Door.Subtype.VERY_WEAK,
		Door.Subtype.WEAK_INVISIBLE
	),
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
