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

import javax.swing.*;
import java.awt.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.entities.DestinationPoint.Subtype;
import zsdx.gui.*;

/**
 * A component to edit a destination point.
 */
public class EditDestinationPointComponent extends EditEntityComponent {

    // specific fields of a destination point
    private JCheckBox changeDirectionField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditDestinationPointComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// change the direction
	changeDirectionField = new JCheckBox("Change hero's direction");
	addField("", changeDirectionField);
	changeDirectionField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		directionField.setEnabled(changeDirectionField.isSelected());
	    }
	});

	// subtype
	subtypeField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		boolean changeDirection = (subtypeField.getValue() == Subtype.INVISIBLE);
		changeDirectionField.setSelected(changeDirection);
		directionField.setEnabled(changeDirection);
	    }
	});
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	DestinationPoint destinationPoint = (DestinationPoint) entity;

	boolean changeDirection = destinationPoint.getDirection() != -1;
	directionField.setEnabled(changeDirection);
	changeDirectionField.setSelected(changeDirection);
    }
}
