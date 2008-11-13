package zsdx.gui.edit_entities;

import java.awt.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit an exit.
 */
public class EditTeletransporterComponent extends EditEntityComponent {

    // specific fields of an exit
    private TransitionChooser transitionField;
    private ResourceChooser mapField;
    private EntityChooser entranceField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditTeletransporterComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {
	
	// transition
	transitionField = new TransitionChooser();
	addField("Transition", transitionField);
	
	// destination map
	mapField = new ResourceChooser(ResourceDatabase.RESOURCE_MAP, true);
	addField("Destination map", mapField);
	
	// entrance
	entranceField = new EntityChooser(null, MapEntity.ENTITY_DESTINATION_POINT, true);
	addField("DestinationPoint", entranceField);
	
	// load the entrance list for the selected map
	mapField.addActionListener(new ActionListenerChangeDestinationMap());
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	Teletransporter teletransporter = (Teletransporter) entity;
	
	transitionField.setTransition(teletransporter.getTransition());
	mapField.setSelectedId(teletransporter.getDestinationMapId());
	entranceField.setSelectedName(teletransporter.getDestinationPointName());
    }

    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     * @throws ZSDXException if the destination map or the entrance name are left blank
     */
    protected ActionEditEntity getAction() throws ZSDXException {
	
	// retrieve the action corresponding to the common entity properties
	ActionEditEntity action = super.getAction();

	// add the properties specific to an exit
	Teletransporter teletransporter = (Teletransporter) entity;
	
	int transition = transitionField.getTransition();
	String destinationMapId = mapField.getSelectedId();
	String entranceName = entranceField.getSelectedName();
	
	if (destinationMapId.length() == 0) {
	    throw new ZSDXException("Please select a destination map");
	}

	if (entranceName.length() == 0) {
	    throw new ZSDXException("Please select an entrance on the destination map");
	}
	
	action.setSpecificAction(new ActionEditExit(map, teletransporter, transition, destinationMapId, entranceName));
	
	return action;
    }

    /**
     * Action performed when the destination map field is changed.
     * The entrance field is reloaded to show the entrances of this map.
     */
    private class ActionListenerChangeDestinationMap implements ActionListener {

	public void actionPerformed(ActionEvent ev) {
	    
	    try {
		String mapId = mapField.getSelectedId();
		
		if (mapId.length() != 0) { // a map has just been selected
		    entranceField.setMap(new Map(mapId));
		}
		else {
		    entranceField.setMap(null);
		}
	    }
	    catch (ZSDXException ex) {
		GuiTools.warningDialog("The destination map is invalid: " + ex.getMessage());
	    }
	}
    }
}
