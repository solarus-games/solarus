package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the properties specific to a destination point.
 */
public class ActionEditDestinationPoint extends MapEditorAction {

    private DestinationPoint destinationPoint;

    private boolean isVisibleBefore;
    private boolean isVisibleAfter;

    /**
     * Constructor.
     * @param map the map
     * @param DestinationPoint the destination point edited
     * @param isVisible true if the destination point becomes visible
     */
    public ActionEditDestinationPoint(Map map, DestinationPoint destinationPoint, boolean isVisible) {
	super(map);
	
	this.destinationPoint = destinationPoint;
	
	this.isVisibleBefore = destinationPoint.isVisible();
	this.isVisibleAfter = isVisible;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	destinationPoint.setVisible(isVisibleAfter);
    }
    
    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	destinationPoint.setVisible(isVisibleBefore);
    }
}
