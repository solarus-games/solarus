package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the properties specific to a pickable item:
 * the pickable item type and the savegame index.
 */
public class ActionEditPickableItem extends MapEditorAction {
    
    private PickableItem pickableItem;
    
    private int typeBefore;
    private int typeAfter;
    
    private int savegameIndexBefore;
    private int savegameIndexAfter;
    
    /**
     * Constructor.
     * @param map the map
     * @param pickableItem the pickable item to edit
     * @param type the type of pickable item
     * @param savegameIndex the savegame index where the pickable item is saved
     */
    public ActionEditPickableItem(Map map, PickableItem pickableItem, int type, int savegameIndex) {
	super(map);
	
	this.pickableItem = pickableItem;
	
	typeBefore = pickableItem.getPickableItemType();
	typeAfter = type;
	
	savegameIndexBefore = pickableItem.getSavegameIndex();
	savegameIndexAfter = savegameIndex;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	pickableItem.setPickableItem(typeAfter, savegameIndexAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	pickableItem.setPickableItem(typeBefore, savegameIndexBefore);
    }

}
