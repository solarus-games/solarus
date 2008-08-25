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

    private static final Class<?>[] editEntityComponentClasses = {
	// make a subpackage
	EditEntityComponent.class,             // ENTITY_TILE
	EditEntityComponent.class,             // ENTITY_ENTRANCE
	EditExitComponent.class,               // ENTITY_EXIT
	EditPickableItemComponent.class,       // ENTITY_PICKABLE_ITEM
	EditTransportableItemComponent.class   // ENTITY_TRANSPORTABLE_ITEM
    };

    /**
     * The map.
     */
    protected Map map;

    /**
     * The map entity to edit.
     */
    protected MapEntity entity;

    // common subcomponents
    private JTextField fieldName;
    private LayerChooser fieldLayer;
    private CoordinatesField fieldPosition;
    private CoordinatesField fieldSize;
    private DirectionChooser fieldDirection;
    
    private GridBagConstraints gridBagConstraints;

    /**
     * Creates a component to edit the common properties of a map entity.
     * If the map entity has specific properties, you should call instead
     * the static method EditEntityComponent.create() which instantiate
     * the specific subclass for this kind of entity.
     * @param map the map
     * @param entity the entity to edit, or null to create a new entity
     */
    public EditEntityComponent(Map map, MapEntity entity) {
	super(new GridBagLayout());
	this.map = map;
	this.entity = entity;
	
	setBorder(BorderFactory.createTitledBorder(
		MapEntity.getTypeName(entity.getType()) + " properties"));

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
	this.fieldPosition = new CoordinatesField();
	fieldPosition.setStepSize(8, 8);
	fieldPosition.setEnabled(true);
	addField("Position", fieldPosition);

	// size
	if (entity.isResizable()) {
	    this.fieldSize = new CoordinatesField();
	    fieldSize.setEnabled(true);
	    addField("Size", fieldSize);
	}

	// direction
	if (entity.hasDirection()) {
	    this.fieldDirection = new DirectionChooser(entity.getNbDirections());
	    addField("Direction", fieldDirection);
	}
	
	// specific fields
	createSpecificFields();
	
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
	catch (InvocationTargetException ex) {
	    throw (RuntimeException) ex.getCause();
	}
	catch (Exception ex) {
	    System.err.println("Cannot create the component to edit this entity: " + ex);
	    ex.printStackTrace();
	    System.exit(1);
	}
	
	return component;
    }
    
    /**
     * Creates the specific fields for this kind of entity.
     * The subclasses should redefine this method to create their specific fields
     * and add them by calling the addField() method.
     * This method is called by the super constructor.
     */
    protected void createSpecificFields() {
	
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
	    fieldSize.setStepSize(entity.getUnitSize().width, entity.getUnitSize().height);
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
     * @throws ZSDXException if the action could not be created (typically because
     * some fields are left blank)
     */
    protected ActionEditEntity getAction() throws ZSDXException {

	String name = entity.hasName() ? fieldName.getText() : null;
	int layer = fieldLayer.getLayer();
	Dimension position = fieldPosition.getCoordinates();
	Dimension size = entity.isResizable() ? fieldSize.getCoordinates() : null;
	int direction = entity.hasDirection() ? fieldDirection.getDirection() : -1;
	
	return new ActionEditEntity(map, entity, name,
		layer, position, size, direction);
	
    }

    /**
     * Applies the modifications to the entity.
     * This function calls getAction() and executes it.
     * @throws ZSDXException if the modifications cannot be applied
     */
    public final void applyModifications() throws ZSDXException {

	ActionEditEntity action = getAction();
	try {
	    map.getHistory().doAction(action);
	}
	catch (ZSDXException ex) {
	    action.undo(); // undo the action because it may be partially done
	    throw ex; // throw the exception again
	}
    }
}
