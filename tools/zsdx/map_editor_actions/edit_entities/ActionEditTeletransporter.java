package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the properties specific to a teletransporter:
 * its subtype, the destination map, the destination point on that map and the transition.
 */
public class ActionEditTeletransporter extends MapEditorAction {

    private Teletransporter teletransporter;

    private int subtypeBefore;
    private int subtypeAfter;

    private int transitionBefore;
    private int transitionAfter;

    private String destinationMapIdBefore;
    private String destinationMapIdAfter;

    private String destinationPointNameBefore;
    private String destinationPointNameAfter;

    /**
     * Constructor.
     * @param map the map
     * @param teletransporter the teletransporter edited
     * @param subtype the subtype of teletransporter
     * @param transition type of transition when taking the teletransporter
     * @param destinationMapId the id of the destination map
     * @param entranceName the entrance on the destination map
     */
    public ActionEditTeletransporter(Map map, Teletransporter teletransporter,
	    int subtype, int transition, String destinationMapId, String entranceName) {
	super(map);

	this.teletransporter = teletransporter;

	this.subtypeBefore = teletransporter.getSubtype();
	this.subtypeAfter = subtype;

	this.transitionBefore = teletransporter.getTransition();
	this.transitionAfter = transition;

	this.destinationMapIdBefore = teletransporter.getDestinationMapId();
	this.destinationMapIdAfter = destinationMapId;

	this.destinationPointNameBefore = teletransporter.getDestinationPointName();
	this.destinationPointNameAfter = entranceName;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	teletransporter.setSubtype(subtypeAfter);
	teletransporter.setTransition(transitionAfter);
	teletransporter.setDestinationMapId(destinationMapIdAfter);
	teletransporter.setDestinationPointName(destinationPointNameAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	teletransporter.setSubtype(subtypeBefore);
	teletransporter.setTransition(transitionBefore);
	teletransporter.setDestinationMapId(destinationMapIdBefore);
	teletransporter.setDestinationPointName(destinationPointNameBefore);
    }
}
