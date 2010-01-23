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

import java.awt.event.*;
import javax.swing.*;
import javax.swing.event.*;
import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a shop item.
 */
public class EditShopItemComponent extends EditEntityComponent {

    // specific fields of this type of entity
    private EnumerationChooser<TreasureContent> contentField;
    private NumberChooser amountField;
    private JCheckBox saveField;
    private NumberChooser savegameVariableField;
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
	contentField = new EnumerationChooser<TreasureContent>(TreasureContent.class);
	addField("Treasure", contentField);

	// treasure amount
	amountField = new NumberChooser(1, 1, 999);
	addField("Amount", amountField);

	// treasure saving option
	saveField = new JCheckBox("Can be bought only once");
	saveField.setSelected(true);
	addField("Unique", saveField);

	// treasure savegame variable
	savegameVariableField = new NumberChooser(0, 0, 32767);
	addField("Savegame variable", savegameVariableField);

	// price
	priceField = new NumberChooser(10, 1, 999);
	addField("Price", priceField);

	// description message id
	messageIdField = new JTextField(15);
	addField("Description message id", messageIdField);

	// enable or disable the amount field depending on the treasure content
	contentField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {

		boolean treasurePresent = (contentField.getValue() != TreasureContent.NOTHING);
		amountField.setEnabled(treasurePresent && contentField.getValue().hasAmount());
		savegameVariableField.setEnabled(treasurePresent);
	    }
	});

	// enable or disable the savegame variable field depending on the check box
	saveField.addChangeListener(new ChangeListener() {
	    public void stateChanged(ChangeEvent ev) {
		savegameVariableField.setEnabled(saveField.isSelected());
	    }
	});
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	ShopItem shopItem = (ShopItem) entity;

	int savegameVariable = shopItem.getIntegerProperty("savegameVariable");
	contentField.setValue(TreasureContent.get(shopItem.getIntegerProperty("content")));
	amountField.setNumber(shopItem.getIntegerProperty("amount"));
	saveField.setSelected(savegameVariable != -1);
	savegameVariableField.setNumber(savegameVariable);
	savegameVariableField.setEnabled(savegameVariable != -1);
	priceField.setNumber(shopItem.getIntegerProperty("price"));
	messageIdField.setText(shopItem.getProperty("messageId"));
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {
	return new ActionEditEntitySpecific(entity,
		Integer.toString(contentField.getValue().getId()),
		Integer.toString(amountField.getNumber()),
		Integer.toString(saveField.isSelected() ? savegameVariableField.getNumber() : -1),
		Integer.toString(priceField.getNumber()),
		messageIdField.getText());
    }
}
