package zsdx.gui.edit_entities;

import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

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

	jumpLengthField.setNumber(jumpSensor.getJumpLength());
    }
    
    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     * @throws ZSDXException
     */
    protected ActionEditEntity getAction() throws ZSDXException {
	
	// retrieve the action corresponding to the common entity properties
	ActionEditEntity action = super.getAction();

	// add the properties specific to this kind of entity
	JumpSensor jumpSensor = (JumpSensor) entity;
	int jumpLength = jumpLengthField.getNumber();

	action.setSpecificAction(new ActionEditJumpSensor(map, jumpSensor, jumpLength));

	return action;
    }
}
