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
import org.solarus.editor.entities.PickableItem.Subtype;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a pickable item.
 */
public class EditPickableItemComponent extends EditEntityComponent {

    // specific fields of a pickable item
    private JCheckBox saveField;
    private NumberChooser savegameVariableField; // enabled only for certain types of pickable items

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditPickableItemComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// saving option
	saveField = new JCheckBox("Save the item state");
	addField("Savegame", saveField);

	// savegame variable
	savegameVariableField = new NumberChooser(0, 0, 32767);
	addField("Savegame variable", savegameVariableField);

	// enable or disable the 'savegame variable' field depending on the check box and the subtype 
	saveField.addChangeListener(new ChangeListener() {
	    public void stateChanged(ChangeEvent ev) {
		savegameVariableField.setEnabled(saveField.isSelected());
	    }
	});

	subtypeField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		if (((Subtype) subtypeField.getValue()).mustBeSaved()) {
		  savegameVariableField.setEnabled(true);
		  saveField.setEnabled(false);
		  saveField.setSelected(true);
		}
		else {
		  saveField.setEnabled(true);
		  savegameVariableField.setEnabled(true);
		}
	    }
	});

	// remove 'Random' and 'None'
	subtypeField.removeItemAt(0);
	subtypeField.removeItemAt(0);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	PickableItem pickableItem = (PickableItem) entity;
	Subtype subtype = (Subtype) pickableItem.getSubtype();

	int savegameVariable = pickableItem.getIntegerProperty("savegameVariable");
	if (savegameVariable != -1) {
	  savegameVariableField.setNumber(savegameVariable);
	  savegameVariableField.setEnabled(true);
	  saveField.setSelected(true);
	}
	else {
	  savegameVariableField.setEnabled(false);
	  saveField.setSelected(false);
	}

	saveField.setEnabled(!subtype.mustBeSaved());
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

	int savegameVariable = savegameVariableField.isEnabled() ? 
		savegameVariableField.getNumber() : -1;

	return new ActionEditEntitySpecific(entity, savegameVariable);
    }
}
