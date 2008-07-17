package zsdx.map_editor_actions.edit_entities;

import zsdx.*;

/**
 * Editing the properties specific to a pickable item:
 * the pickable item type and the unique id.
 */
public class ActionEditPickableItem extends MapEditorAction {
    
    private PickableItem pickableItem;
    
    private int typeBefore;
    private int typeAfter;
    
    private int uniqueIdBefore;
    private int uniqueIdAfter;
    
    /**
     * Constructor.
     * @param map the map
     * @param pickableItem the pickable item to edit
     * @param type the type of pickable item
     * @param uniqueId the unique id of the pickable item
     */
    public ActionEditPickableItem(Map map, PickableItem pickableItem, int type, int uniqueId) {
	super(map);
	
	typeBefore = pickableItem.getPickableItemType();
	typeAfter = type;
	
	uniqueIdBefore = pickableItem.getUniqueId();
	uniqueIdAfter = uniqueId;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	pickableItem.setPickableItem(typeAfter, uniqueIdAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	pickableItem.setPickableItem(typeBefore, uniqueIdBefore);
    }

}
