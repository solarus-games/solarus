package zsdx.gui.edit_entities;

import zsdx.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit a map exit.
 */
public class EditExitComponent extends EditEntityComponent {
    
    private TransitionChooser transitionField;
    private ResourceChooser mapField;
    private EntityChooser entranceField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditExitComponent(Map map, MapEntity entity) {
	super(map, entity);
	
	// transition
	transitionField = new TransitionChooser();
	addField("Transition", transitionField);
	
	// destination map
	mapField = new ResourceChooser(ResourceDatabase.RESOURCE_MAP, true);
	addField("Destination map", mapField);
	
	// entrance
	entranceField = new EntityChooser(map, MapEntity.ENTITY_ENTRANCE, true);
	addField("Entrance", entranceField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	MapExit exit = (MapExit) entity;
	
	transitionField.setTransition(exit.getTransition());
	mapField.setSelectedId(exit.getDestinationMapId());
	entranceField.setSelectedName(exit.getEntranceName());
    }
    
    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     */
    protected ActionEditEntity getAction() {
	
	// retrieve the action corresponding to the common entity properties
	ActionEditEntity action = super.getAction();

	// add the properties specific to a map exit
	MapExit exit = (MapExit) entity;
	
	int transition = transitionField.getTransition();
	String destinationMapId = mapField.getSelectedId();
	String entranceName = entranceField.getSelectedName();
	
	action.setSpecificAction(new ActionEditExit(map, exit, transition, destinationMapId, entranceName));
	
	return action;
    }

}
