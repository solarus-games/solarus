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
import org.solarus.editor.entities.Block.*;
import java.awt.event.*;

/**
 * A component to edit a block.
 */
public class EditBlockComponent extends EditEntityComponent {

    // specific fields of this entity
    private EnumerationChooser<MaximumMoves> maximumMovesField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditBlockComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// maximum moves
	maximumMovesField = new EnumerationChooser<MaximumMoves>(MaximumMoves.class);
	addField("Maximum moves", maximumMovesField);

	subtypeField.addActionListener(new ActionListener() {
		public void actionPerformed(ActionEvent ev) {
		    if (subtypeField.getValue() == Subtype.STATUE) {
			maximumMovesField.setValue(MaximumMoves.INFINITE);
			directionField.setDirection(-1);
		    }
		}
	    });
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	Block block = (Block) entity;
	MaximumMoves maximumMoves = MaximumMoves.get(block.getIntegerProperty("maximumMoves"));
	maximumMovesField.setValue(maximumMoves);
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

	MaximumMoves maximumMoves = maximumMovesField.getValue();
	return new ActionEditEntitySpecific(entity, Integer.toString(maximumMoves.getId()));
    }
}
