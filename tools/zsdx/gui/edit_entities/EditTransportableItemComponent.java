package zsdx.gui.edit_entities;

import zsdx.*;
import zsdx.gui.*;
import zsdx.map_editor_actions.*;
import zsdx.map_editor_actions.edit_entities.*;

/**
 * A component to edit a transportable item.
 */
public class EditTransportableItemComponent extends EditEntityComponent {

    // specific fields of a transportable item
    private TransportableItemTypeChooser typeField;
    private PickableItemTypeChooser pickableItemTypeField;
    private NumberChooser pickableItemUniqueIdField; // enabled only for certain types of pickable items

    /**
     * Constructor.
     * @param map the map
     * @param entity the entity to edit
     */
    public EditTransportableItemComponent(Map map, MapEntity entity) {
	super(map, entity);
    }

    /**
     * Creates the specific fields for this kind of entity.
     */
    protected void createSpecificFields() {

	// transportable item type
	typeField = new TransportableItemTypeChooser();
	addField("Transportable item type", typeField);

	// pickable item type
	pickableItemTypeField = new PickableItemTypeChooser();
	addField("Pickable item", typeField);

	// unique id
	pickableItemUniqueIdField = new NumberChooser();
	addField("Pickable item unique id", pickableItemUniqueIdField);
    }

    /**
     * Updates the information displayed in the fields.
     */
    public void update() {
	super.update(); // update the common fields

	TransportableItem transportableItem = (TransportableItem) entity;
	
	typeField.setTransportableItemType(transportableItem.getTransportableItemType());
	pickableItemTypeField.setPickableItemType(transportableItem.getPickableItemType());
	pickableItemUniqueIdField.setNumber(transportableItem.getPickableItemUniqueId());
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

	// add the properties specific to a transportable item
	TransportableItem transportableItem = (TransportableItem) entity;

	int type = typeField.getTransportableItemType();
	int pickableItemType = pickableItemTypeField.getPickableItemType();
	int pickableItemUniqueId = pickableItemUniqueIdField.getNumber();

	action.setSpecificAction(new ActionEditTransportableItem(map, transportableItem,
		type, pickableItemType, pickableItemUniqueId));

	return action;
    }

}
