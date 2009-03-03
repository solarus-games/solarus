package zsdx.gui.edit_entities;

import java.awt.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;

/**
 * A component to edit a destructible item.
 */
public class EditDestructibleItemComponent extends EditEntityComponent {

    // specific fields of a destructible item
    private PickableItemSubtypeChooser pickableItemSubtypeField;
    private NumberChooser pickableItemSavegameVariableField;

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditDestructibleItemComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// pickable item type
	pickableItemSubtypeField = new PickableItemSubtypeChooser(true);
	addField("Pickable item", pickableItemSubtypeField);

	// savegame variable
	pickableItemSavegameVariableField = new NumberChooser(0, 0, 32767);
	addField("Pickable item savegame variable", pickableItemSavegameVariableField);

	// enable or disable the 'savegame index' field depending on the pickable item type
	pickableItemSubtypeField.addActionListener(new ActionListenerEnableSavegameVariable());
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	DestructibleItem destructibleItem = (DestructibleItem) entity;

	pickableItemSubtypeField.setValue(PickableItem.Subtype.get(destructibleItem.getIntegerProperty("pickableItemSubtype")));
	pickableItemSavegameVariableField.setNumber(destructibleItem.getIntegerProperty("pickableItemSavegameVariable"));
	new ActionListenerEnableSavegameVariable().actionPerformed(null);
    }

    /**
     * Returns the specific part of the action made on the entity.
     * @return the specific part of the action made on the entity
     */
    protected ActionEditEntitySpecific getSpecificAction() {
	return new ActionEditEntitySpecific(entity,
		pickableItemSubtypeField.getValue().getId(),
		pickableItemSavegameVariableField.getNumber());
    }

    /**
     * A listener associated to the 'pickable item type' field,
     * to enable or disable the 'savegame index' field depending on the type.
     */
    private class ActionListenerEnableSavegameVariable implements ActionListener {

	public void actionPerformed(ActionEvent ev) {

	    PickableItem.Subtype subtype = pickableItemSubtypeField.getValue();
	    pickableItemSavegameVariableField.setEnabled(subtype.isSaved());
	}
    }
}
