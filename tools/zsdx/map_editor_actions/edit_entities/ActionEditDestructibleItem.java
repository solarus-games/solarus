package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the properties specific to a destructible item:
 * the destructible item type and the pickable item associated to it.
 */
public class ActionEditDestructibleItem extends MapEditorAction {

    private DestructibleItem destructibleItem;

    private int subtypeBefore;
    private int subtypeAfter;
    
    private int pickableItemSubtypeBefore;
    private int pickableItemSubtypeAfter;

    private int pickableItemSavegameVariableBefore;
    private int pickableItemSavegameVariableAfter;

    /**
     * Constructor.
     * @param map the map
     * @param destructibleItem the destructible item to edit
     * @param subtype the subtype of destructible item
     * @param pickableItemSubtype the type of pickable item under this destructible item
     * @param pickableItemSavegameVariable the savegame index where the pickable item is saved
     */
    public ActionEditDestructibleItem(Map map, DestructibleItem destructibleItem,
	    int subtype, int pickableItemSubtype, int pickableItemSavegameVariable) {
	super(map);
	
	this.destructibleItem = destructibleItem;

	subtypeBefore = destructibleItem.getSubtype();
	subtypeAfter = subtype;

	pickableItemSubtypeBefore = destructibleItem.getPickableItemSubtype();
	pickableItemSubtypeAfter = pickableItemSubtype;
	
	pickableItemSavegameVariableBefore = destructibleItem.getPickableItemSavegameVariable();
	pickableItemSavegameVariableAfter = pickableItemSavegameVariable;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	destructibleItem.setSubtype(subtypeAfter);
	destructibleItem.setPickableItem(pickableItemSubtypeAfter, pickableItemSavegameVariableAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	destructibleItem.setSubtype(subtypeBefore);
	destructibleItem.setPickableItem(pickableItemSubtypeBefore, pickableItemSavegameVariableBefore);
    }
}
