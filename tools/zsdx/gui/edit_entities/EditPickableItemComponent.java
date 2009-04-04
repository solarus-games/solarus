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
package zsdx.gui.edit_entities;

import java.awt.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.entities.PickableItem.Subtype;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;

/**
 * A component to edit a pickable item.
 */
public class EditPickableItemComponent extends EditEntityComponent {

    // specific fields of a pickable item
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

	// savegame index
	savegameVariableField = new NumberChooser(0, 0, 32767);
	addField("Savegame variable", savegameVariableField);

	// enable or disable the 'savegame variable' field depending on the pickable item type
	subtypeField.addActionListener(new ActionListenerEnableSavegameVariable());

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

	savegameVariableField.setNumber(pickableItem.getIntegerProperty("savegameVariable"));
	new ActionListenerEnableSavegameVariable().actionPerformed(null);
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

    /**
     * A listener associated to the 'pickable item type' field,
     * to enable or disable the 'savegame index' field depending on the type.
     */
    private class ActionListenerEnableSavegameVariable implements ActionListener {

	public void actionPerformed(ActionEvent ev) {
	    Subtype subtype = (Subtype) subtypeField.getValue();
	    savegameVariableField.setEnabled(subtype.isSaved());
	}
    }
}
