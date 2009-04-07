/*
 * Copyright (C) 2009 Christopho, Zelda Solarus - http://www.zelda-solarus.com
 * 
 * Zelda: Mystery of Solarus DX is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * Zelda: Mystery of Solarus DX is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program. If not, see <http://www.gnu.org/licenses/>.
 */
package zsdx.gui;

import java.awt.*;
import javax.swing.*;
import java.lang.reflect.*;
import zsdx.*;
import zsdx.map_editor_actions.*;
import zsdx.entities.*;
import zsdx.gui.edit_entities.*;

/**
 * A component to edit a map entity.
 */
public class EditEntityComponent extends JPanel {

    private static final Class<?>[] editEntityComponentClasses = {
	EditEntityComponent.class,
	EditDestinationPointComponent.class,
	EditTeletransporterComponent.class,
	EditPickableItemComponent.class,
	EditDestructibleItemComponent.class,
	EditChestComponent.class,
	EditJumpSensorComponent.class,
	EditEnemyComponent.class,
	EditInteractiveEntityComponent.class,
	EditBlockComponent.class,
	EditDynamicTileComponent.class,
	EditSwitchComponent.class,
	EditCustomObstacleComponent.class,
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
    protected JTextField nameField;
    protected EnumerationChooser<Layer> layerField;
    protected CoordinatesField positionField;
    protected CoordinatesField sizeField;
    protected DirectionChooser directionField;
    protected EntitySubtypeChooser subtypeField;

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
		entity.getType().getName() + " properties"));

	gridBagConstraints = new GridBagConstraints();
	gridBagConstraints.insets = new Insets(5, 5, 5, 5); // margins
	gridBagConstraints.anchor = GridBagConstraints.LINE_START;
	gridBagConstraints.gridy = 0;

	// name
	if (entity.hasName()) {
	    nameField = new JTextField(15);
	    addField("Name", nameField);
	}

	// layer
	layerField = new EnumerationChooser<Layer>(Layer.class);
	addField("Layer", layerField);

	// position
	this.positionField = new CoordinatesField();
	positionField.setStepSize(8, 8);
	positionField.setEnabled(true);
	addField("Position", positionField);

	// size
	if (entity.isResizable()) {
	    this.sizeField = new CoordinatesField();
	    sizeField.setEnabled(true);
	    addField("Size", sizeField);
	}

	// direction
	if (entity.hasDirection()) {
	    this.directionField = new DirectionChooser(entity.getNbDirections());
	    addField("Direction", directionField);
	}

	// subtype
	if (entity.hasSubtype()) {
	    this.subtypeField = new EntitySubtypeChooser(entity.getType());
	    addField(entity.getType().getName() + " subtype", subtypeField);
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

	Class<?> componentClass = editEntityComponentClasses[entity.getType().getIndex()];
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
	    nameField.setText(entity.getName());
	}

	layerField.setValue(entity.getLayer());

	positionField.setCoordinates(entity.getX(), entity.getY());

	if (entity.isResizable()) {
	    sizeField.setStepSize(entity.getUnitarySize().width, entity.getUnitarySize().height);
	    sizeField.setCoordinates(entity.getWidth(), entity.getHeight());
	}

	if (entity.hasDirection()) {
	    directionField.setDirection(entity.getDirection());
	}

	if (entity.hasSubtype()) {
	    subtypeField.setValue(entity.getSubtype());
	}
    }

    /**
     * Returns the specific part of the action made on the entity.
     */
    protected ActionEditEntitySpecific getSpecificAction() {
	return null;
    }

    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     * @throws ZSDXException if the action could not be created (typically because
     * some fields are left blank)
     */
    private ActionEditEntity getAction() throws ZSDXException {

	String name = entity.hasName() ? nameField.getText() : null;
	Layer layer = layerField.getValue();
	Point position = positionField.getCoordinates();
	Dimension size = null;
	if (entity.isResizable()) {
	    Point coords = sizeField.getCoordinates();
	    size = new Dimension(coords.x, coords.y);
	}
	int direction = entity.hasDirection() ? directionField.getDirection() : -1;
	EntitySubtype subtype = entity.hasSubtype() ? subtypeField.getValue() : null;

	ActionEditEntitySpecific specificAction = getSpecificAction();

	return new ActionEditEntity(map, entity, name,
		layer, position, size, direction, subtype, specificAction);
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

	    try {
		action.undo(); // undo the action because it may be partially done
	    }
	    catch (ZSDXException ex2) {
		// this is not supposed to happen
		System.err.println("Unexpected error: could not undo the action: " + ex2.getMessage());
		ex2.printStackTrace();
		System.exit(1);
	    }
	    throw ex; // throw the exception again
	}
    }
}
