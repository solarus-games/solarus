package zsdx.map_editor_actions.edit_entities;

import zsdx.*;

/**
 * Editing the properties specific to a map exit:
 * the destination map, the name of the entrance on that map and the transition.
 */
public class ActionEditExit extends MapEditorAction {
    
    private MapExit exit;
    
    private int transitionBefore;
    private int transitionAfter;
    
    private String destinationMapIdBefore;
    private String destinationMapIdAfter;
    
    private String entranceNameBefore;
    private String entranceNameAfter;

    /**
     * Constructor.
     * @param map the map
     * @param exit the exit edited
     * @param transition type of transition between the two maps
     * @param destinationMapId the id of the destination map
     * @param entranceName the entrance on the destination map
     */
    public ActionEditExit(Map map, MapExit exit, int transition, String destinationMapId, String entranceName) {
	super(map);
	
	this.exit = exit;
	
	this.transitionBefore = exit.getTransition();
	this.transitionAfter = transition;
	
	this.destinationMapIdBefore = exit.getDestinationMapId();
	this.destinationMapIdAfter = destinationMapId;
	
	this.entranceNameBefore = exit.getEntranceName();
	this.entranceNameAfter = entranceName;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	exit.setTransition(transitionAfter);
	exit.setDestinationMapId(destinationMapIdAfter);
	exit.setEntranceName(entranceNameAfter);
    }
    
    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	exit.setTransition(transitionBefore);
	exit.setDestinationMapId(destinationMapIdBefore);
	exit.setEntranceName(entranceNameBefore);
    }
}
