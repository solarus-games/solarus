package zsdx.gui.edit_entities;

import java.awt.event.*;
import javax.swing.*;
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
    private JCheckBox isVisibleField;

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
	
	// visibility
	isVisibleField = new JCheckBox();
	addField("Visible", isVisibleField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	DestinationPoint destinationPoint = (DestinationPoint) entity;
	
	isVisibleField.setSelected(destinationPoint.isVisible());
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
	
	boolean isVisible = destinationPoint.isVisible();

	action.setSpecificAction(new ActionEditDestinationPoint(map, destinationPoint, isVisible));
	
	return action;
    }

}
