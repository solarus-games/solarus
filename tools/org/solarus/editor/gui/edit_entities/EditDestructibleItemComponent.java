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
 * A component to edit a destructible item.
 */
public class EditDestructibleItemComponent extends EditEntityComponent {

    // specific fields of a destructible item
    private PickableItemSubtypeChooser pickableItemSubtypeField;
    private JCheckBox pickableItemSaveField;
    private NumberChooser pickableItemSavegameVariableField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditDestructibleItemComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// pickable item type
	pickableItemSubtypeField = new PickableItemSubtypeChooser(true);
	addField("Pickable item", pickableItemSubtypeField);

	// pickable item saving option
	pickableItemSaveField = new JCheckBox("Save the pickable item state");
	addField("Savegame", pickableItemSaveField);

	// pickable item savegame variable
	pickableItemSavegameVariableField = new NumberChooser(0, 0, 32767);
	addField("Pickable item savegame variable", pickableItemSavegameVariableField);

	// enable or disable the 'pickable item savegame variable' field depending on the checkbox and the subtype
	pickableItemSaveField.addChangeListener(new ChangeListener() {
	    public void stateChanged(ChangeEvent ev) {
		pickableItemSavegameVariableField.setEnabled(pickableItemSaveField.isSelected());
	    }
	});

	pickableItemSubtypeField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {

	        PickableItem.Subtype pickableItemSubtype = (PickableItem.Subtype) pickableItemSubtypeField.getValue();
		if (pickableItemSubtype.mustBeSaved()) {
		  pickableItemSavegameVariableField.setEnabled(true);
		  pickableItemSaveField.setEnabled(false);
		  pickableItemSaveField.setSelected(true);
		}
		else if (!pickableItemSubtype.canBeSaved()) {
		  pickableItemSaveField.setEnabled(false);
		  pickableItemSavegameVariableField.setEnabled(false);
		  pickableItemSaveField.setSelected(false);
		}
		else {
		  pickableItemSaveField.setEnabled(true);
		}
	    }
	});
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	DestructibleItem destructibleItem = (DestructibleItem) entity;

	PickableItem.Subtype pickableItemSubtype = PickableItem.Subtype.get(destructibleItem.getIntegerProperty("pickableItemSubtype"));
	pickableItemSubtypeField.setValue(pickableItemSubtype);
	int pickableItemSavegameVariable = destructibleItem.getIntegerProperty("pickableItemSavegameVariable");
	if (pickableItemSavegameVariable != -1) {
	  pickableItemSavegameVariableField.setNumber(pickableItemSavegameVariable);
	  pickableItemSavegameVariableField.setEnabled(true);
	  pickableItemSaveField.setSelected(true);
	}
	else {
	  pickableItemSavegameVariableField.setEnabled(false);
	  pickableItemSaveField.setSelected(false);
	}

	pickableItemSaveField.setEnabled(pickableItemSubtype.canBeSaved() && !pickableItemSubtype.mustBeSaved());
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

	int pickableItemSavegameVariable = pickableItemSavegameVariableField.isEnabled() ? 
		pickableItemSavegameVariableField.getNumber() : -1;

	return new ActionEditEntitySpecific(entity,
		pickableItemSubtypeField.getValue().getId(),
		pickableItemSavegameVariable);
    }
}

