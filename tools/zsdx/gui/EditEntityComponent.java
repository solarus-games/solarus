package zsdx.gui;

import java.awt.*;
import javax.swing.*;
import java.lang.reflect.*;

import zsdx.*;
import zsdx.map_editor_actions.*;
import zsdx.gui.edit_entities.*;

/**
 * A component to edit a map entity.
 */
public class EditEntityComponent extends JPanel {

    /**
     * The map.
     */
    private Map map;

    /**
     * The map entity to edit.
     */
    private MapEntity entity;

    // common subcomponents
    private JTextField fieldName;
    private LayerChooser fieldLayer;
    private CoordinatesField fieldPosition;
    private CoordinatesField fieldSize;
    private DirectionChooser fieldDirection;
    
    private GridBagConstraints gridBagConstraints;

    private static final Class<?>[] editEntityComponentClasses = {
	// make a subpackage
	EditEntityComponent.class,          // ENTITY_TILE
	EditEntityComponent.class,          // ENTITY_ENTRANCE
	EditExitComponent.class             // ENTITY_EXIT
    };

    // TODO new entity?
    
    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit, or null to create a new entity
     */
    protected EditEntityComponent(Map map, MapEntity entity) {
	super(new GridBagLayout());
	this.map = map;
	this.entity = entity;
	
	setBorder(BorderFactory.createTitledBorder("Entity properties"));

	gridBagConstraints = new GridBagConstraints();
	gridBagConstraints.insets = new Insets(5, 5, 5, 5); // margins
	gridBagConstraints.anchor = GridBagConstraints.LINE_START;
	gridBagConstraints.gridy = 0;
	
	// name
	if (entity.hasName()) {
	    fieldName = new JTextField(15);
	    addField("Name", fieldName);
	}
	
	// layer
	fieldLayer = new LayerChooser();
	addField("Layer", fieldLayer);
	
	// position
	this.fieldPosition = new CoordinatesField(false);
	fieldPosition.setEnabled(true);
	addField("Position", fieldPosition);

	// size
	if (entity.isResizable()) {
	    this.fieldSize = new CoordinatesField(false);
	    fieldSize.setEnabled(true);
	    addField("Size", fieldSize);
	}

	// direction
	if (entity.hasDirection()) {
	    this.fieldDirection = new DirectionChooser(entity.getNbDirections());
	    addField("Direction", fieldDirection);
	}
	
	update();
    }

    /**
     * Creates a component adapted to edit a given entity.
     * The dynamic type of the component returned depends on the type of entity.
     * It can be EditEntityComponent, or a subclass of it if the entity has
     * specific information to edit. 
     * @param map the map
     * @param entity the entity to edit
     * @return the component to edit this entity
     */
    public static EditEntityComponent create(Map map, MapEntity entity) {
	
	EditEntityComponent component = null;
	
	Class<?> componentClass = editEntityComponentClasses[entity.getType()];
	Constructor<?> constructor;
	try {
	    constructor = componentClass.getConstructor(Map.class, MapEntity.class);
	    component = (EditEntityComponent) constructor.newInstance(map, entity);
	}
	catch (Exception ex) {
	    System.err.println("Unexpected error: " + ex.getMessage());
	    System.exit(1);
	}
	
	return component;
    }
    
    /**
     * Adds a field in the component.
     * @param label name displayed of the field
     * @param field the field to add (can be a JTextField, a JComboBox, etc.)
     */
    protected void addField(String label, JComponent field) {

	gridBagConstraints.gridx = 0;
	add(new JLabel(label), gridBagConstraints);
	gridBagConstraints.gridx = 1;
	add(field, gridBagConstraints);
	gridBagConstraints.gridy++;
    }
    
    /**
     * Updates the common information displayed in the fields.
     * The subclasses should redefine this method to display their
     * specific information.
     */
    public void update() {
	
	if (entity.hasName()) {
	    fieldName.setText(entity.getName());
	}
	
	fieldLayer.setLayer(entity.getLayer());
	fieldPosition.setCoordinates(entity.getX(), entity.getY());
	
	if (entity.isResizable()) {
	    fieldSize.setCoordinates(entity.getWidth(), entity.getHeight());
	}
	
	if (entity.hasDirection()) {
	    fieldDirection.setDirection(entity.getDirection());
	}	
    }
    
    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * The subclasses should redefine this method to add their specific handling.
     * @return the action object corresponding to the modifications made
     */
    protected ActionEditEntity getAction() {

	String name = entity.hasName() ? entity.getName() : null;
	int layer = entity.getLayer();
	Dimension position = new Dimension(entity.getX(), entity.getY());
	Dimension size = entity.isResizable() ? entity.getSize() : null;
	int direction = entity.hasDirection() ? entity.getDirection() : -1;
	
	return new ActionEditEntity(map, entity, name,
		layer, position, size, direction);
	
    }

    /**
     * Applies the modifications to the entity.
     * This function calls getAction() and executes it.
     * @throws ZSDXException if the modifications are invalid
     */
    public final void applyModifications() throws ZSDXException {
	
	map.getHistory().doAction(getAction());
    }
}
