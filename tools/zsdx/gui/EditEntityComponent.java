package zsdx.gui;

import java.awt.*;
import javax.swing.*;

import zsdx.*;

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

    // subcomponents
    private JTextField fieldName;
    private DirectionChooser fieldDirection;
    private LayerChooser fieldLayer;
    private CoordinatesField fieldPosition;
    private CoordinatesField fieldSize;
    private JComponent fieldSpecific;

    private static final Class<?>[] subComponentClasses = {
	// make a subpackage
	null,                               // ENTITY_TILE
	null,                               // ENTITY_ENTRANCE
	null /* EditExitComponent.class */  // ENTITY_EXIT: destination map and entrance
    };

    private static final Class<?>[] specificActionClasses = {
	// make a subpackage
	null,                            // ENTITY_TILE
	null,                            // ENTITY_ENTRANCE
	null /* ActionEditExit.class */  // ENTITY_EXIT: destination map and entrance
    };

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit, or null to create a new entity
     */
    public EditEntityComponent(Map map, MapEntity entity) {
	super(new GridBagLayout());
	this.map = map;
	this.entity = entity;
	
	setBorder(BorderFactory.createTitledBorder("Entity properties"));

	GridBagConstraints constraints = new GridBagConstraints();
	constraints.insets = new Insets(5, 5, 5, 5); // margins
	constraints.anchor = GridBagConstraints.LINE_START;
	constraints.gridy = 0;
	
	// name
	if (entity.hasName()) {
	    this.fieldName = new JTextField(15);
	    constraints.gridx = 0;
	    add(new JLabel("Name"), constraints);
	    constraints.gridx = 1;
	    add(fieldName, constraints);
	    constraints.gridy++;
	}
	
	// direction
	if (entity.hasDirection()) {
	    this.fieldDirection = new DirectionChooser(entity.getNbDirections());
	    constraints.gridx = 0;
	    add(new JLabel("Direction"), constraints);
	    constraints.gridx = 1;
	    add(fieldDirection, constraints);
	    constraints.gridy++;
	}
	
	// layer
	this.fieldLayer = new LayerChooser();
	constraints.gridx = 0;
	add(new JLabel("Layer"), constraints);
	constraints.gridx = 1;
	add(fieldLayer, constraints);
	constraints.gridy++;
	
	// position
	this.fieldPosition = new CoordinatesField(false);
	fieldPosition.setEnabled(true);
	constraints.gridx = 0;
	add(new JLabel("Position"), constraints);
	constraints.gridx = 1;
	add(fieldPosition, constraints);
	constraints.gridy++;

	// size
	if (entity.isResizable()) {
	    this.fieldSize = new CoordinatesField(false);
	    fieldSize.setEnabled(true);
	    constraints.gridx = 0;
	    add(new JLabel("Size"), constraints);
	    constraints.gridx = 1;
	    add(fieldSize, constraints);
	    constraints.gridy++;
	}

	update();
    }

    /**
     * Updates the information in the fields.
     */
    public void update() {
/*
	    private JTextField fieldName;
	    private DirectionChooser fieldDirection;
	    private LayerChooser fieldLayer;
	    private CoordinatesField fieldPosition;
	    private CoordinatesField fieldSize;
	    private JComponent fieldSpecific;
	    */
	if (entity.hasName()) {
	    fieldName.setText(entity.getName());
	}
	
	if (entity.hasDirection()) {
	    fieldDirection.setDirection(entity.getDirection());
	}
	
	fieldLayer.setLayer(entity.getLayer());
	fieldPosition.setCoordinates(entity.getX(), entity.getY());
	
	if (entity.isResizable()) {
	    fieldSize.setCoordinates(entity.getWidth(), entity.getHeight());
	}
    }

    /**
     * Applies the modifications to the entity.
     */
    public void applyModifications() {
	// TODO: handle undo/redo
	// => make an ActionEditEntity class which edits all common properties of an entity
	// this class take as parameter a specific action class
	// or: the specific action classes are subclasses of ActionEditEntity

    }
}
