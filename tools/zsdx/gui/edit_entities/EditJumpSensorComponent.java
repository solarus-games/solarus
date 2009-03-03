package zsdx.gui.edit_entities;

import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;

/**
 * A component to edit a jump sensor.
 */
public class EditJumpSensorComponent extends EditEntityComponent {

    // specific fields of a jump sensor
    private NumberChooser jumpLengthField;
    
    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditJumpSensorComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// jump length
	jumpLengthField = new NumberChooser(56, JumpSensor.MINIMUM_JUMP_LENGTH, 800);
	jumpLengthField.setStepSize(8);
	addField("Jump length", jumpLengthField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	JumpSensor jumpSensor = (JumpSensor) entity;

	jumpLengthField.setNumber(jumpSensor.getIntegerProperty("jumpLength"));
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {

	return new ActionEditEntitySpecific(entity, jumpLengthField.getNumber());
    }
}
