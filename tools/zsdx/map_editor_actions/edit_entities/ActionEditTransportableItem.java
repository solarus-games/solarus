package zsdx.map_editor_actions.edit_entities;

import zsdx.*;

/**
 * Editing the properties specific to a transportable item:
 * the transportable item type and the pickable item associated to it.
 */
public class ActionEditTransportableItem extends MapEditorAction {

    private TransportableItem transportableItem;

    private int typeBefore;
    private int typeAfter;
    
    private int pickableItemTypeBefore;
    private int pickableItemTypeAfter;

    private int pickableItemUniqueIdBefore;
    private int pickableItemUniqueIdAfter;

    /**
     * Constructor.
     * @param map the map
     * @param transportableItem the transportable item to edit
     * @param pickableItemType the type of pickable item under this transportable item
     * @param pickableItemUniqueId the unique id of the pickable item
     */
    public ActionEditTransportableItem(Map map, TransportableItem transportableItem,
	    int type, int pickableItemType, int pickableItemUniqueId) {
	super(map);
	
	this.transportableItem = transportableItem;

	typeBefore = transportableItem.getTransportableItemType();
	typeAfter = type;

	pickableItemTypeBefore = transportableItem.getPickableItemType();
	pickableItemTypeAfter = pickableItemType;
	
	pickableItemUniqueIdBefore = transportableItem.getPickableItemUniqueId();
	pickableItemUniqueIdAfter = pickableItemUniqueId;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	transportableItem.setTransportableItemType(typeAfter);
	transportableItem.setPickableItem(pickableItemTypeAfter, pickableItemUniqueIdAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	transportableItem.setTransportableItemType(typeBefore);
	transportableItem.setPickableItem(pickableItemTypeBefore, pickableItemUniqueIdBefore);
    }

}
