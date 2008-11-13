package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the properties specific to an teletransporter:
 * the destination map, the name of the entrance on that map and the transition.
 */
public class ActionEditExit extends MapEditorAction {
    
    private Teletransporter teletransporter;
    
    private int transitionBefore;
    private int transitionAfter;
    
    private String destinationMapIdBefore;
    private String destinationMapIdAfter;
    
    private String entranceNameBefore;
    private String entranceNameAfter;

    /**
     * Constructor.
     * @param map the map
     * @param teletransporter the teletransporter edited
     * @param transition type of transition between the two maps
     * @param destinationMapId the id of the destination map
     * @param entranceName the entrance on the destination map
     */
    public ActionEditExit(Map map, Teletransporter teletransporter, int transition, String destinationMapId, String entranceName) {
	super(map);
	
	this.teletransporter = teletransporter;
	
	this.transitionBefore = teletransporter.getTransition();
	this.transitionAfter = transition;
	
	this.destinationMapIdBefore = teletransporter.getDestinationMapId();
	this.destinationMapIdAfter = destinationMapId;
	
	this.entranceNameBefore = teletransporter.getDestinationPointName();
	this.entranceNameAfter = entranceName;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	teletransporter.setTransition(transitionAfter);
	teletransporter.setDestinationMapId(destinationMapIdAfter);
	teletransporter.setDestinationPointName(entranceNameAfter);
    }
    
    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	teletransporter.setTransition(transitionBefore);
	teletransporter.setDestinationMapId(destinationMapIdBefore);
	teletransporter.setDestinationPointName(entranceNameBefore);
    }
}
