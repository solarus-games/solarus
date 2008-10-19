package zsdx.gui.edit_entities;

import java.awt.event.*;
import zsdx.*;
import zsdx.entities.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit a pickable item.
 */
public class EditPickableItemComponent extends EditEntityComponent {
    
    // specific fields of a pickable item
    private PickableItemSubtypeChooser typeField;
    private NumberChooser savegameIndexField; // enabled only for certain types of pickable items

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditPickableItemComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// pickable item type
	typeField = new PickableItemSubtypeChooser(false);
	addField("Pickable item type", typeField);

	// savegame index
	savegameIndexField = new NumberChooser(0, 0, 32767);
	addField("Savegame variable", savegameIndexField);

	// enable or disable the 'savegame index' field depending on the pickable item type
	typeField.addActionListener(new ActionListenerEnableSavegameIndex());
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	PickableItem pickableItem = (PickableItem) entity;

	typeField.setPickableItemSubtype(pickableItem.getPickableItemType());
	savegameIndexField.setNumber(pickableItem.getSavegameIndex());
	new ActionListenerEnableSavegameIndex().actionPerformed(null);
    }

    /**
     * Creates the map editor action object which corresponds
     * to the modifications indicated in the fields.
     * @return the action object corresponding to the modifications made
     * @throws ZSDXException if the fields are incorrectly filled
     */
    protected ActionEditEntity getAction() throws ZSDXException {

	// retrieve the action corresponding to the common entity properties
	ActionEditEntity action = super.getAction();

	// add the properties specific to a pickable item
	PickableItem pickableItem = (PickableItem) entity;

	int type = typeField.getPickableItemType();
	int savegameIndex = savegameIndexField.getNumber();

	action.setSpecificAction(new ActionEditPickableItem(map, pickableItem, type, savegameIndex));

	return action;
    }

    /**
     * A listener associated to the 'pickable item type' field,
     * to enable or disable the 'savegame index' field depending on the type.
     */
    private class ActionListenerEnableSavegameIndex implements ActionListener {

	public void actionPerformed(ActionEvent ev) {
	    int type = typeField.getPickableItemType();
	    if (type <= PickableItem.PICKABLE_ITEM_ARROW_10) {
		savegameIndexField.setEnabled(false);
	    }
	    else {
		savegameIndexField.setEnabled(true);
	    }
	}
    }
}
