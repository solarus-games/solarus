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
