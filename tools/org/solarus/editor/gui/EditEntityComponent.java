/*
 * Copyright (C) 2006-2012 Christopho, Solarus - http://www.solarus-games.org
 *
 * Solarus Quest Editor is free software; you can redistribute it and/or modify
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
package org.solarus.editor.gui;

import java.awt.*;
import javax.swing.*;
import java.lang.reflect.*;
import java.util.HashMap;
import org.solarus.editor.*;
import org.solarus.editor.map_editor_actions.*;
import org.solarus.editor.entities.*;
import org.solarus.editor.gui.edit_entities.*;

/**
 * A component to edit a map entity.
 */
public class EditEntityComponent extends JPanel {

    /**
     * The component to edit each type of map entity.
     * Each component must be a subclass of EditEntityComponent,
     * or EditEntityComponent itself if this map entity does not introduce
     * specific additional properties.
     */
    private static final HashMap<EntityType, Class<?>> editEntityComponentClasses =
        new HashMap<EntityType, Class<?>>();
    static {
        editEntityComponentClasses.put(EntityType.TILE, EditEntityComponent.class);
        editEntityComponentClasses.put(EntityType.DESTINATION, EditDestinationComponent.class);
        editEntityComponentClasses.put(EntityType.TELETRANSPORTER, EditTeletransporterComponent.class);
        editEntityComponentClasses.put(EntityType.PICKABLE, EditPickableComponent.class);
        editEntityComponentClasses.put(EntityType.DESTRUCTIBLE, EditDestructibleComponent.class);
        editEntityComponentClasses.put(EntityType.CHEST, EditChestComponent.class);
        editEntityComponentClasses.put(EntityType.JUMPER, EditJumperComponent.class);
        editEntityComponentClasses.put(EntityType.ENEMY, EditEnemyComponent.class);
        editEntityComponentClasses.put(EntityType.NPC, EditNPCComponent.class);
        editEntityComponentClasses.put(EntityType.BLOCK, EditBlockComponent.class);
        editEntityComponentClasses.put(EntityType.DYNAMIC_TILE, EditDynamicTileComponent.class);
        editEntityComponentClasses.put(EntityType.SWITCH, EditSwitchComponent.class);
        editEntityComponentClasses.put(EntityType.WALL, EditWallComponent.class);
        editEntityComponentClasses.put(EntityType.SENSOR, EditEntityComponent.class);
        editEntityComponentClasses.put(EntityType.CRYSTAL, EditEntityComponent.class);
        editEntityComponentClasses.put(EntityType.CRYSTAL_BLOCK, EditEntityComponent.class);
        editEntityComponentClasses.put(EntityType.SHOP_ITEM, EditShopItemComponent.class);
        editEntityComponentClasses.put(EntityType.CONVEYOR_BELT, EditEntityComponent.class);
        editEntityComponentClasses.put(EntityType.DOOR, EditDoorComponent.class);
        editEntityComponentClasses.put(EntityType.STAIRS, EditEntityComponent.class);
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
                entity.getType().getHumanName() + " properties"));

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

        // subtype
        if (entity.hasSubtype()) {
            this.subtypeField = new EntitySubtypeChooser(entity.getType());
            //addField(entity.getType().getName() + " subtype", subtypeField);
            addField("Subtype", subtypeField);
        }

        // direction
        if (entity.hasDirectionProperty()) {

            String noDirectionText = null;
            if (entity.canHaveNoDirection()) {
                noDirectionText = entity.getNoDirectionText();
            }
            this.directionField = new DirectionChooser(entity.getNbDirections(), noDirectionText);

            addField("Direction", directionField);
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

        Class<?> componentClass = editEntityComponentClasses.get(entity.getType());
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
     * Adds a field in the component: label and component.
     * @param name Displayed name of the field.
     * @param field The field to add (can be a JTextField, a JComboBox, etc.).
     * @return The JLabel created to display the name.
     */
    protected JLabel addField(String name, JComponent field) {

        JLabel label = new JLabel(name);
        gridBagConstraints.gridx = 0;
        add(label, gridBagConstraints);
        gridBagConstraints.gridx = 1;
        add(field, gridBagConstraints);
        gridBagConstraints.gridy++;

        return label;
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

        if (entity.hasDirectionProperty()) {
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
     * @throws QuestEditorException if the action could not be created (typically because
     * some fields are left blank)
     */
    private ActionEditEntity getAction() throws QuestEditorException {

        String name = entity.hasName() ? nameField.getText() : null;
        Layer layer = layerField.getValue();
        Point position = positionField.getCoordinates();
        Dimension size = null;
        if (entity.isResizable()) {
            Point coords = sizeField.getCoordinates();
            size = new Dimension(coords.x, coords.y);
        }
        int direction = entity.hasDirectionProperty() ? directionField.getDirection() : -1;
        EntitySubtype subtype = entity.hasSubtype() ? subtypeField.getValue() : null;

        ActionEditEntitySpecific specificAction = getSpecificAction();

        return new ActionEditEntity(map, entity, name,
                layer, position, size, direction, subtype, specificAction);
    }

    /**
     * Applies the modifications to the entity.
     * This function calls getAction() and executes it.
     * @throws QuestEditorException if the modifications cannot be applied
     */
    public final void applyModifications() throws QuestEditorException {

        ActionEditEntity action = getAction();
        try {
            map.getHistory().doAction(action);
        }
        catch (QuestEditorException ex) {

            try {
                action.undo(); // undo the action because it may be partially done
            }
            catch (QuestEditorException ex2) {
                // this is not supposed to happen
                System.err.println("Unexpected error: could not undo the action: " + ex2.getMessage());
                ex2.printStackTrace();
                System.exit(1);
            }
            throw ex; // throw the exception again
        }
    }
}

