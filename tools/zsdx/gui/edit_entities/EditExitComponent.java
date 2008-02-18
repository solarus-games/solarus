package zsdx.gui.edit_entities;

import javax.swing.*;
import zsdx.*;
import zsdx.gui.*;

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
	entranceField = new EntityChooser(map, MapEntity.ENTITY_ENTRANCE);
	addField("Entrance", entranceField);
    }
    
}
