package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the properties specific to a destination point.
 */
public class ActionEditDestinationPoint extends MapEditorAction {

    private DestinationPoint destinationPoint;

    private int subtypeBefore;
    private int subtypeAfter;

    /**
     * Constructor.
     * @param map the map
     * @param DestinationPoint the destination point edited
     * @param subtype the subtype of destination point
     */
    public ActionEditDestinationPoint(Map map, DestinationPoint destinationPoint, int subtype) {
	super(map);

	this.destinationPoint = destinationPoint;

	this.subtypeBefore = destinationPoint.getSubtype();
	this.subtypeAfter = subtype;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	destinationPoint.setSubtype(subtypeAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	destinationPoint.setSubtype(subtypeBefore);
    }
}
