package zsdx.gui;

import java.awt.event.*;
import javax.swing.*;

import zsdx.Map;
import zsdx.MapEntity;

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

    /* TODO: subcomponents:
     *  - name
     *  - direction (if any)
     *  - layer
     *  - position
     *  - size (if any)
     *  - entity specific (make a subcomponent for each concrete subclass of MapEntity)
     */

    private JTextField fieldName;
    private JComboBox fieldDirection;
    private JComboBox fieldLayer;

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
	super();
	this.map = map;
	this.entity = entity;

	// TODO: fill the subcomponents
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
