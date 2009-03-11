package zsdx.gui.edit_entities;

import javax.swing.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;

/**
 * A component to edit a dynamic tile.
 */
public class EditDynamicTileComponent extends EditEntityComponent {

    // specific fields of a dynamic tile
    private JCheckBox enabledField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditDynamicTileComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// treasure saving option
	enabledField = new JCheckBox("Visible at start");
	enabledField.setSelected(true);
	addField("Visibility", enabledField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	DynamicTile dynamicTile = (DynamicTile) entity;

	enabledField.setSelected(dynamicTile.getBooleanProperty("enabled"));
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {
	return new ActionEditEntitySpecific(entity,
		((Tile) entity).getTilePatternId(),
		enabledField.isSelected() ? 1 : 0);
    }
}
