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

import org.solarus.editor.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.*;
import org.solarus.editor.map_editor_actions.*;

/**
 * A component to edit a jumper.
 */
public class EditJumperComponent extends EditEntityComponent {

    // specific fields of a jumper
    private NumberChooser jumpLengthField;
    
    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditJumperComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// jump length
	jumpLengthField = new NumberChooser(56, Jumper.MINIMUM_JUMP_LENGTH, 800);
	jumpLengthField.setStepSize(8);
	addField("Jump length", jumpLengthField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	Jumper jumper = (Jumper) entity;

	jumpLengthField.setNumber(jumper.getIntegerProperty("jumpLength"));
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

	return new ActionEditEntitySpecific(entity, jumpLengthField.getNumber());
    }
}
