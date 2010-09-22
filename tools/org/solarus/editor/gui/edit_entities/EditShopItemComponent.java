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
package org.solarus.editor.gui.edit_entities;

import javax.swing.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a shop item.
 */
public class EditShopItemComponent extends EditEntityComponent {

    // specific fields of this type of entity
    private TreasureChooser treasureField;
    private NumberChooser priceField;
    private JTextField messageIdField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditShopItemComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// treasure content
	treasureField = new TreasureChooser(false, false);
	addField("Treasure", treasureField);

	// price
	priceField = new NumberChooser(10, 1, 999);
	addField("Price", priceField);

	// description message id
	messageIdField = new JTextField(15);
	addField("Description message id", messageIdField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	ShopItem shopItem = (ShopItem) entity;

	treasureField.setTreasure(
		shopItem.getProperty("treasureName"),
		shopItem.getIntegerProperty("treasureVariant"),
		shopItem.getIntegerProperty("treasureSavegameVariable"));
	priceField.setNumber(shopItem.getIntegerProperty("price"));
	messageIdField.setText(shopItem.getProperty("messageId"));
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {
	return new ActionEditEntitySpecific(entity,
		treasureField.getTreasure().getItemName(),
		Integer.toString(treasureField.getTreasure().getVariant()),
		Integer.toString(treasureField.getTreasure().getSavegameVariable()),
		Integer.toString(priceField.getNumber()),
		messageIdField.getText());
    }
}
