package zsdx.gui.edit_entities;

import javax.swing.*;
import java.awt.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit a destination point.
 */
public class EditDestinationPointComponent extends EditEntityComponent {

    // specific fields of a destination point
    private JCheckBox changeDirectionField;
    private RadioField subtypeField;

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
	subtypeField = new RadioField("Invisible", "Teletransporter destination");
	addField("Subtype", subtypeField);
	subtypeField.addActionListener(new ActionListener() {
	    public void actionPerformed(ActionEvent ev) {
		boolean changeDirection = (subtypeField.getSelectedIndex() == DestinationPoint.INVISIBLE);
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
	subtypeField.setSelectedIndex(destinationPoint.getSubtypeIndex());
    }

    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     */
    protected ActionEditEntity getAction() throws ZSDXException {

	// retrieve the action corresponding to the common entity properties
	ActionEditEntity action = super.getAction();

	// add the properties specific to a destination point
	DestinationPoint destinationPoint = (DestinationPoint) entity;

	int subtype = subtypeField.getSelectedIndex();

	action.setSpecificAction(new ActionEditDestinationPoint(map, destinationPoint, subtype));

	return action;
    }
}
