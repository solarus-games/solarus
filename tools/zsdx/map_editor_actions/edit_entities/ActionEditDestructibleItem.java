package zsdx.map_editor_actions.edit_entities;

import zsdx.*;
import zsdx.entities.*;

/**
 * Editing the properties specific to a destructible item:
 * the destructible item type and the pickable item associated to it.
 */
public class ActionEditDestructibleItem extends MapEditorAction {

    private DestructibleItem destructibleItem;

    private PickableItem.Subtype pickableItemSubtypeBefore;
    private PickableItem.Subtype pickableItemSubtypeAfter;

    private int pickableItemSavegameVariableBefore;
    private int pickableItemSavegameVariableAfter;

    /**
     * Constructor.
     * @param map the map
     * @param destructibleItem the destructible item to edit
     * @param pickableItemSubtype the type of pickable item under this destructible item
     * @param pickableItemSavegameVariable the savegame index where the pickable item is saved
     */
    public ActionEditDestructibleItem(Map map, DestructibleItem destructibleItem,
	    PickableItem.Subtype pickableItemSubtype, int pickableItemSavegameVariable) {
	super(map);

	this.destructibleItem = destructibleItem;

	pickableItemSubtypeBefore = destructibleItem.getPickableItemSubtype();
	pickableItemSubtypeAfter = pickableItemSubtype;

	pickableItemSavegameVariableBefore = destructibleItem.getPickableItemSavegameVariable();
	pickableItemSavegameVariableAfter = pickableItemSavegameVariable;
    }

    /**
     * Executes the action.
     */
    public void execute() throws ZSDXException {
	destructibleItem.setPickableItem(pickableItemSubtypeAfter, pickableItemSavegameVariableAfter);
    }

    /**
     * Undoes the action.
     */
    public void undo() throws ZSDXException {
	destructibleItem.setPickableItem(pickableItemSubtypeBefore, pickableItemSavegameVariableBefore);
    }
}
