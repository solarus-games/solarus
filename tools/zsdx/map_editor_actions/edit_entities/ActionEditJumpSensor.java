package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the properties specific to a jump sensor.
 */
public class ActionEditJumpSensor extends MapEditorAction {

    private JumpSensor jumpSensor;

    private int jumpLengthBefore;
    private int jumpLengthAfter;

    /**
     * Constructor.
     * @param map the map
     * @param jumpSensor the entity edited
     * @param jumpLength length of the jump
     */
    public ActionEditJumpSensor(Map map, JumpSensor jumpSensor, int jumpLength) {
	super(map);
	
	this.jumpSensor = jumpSensor;
	
	this.jumpLengthBefore = jumpSensor.getJumpLength();
	this.jumpLengthAfter = jumpLength;
    }


    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	jumpSensor.setJumpLength(jumpLengthAfter);
    }
    
    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	jumpSensor.setJumpLength(jumpLengthBefore);
    }
}
