package zsdx.map_editor_actions.edit_entities;

import java.awt.Dimension;

import zsdx.*;
import zsdx.map_editor_actions.ActionEditEntity;

/**
 * Editing all properties of a map exit, i.e. the common properties of
 * every entity, and the specific properties of a map exit:
 * the destination map, the name of the entrance on this map and the transition.
 */
public class ActionEditExit extends ActionEditEntity {
    
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
     * @param name the new name of the exit
     * @param layer the new layer of the exit
     * @param position the new position of the exit
     * @param size the new size of the exit
     * @param destinationMapId the id of the destination map
     * @param entranceName the entrance on the destination map
     * @param transition type of transition between the two maps
     */
    public ActionEditExit(Map map, MapExit exit, String name, int layer,
	    Dimension position, Dimension size, int transition,
	    String destinationMapId, String entranceName) {
	super(map, exit, name, layer, position, size, -1);
	
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
	super.execute();
	
	MapExit exit = (MapExit) entity;
	exit.setTransition(transitionAfter);
	exit.setDestinationMapId(destinationMapIdAfter);
	exit.setEntranceName(entranceNameAfter);
    }
    
    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	super.undo();
	
	MapExit exit = (MapExit) entity;
	exit.setTransition(transitionBefore);
	exit.setDestinationMapId(destinationMapIdBefore);
	exit.setEntranceName(entranceNameBefore);
    }
}
